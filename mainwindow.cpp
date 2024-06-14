#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "dialog.h"
#include "users.h"
#include <QDirIterator>
#include <QDebug>  //for debug
#include <QFileDialog>


#include <QIODevice>

MainWindow::MainWindow(QWidget *parent)
  : QMainWindow(parent)
  , ui(new Ui::MainWindow),serial(new QSerialPort(this))
{
  ui->setupUi(this);
  ui->mainConsole_1->setPlainText("Устройство не подключено.\n");

  //setting port to default for 101/211 TMFC
  serial->setBaudRate(QSerialPort::Baud9600);
  serial->setDataBits(QSerialPort::Data8);
  serial->setParity(QSerialPort::NoParity);
  serial->setStopBits(QSerialPort::TwoStop);
  serial->setFlowControl(QSerialPort::NoFlowControl);


  //seaching current file path to Acrobat.exe
  QDirIterator it("C:/Program Files (x86)/", QStringList() << "Acrobat.exe", QDir::NoFilter, QDirIterator::Subdirectories);
  while (it.hasNext()) {
      QFile f(it.next());
      f.open(QIODevice::ReadOnly);
      this->PathToAdobe=f.fileName();
    }
  ui->PathToAdobeLine->setText(this->PathToAdobe); //refresh path in text line
}

MainWindow::~MainWindow()
{

  delete serial;
  delete ui;
}


void MainWindow::on_FolderDataBase_clicked()
{
  this->FolderNameDB = QFileDialog::getExistingDirectory(this,
                                                         tr("Open Database folder"), "./");
  ui->DbPath->setText(this->FolderNameDB);
}


void MainWindow::on_FolderTemporaryFilesPdf_clicked()
{
  this->FolderNamePdf = QFileDialog::getExistingDirectory(this,
                                                          tr("Open PDF folder"), "./");
  ui->TempPathPdf->setText(this->FolderNamePdf);
}



void MainWindow::on_FolderTemporaryFilesXlsx_clicked()
{
  this->FolderNameXlsx = QFileDialog::getExistingDirectory(this,
                                                           tr("Open Xlsx folder"), "./");
  ui->TempPathXlsx->setText(this->FolderNameXlsx);
}


void MainWindow::on_PathToAdobe_clicked()
{   //setting current folder name to adobe, if nothing to found in c:/program files (x86)/
  this->PathToAdobe = QFileDialog::getOpenFileName(this,
                                                   tr("Open Adobe path"),this->PathToAdobe ,tr("*.exe"));
  ui->PathToAdobeLine->setText(this->PathToAdobe);
}


void MainWindow::on_lineEdit_7_textChanged(const QString &arg1)
{
  this->FolderNameDB=arg1;
}


void MainWindow::on_lineEdit_8_textChanged(const QString &arg1)
{
  this->FolderNamePdf=arg1;
}


void MainWindow::on_lineEdit_9_textChanged(const QString &arg1)
{
  this->FolderNameXlsx=arg1;
}


void MainWindow::on_lineEdit_10_textChanged(const QString &arg1)
{
  this->PathToAdobe =arg1;
}

void MainWindow::on_toolButton_13_clicked()
{
  Dialog d;
  d.setWindowFlags (d.windowFlags() & ~Qt::WindowContextHelpButtonHint);//disable button "?" near close button
  d.exec();
}


void MainWindow::on_toolButton_12_clicked()
{
  g.show();
}

void MainWindow::on_UserButton_clicked()
{
  Users us;
  us.setWindowFlags (us.windowFlags() & ~Qt::WindowContextHelpButtonHint); //disable button "?" near close button
  us.exec();
}

//Button Auto connect to device
void MainWindow::on_Button_connect_clicked()
{
  auto_search_com_ports();
}

//This function is getting listed all com ports in system and goes through all list
void MainWindow::auto_search_com_ports(){

  ui->mainConsole_1->clear();
  ui->mainConsole_1->setText("Подключение к устройству...");
  QApplication::processEvents();
  ui->mainConsole_1->clear();

  const QList<QSerialPortInfo> serialPortInfos = QSerialPortInfo::availablePorts();

  for (const QSerialPortInfo &p : serialPortInfos) {
      serial->setPortName(p.portName());

      if (serial->open(QIODevice::ReadWrite)) {
          if(askSerialPort()){
              updateData();
              break;
            };
        }
    }
  ui->mainConsole_1->clear();
  ui->mainConsole_1->setText("ТМФЦ устройство не найдено.");
}

//This function is asking "Who are you?" the connected device
bool MainWindow::askSerialPort()
{
  //array hex commands for building request "Who are you?", command 57.
  /*
#0      0xF0            - to flash interrupt, ignored at MCU side
#1      0xEA,0xE5       - header
#2      comd            - command
#3      data0           - actual temperature in milliCelsius, MSB
#4      data1           - actual temperature in milliCelsius, LSB
#5      cs              - control sum = sum of bytes 1..4
*/
  if(serial->isOpen()){

      writeData(createCommand(0x39,0x00,0x00));
      serial->waitForBytesWritten(100);//this can work with auto connection using default value, but for manual connection need a set timeout

      serial->waitForReadyRead(500); //optimal time for delay before read, we have a problem, sometimes its not enouth for manual connect
      const QByteArray data = serial->readAll();

      if(data != static_cast<QByteArray>("101")&& data != static_cast<QByteArray>("211")){
          qDebug()<<serial->portName()<<" no get correct answer not given\n";
          closeSerialPort();
        }
      else{
          qDebug()<<"hello "<<data<<"\n";
          ui->Disconnected_device->hide();
          uint8_t value=1;
          QString PortName=serial->portName();
          QString numberPort="";

          for(int chr=3;chr<PortName.size();++chr){
              numberPort+=PortName[chr];
            }

          value=numberPort.toInt();
          ui->numberComPort_2->setValue(value);
          ui->textBrowser->insertPlainText("Устройство подключено. Порт "+serial->portName()+"\nМодель ТМФЦ "+ data+".\n");
          ui->Connected_device->show();
          return true;
        }
      return false;
    }
  else{
      qDebug()<<serial->portName()<<"not open\n";
      return false;
    }
}

//this function close the open COM and swap the widgets
void MainWindow::closeSerialPort()
{
  if (serial->isOpen()){
      qDebug()<<serial->portName()<<" serial was closed\n";
      ui->mainConsole_1->setText("Устройство отключено.\n");
      ui->Disconnected_device->show();
      ui->Connected_device->hide();
      ui->SNumber_text->clear();
      ui->FW_version_text->clear();
      serial->close();
    }
}

//this function sends array data to the UART device
void MainWindow::writeData(const QByteArray &data)
{
  const qint64 written = serial->write(data);
  if (written == data.size()) {
      qDebug()<<written<<" bytes was written\n";
    } else {
      qDebug()<<"Failed to write all data to port %1.\n"
                "Error: "<<serial->errorString()<<"\n";
    }
}

//auto disconnect button
void MainWindow::on_Disconnect_device_clicked()
{
  closeSerialPort();
  ui->pushButton_11->setEnabled(true);
}

//this is func for hide and show manual and auto connect buttons
void MainWindow::on_SelectPort_2_stateChanged(int arg1)
{
  if(arg1==2){
      ui->FrameComPort_2->setEnabled(true);
      ui->Button_connect->setEnabled(false);
    }
  if(arg1==0){
      ui->FrameComPort_2->setEnabled(false);
      ui->Button_connect->setEnabled(true);
    }
}

//button manual connect
void MainWindow::on_pushButton_11_clicked()
{
  ui->mainConsole_1->clear();
  ui->mainConsole_1->setText("Подключение к устройству...");
  QApplication::processEvents();
  ui->mainConsole_1->clear();

  QString str="COM"+QString::number(ui->numberComPort_2->value());
  serial->setPortName(str);
  if (serial->open(QIODevice::ReadWrite)) {
      askSerialPort();
      updateData();
    }
  else{
      qDebug()<<"can't open port\n";
      QString str=serial->portName()+ " порт не найден или отсутствует в системе.";
      ui->mainConsole_1->setText(str);
    }
}

void MainWindow::readFwVersion(){
  serial->waitForBytesWritten(1000);
  fwVersion.clear();
  if(serial->isOpen()){

      writeData(createCommand(0x3A,0x00,0x00)); //command #58, data1=0, data2=0
      serial->waitForBytesWritten(500);

      serial->waitForReadyRead(500);
      const QByteArray data = serial->readAll();

      for(const auto& it:data){
          fwVersion+=QString::number(it);
        }
      ui->FW_version_text->setText(fwVersion);

    }
  qDebug()<<"fwVersion: "<<fwVersion;
}

void MainWindow::readSnDevice(){
  serial->waitForBytesWritten(1000);

  if(serial->isOpen()){

      writeData(createCommand(0x15,0x00,0x00));
      serial->waitForBytesWritten(500);

      serial->waitForReadyRead(500);
      const QByteArray data = serial->readAll();
      deviceInfo=data;
      if(data.size()==32){
          QString str;
          for(int it=10;it<16;++it){
              str+=QString::number(data[it]);
            }
          ui->SNumber_text->setText(str);
          qDebug()<<"s.n "<<str;
        }

    }
}

QByteArray MainWindow::createCommand(uint8_t cmd, uint8_t data_1, uint8_t data_2){
  QByteArray cmd_arr;
  cmd_arr.resize(7);
  cmd_arr[0]=0xF0;
  cmd_arr[1]=0xEA;
  cmd_arr[2]=0xE5;
  cmd_arr[3]=cmd;
  cmd_arr[4]=data_1;
  cmd_arr[5]=data_2;
  cmd_arr[6]=cmd+data_1+data_2;
  return cmd_arr;
}

void MainWindow::updateData(){
  readSnDevice();
  readFwVersion();
}


void MainWindow::on_pushButton_10_clicked()
{
  dataTime=QDateTime::currentDateTime();
  ui->textBrowser->insertPlainText("Дата и время ПК: "+dataTime.toString("dd.MM.yyyy hh:mm")+"\n");
  ui->dateEdit->setDateTime(dataTime);
  ui->timeEdit->setTime(dataTime.time());
  dataTime.currentDateTimeUtc();
  ui->UTC->setText("+"+QString::number(dataTime.utcOffset()/60/60)+":00");
  qDebug()<<dataTime.toString("dd/MM/yyyy");
}


void MainWindow::on_readDateTimeFromDevice_clicked()
{
  if(serial->isOpen()){
      writeData(createCommand(0x3B,0x00,0x00));
      serial->waitForBytesWritten(500);

      serial->waitForReadyRead(1000);
      const QByteArray data = serial->readAll();
      if(data.size()==5){
      dataTime.setTime(QTime(data[0],data[1],0));
      dataTime.setDate(QDate(2000+data[4],data[3],data[2]));

      ui->textBrowser->insertPlainText("Дата и время устройства: "+dataTime.toString("dd.MM.yyyy hh:mm")+"\n");

      ui->dateEdit->setDate(dataTime.date());
      ui->timeEdit->setTime(dataTime.time());
        }
    }

}


void MainWindow::on_writeDateTimeFromDevice_clicked()
{
  if(serial->isOpen()){

      ui->textBrowser->insertPlainText("Дата и время обновлено на устройстве.\n");

      serial->waitForReadyRead(500);//timeout

      QString hour = QString::number(dataTime.time().hour());

      QString minuts = QString::number(dataTime.time().minute());

      writeData(createCommand(0x27,hour.toUInt(), minuts.toUInt()));

      serial->waitForBytesWritten(500);
      //-----------

      serial->waitForReadyRead(500);//timeout

      QString month = QString::number(dataTime.date().month());

      QString day = QString::number(dataTime.date().day());

      QString year = QString::number(dataTime.date().year());

      QByteArray year_array= year.toUtf8();

      uint8_t result = (year_array[2]-'0')*10+(year_array[3]-'0');

      writeData(createCommand(0x28,result, 0x00));

      serial->waitForBytesWritten(500);

      //------------
      serial->waitForReadyRead(500);
      writeData(createCommand(0x29,month.toUInt(), day.toUInt()));

      serial->waitForBytesWritten(100);
    }
}


void MainWindow::on_dateEdit_userDateChanged(const QDate &date)
{
    dataTime.setDate(date);
}





void MainWindow::on_pushButton_16_clicked()
{
    ui->textBrowser->clear();
}


void MainWindow::on_pushButton_18_clicked()
{
    ui->mainConsole_1->clear();
}


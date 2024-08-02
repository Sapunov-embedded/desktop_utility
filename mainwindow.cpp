#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "dialog.h"
#include "users.h"

#include <QDirIterator>
#include <QDebug>  //for debug
#include <QFileDialog>
#include <QIODevice>

MainWindow::MainWindow(SerialPortManager *SerialPM, ExportCSV *CSV,ExportDataFromBytes *parsedData, QWidget *parent)
  : QMainWindow(parent)
  , ui(new Ui::MainWindow),
    SerialPM(SerialPM),storage(DeviceInfoStorage::getInstanse()),CSV(CSV),timer(new QTimer),layout(new QVBoxLayout),parsed(parsedData),g(parsedData)
{

  ui->setupUi(this);


  ui->mainConsole_1->setPlainText("Устройство не подключено.\n");

  ui->progressBar->setMinimum(0);
  ui->progressBar->setMaximum(storage.getBlockSizeValue());

  layout->addWidget( ui->lcdDate);
  // setLayout(layout);

  layout->addWidget(ui->lcdHours);
  // setLayout(layout);

  // connect(timer, &QTimer::timeout, this, &MainWindow::on_getTempButton_clicked);
  // connect(timer, &QTimer::timeout, this, &MainWindow::set_lcd_datatime);
  connect(timer, &QTimer::timeout, this, &MainWindow::setProgressBar);
  timer->start(1000);

  connect (ui->Button_connect,&QPushButton::clicked,this,&MainWindow::on_getTempButton_clicked);
  connect (ui->Button_connect,&QPushButton::clicked,this,&MainWindow::set_lcd_datatime);

  //-------serial connect
  //Auto connect
  connect(ui->Button_connect, &QPushButton::clicked,this, [this,SerialPM](){
      ProcessConnect();

      SerialPM->auto_search_com_ports();

      if(SerialPM->getConnectStatus()){

          ShowHideConnectWindow();

        }
      else {
          ui->mainConsole_1->setText("ТМФЦ устройство не найдено.");
        }
    },Qt::AutoConnection);

  //disconnect
  connect(ui->Disconnect_device, &QPushButton::clicked,this, [this,SerialPM](){
      SerialPM->closeSerialPort();
      ui->pushButton_11->setEnabled(true);
      ui->SNumber_text->clear();
      ui->FW_version_text->clear();
      closeSerialPort();
    },Qt::AutoConnection);

  //manual connect
  connect(ui->pushButton_11, &QPushButton::clicked,this, [this,SerialPM](){
      ProcessConnect();

      SerialPM->PortNumUpdate(ui->numberComPort_2->value());

      SerialPM->on_pushButton_11_clicked();

      if (!SerialPM->getConnectStatus()) {
          qDebug()<<"can't open port\n";
          ui->mainConsole_1->setText(SerialPM->getPortName()+" порт не найден или отсутствует в системе.");
        }
      else {
          ShowHideConnectWindow();
        }
    },Qt::AutoConnection);

  //------date and time pc
  connect(ui->pushButton_10, &QPushButton::clicked,this, [this](){
      storage.setDateTime(QDateTime::currentDateTime());
      ui->textBrowser->insertPlainText("Дата и время ПК: "+storage.getDateTime().toString("dd.MM.yyyy hh:mm")+"\n");
      ui->dateEdit->setDateTime(storage.getDateTime());
      ui->timeEdit->setTime(storage.getDateTime().time());
      storage.getDateTime().currentDateTimeUtc();
      ui->UTC->setText("+"+QString::number(storage.getDateTime().utcOffset()/60/60)+":00");

    },Qt::AutoConnection);

  connect(ui->readDateTimeFromDevice, &QPushButton::clicked,this, [this,SerialPM](){
      SerialPM->on_readDateTimeFromDevice_clicked();
      ui->textBrowser->insertPlainText("Дата и время устройства: "+storage.getDateTime().toString("dd.MM.yyyy hh:mm")+"\n");

      ui->dateEdit->setDate(storage.getDateTime().date());
      ui->timeEdit->setTime(storage.getDateTime().time());
    },Qt::AutoConnection);

  connect(ui->writeDateTimeFromDevice, &QPushButton::clicked,this, [this,SerialPM](){
      SerialPM->on_writeDateTimeFromDevice_clicked();//temp
      ui->textBrowser->insertPlainText("Дата и время обновлено на устройстве.\n");
    },Qt::AutoConnection);

  connect(ui->pushButton_16, &QPushButton::clicked,this, [this](){
      ui->textBrowser->clear();
    },Qt::AutoConnection);

  connect(ui->pushButton_18, &QPushButton::clicked,this, [this](){
      ui->mainConsole_1->clear();
    },Qt::AutoConnection);


}

MainWindow::~MainWindow()
{
  delete layout;
  delete timer;
  delete ui;
}

//--------------------------------------------------------------------------
//report window
void MainWindow::on_toolButton_13_clicked()
{

  Dialog d(CSV);
  d.setWindowFlags (d.windowFlags() & ~Qt::WindowContextHelpButtonHint);//disable button "?" near close button
  d.exec();
}

//graphics window show
void MainWindow::on_toolButton_12_clicked()
{
  g.show();
}

//-------------------------------------------------------------------------


//Button Auto connect to device
void MainWindow::on_Button_connect_clicked(){};

void MainWindow::closeSerialPort()
{
  ui->mainConsole_1->setText("Устройство отключено.\n");
  ui->Disconnected_device->show();
  ui->Connected_device->hide();

}

//auto disconnect button
void MainWindow::on_Disconnect_device_clicked(){};

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
void MainWindow::on_pushButton_11_clicked(){};

void MainWindow::readFwVersion(){

  SerialPM->readFwVersion();
  ui->FW_version_text->setText(storage.getFwVersion());
  qDebug()<<"fwVersion: "<<storage.getFwVersion();
}

void MainWindow::readSnDevice(){
  ui->SNumber_text->setText(storage.getSnDevice());
}


void MainWindow::updateData(){
  readFwVersion();
  readSnDevice();

  SerialPM->getTempHumid();
}


void MainWindow::on_pushButton_10_clicked(){};


void MainWindow::on_readDateTimeFromDevice_clicked(){};


void MainWindow::on_writeDateTimeFromDevice_clicked(){};

void MainWindow::on_dateEdit_userDateChanged(const QDate &date)
{
  QDateTime dt=storage.getDateTime();
  dt.setDate(date);
  storage.setDateTime(dt);
}

void MainWindow::on_pushButton_16_clicked(){};


void MainWindow::on_pushButton_18_clicked(){};

void MainWindow::on_timeEdit_userTimeChanged(const QTime &time)
{
  QDateTime dt=storage.getDateTime();
  dt.setTime(time);
  storage.setDateTime(dt);
}

void MainWindow::ProcessConnect(){
  ui->mainConsole_1->clear();
  ui->mainConsole_1->setText("Подключение к устройству...");
  QApplication::processEvents();

};

void MainWindow::ShowHideConnectWindow(){
  ui->SNumber_text->setText(storage.getSnDevice());
  ui->FW_version_text->setText(storage.getFwVersion());
  ui->Disconnected_device->hide();
  ui->Connected_device->show();
  //------
  SerialPM->on_readDateTimeFromDevice_clicked();
  set_lcd_datatime();
  initVerificationDate();
  on_readFlash_clicked();

  ui->numberComPort_2->setValue(SerialPM->getPortNumber());
  ui->textBrowser->insertPlainText("Устройство подключено. Порт "+ SerialPM->getPortName() +"\nМодель ТМФЦ "+ storage.getModelDevice() +".\n");
};


void MainWindow::on_getTempButton_clicked()
{
  SerialPM->getTempHumid();
  ui->lcd_temp->display(storage.getTemperature());
  ui->lcd_humid->display(qRound(storage.getHumid()));
  SerialPM->on_readDateTimeFromDevice_clicked();
  set_lcd_datatime();
}

void MainWindow::set_lcd_datatime(){
  QDateTime dataTime=storage.getDateTime();

  ui->lcdDate->display(dataTime.toString("dd.MM.yyyy"));

  ui->lcdHours->display(dataTime.time().toString("HH:mm"));

  ui->dateEdit->setDate(dataTime.date());

  ui->timeEdit->setTime(dataTime.time());

}

void MainWindow::on_readFlash_clicked()
{
  SerialPM->GetControlRange();

  ui->indicate_16->setStyleSheet("background-color:rgb(255, 255, 255); ");
  ui->indicate_48->setStyleSheet("background-color:rgb(255, 255, 255); ");
  ui->indicate_112->setStyleSheet("background-color:rgb(255, 255, 255); ");
  ui->indicate_240->setStyleSheet("background-color:rgb(255, 255, 255); ");
  ui->indicate_8->setStyleSheet("background-color:rgb(255, 255, 255); ");
  ui->indicate_4->setStyleSheet("background-color:rgb(255, 255, 255); ");
  ui->indicate_2->setStyleSheet("background-color:rgb(255, 255, 255); ");
  ui->indicate_1->setStyleSheet("background-color:rgb(255, 255, 255); ");


  for(uint8_t it=1;it<=8;++it){
      if(SerialPM->controllSettings[it]&&it==1){
          ui->indicate_1->setStyleSheet("background-color:black; ");
        }

      if(SerialPM->controllSettings[it]&&it==2){
          ui->indicate_2->setStyleSheet("background-color:black; ");
        }

      if(SerialPM->controllSettings[it]&&it==3){
          ui->indicate_4->setStyleSheet("background-color:black; ");
        }


      if(SerialPM->controllSettings[it]&&it==4){
          ui->indicate_8->setStyleSheet("background-color:black; ");
        }


      if(SerialPM->controllSettings[it]&&it==5){
          ui->indicate_16->setStyleSheet("background-color:black; ");
        }

      if(SerialPM->controllSettings[it]&&it==6){
          ui->indicate_48->setStyleSheet("background-color:black; ");
        }


      if(SerialPM->controllSettings[it]&&it==7){
          ui->indicate_112->setStyleSheet("background-color:black; ");
        }

      if(SerialPM->controllSettings[it]&&it==8){
          ui->indicate_240->setStyleSheet("background-color:black; ");
        }
    }
  SerialPM->GetVolumeLevel();
  uint8_t VolumeLevel=storage.getVolumeLevel();
  qDebug()<<VolumeLevel;
  ui->VolumeLevel->setCurrentText(QString::number(VolumeLevel));
}


//range squars
void MainWindow::on_refresh_clicked()
{
  ui->indicate_16->setStyleSheet("background-color:rgb(255, 255, 255); ");
  ui->indicate_48->setStyleSheet("background-color:rgb(255, 255, 255); ");
  ui->indicate_112->setStyleSheet("background-color:rgb(255, 255, 255); ");
  ui->indicate_240->setStyleSheet("background-color:rgb(255, 255, 255); ");
  ui->indicate_8->setStyleSheet("background-color:rgb(255, 255, 255); ");
  ui->indicate_4->setStyleSheet("background-color:rgb(255, 255, 255); ");
  ui->indicate_2->setStyleSheet("background-color:rgb(255, 255, 255); ");
  ui->indicate_1->setStyleSheet("background-color:rgb(255, 255, 255); ");


  for(uint8_t it=1;it<=8;++it){
      if(SerialPM->controllSettings[it]&&it==1){
          ui->indicate_1->setStyleSheet("background-color:black; ");
        }


      if(SerialPM->controllSettings[it]&&it==2){
          ui->indicate_2->setStyleSheet("background-color:black; ");
        }

      if(SerialPM->controllSettings[it]&&it==3){
          ui->indicate_4->setStyleSheet("background-color:black; ");
        }

      if(SerialPM->controllSettings[it]&&it==4){
          ui->indicate_8->setStyleSheet("background-color:black; ");
        }

      if(SerialPM->controllSettings[it]&&it==5){
          ui->indicate_16->setStyleSheet("background-color:black; ");
        }

      if(SerialPM->controllSettings[it]&&it==6){
          ui->indicate_48->setStyleSheet("background-color:black; ");
        }

      if(SerialPM->controllSettings[it]&&it==7){
          ui->indicate_112->setStyleSheet("background-color:black; ");
        }

      if(SerialPM->controllSettings[it]&&it==8){
          ui->indicate_240->setStyleSheet("background-color:black; ");
        }
    }
}

//write to flash control settings
void MainWindow::on_WriteToFlash_clicked()
{
  SerialPM->SetControlRange();
}

//mask squers
void MainWindow::saveNewMask(uint8_t cell_number, bool checked){
  if(checked){
      SerialPM->controllSettings[cell_number]=true;
    }
  else{
      SerialPM->controllSettings[cell_number]=false;
    }
  on_refresh_clicked();
};

void MainWindow::on_indicate_240_clicked(bool checked)
{
  saveNewMask(8,checked);
}

void MainWindow::on_indicate_112_clicked(bool checked)
{
  saveNewMask(7,checked);
}

void MainWindow::on_indicate_48_clicked(bool checked)
{
  saveNewMask(6,checked);
}

void MainWindow::on_indicate_16_clicked(bool checked)
{
  saveNewMask(5,checked);
}

void MainWindow::on_indicate_8_clicked(bool checked)
{
  saveNewMask(4,checked);
}

void MainWindow::on_indicate_4_clicked(bool checked)
{
  saveNewMask(3,checked);
}

void MainWindow::on_indicate_2_clicked(bool checked)
{
  saveNewMask(2,checked);
}

void MainWindow::on_indicate_1_clicked(bool checked)
{
  saveNewMask(1,checked);
}

//write to device new settings volume level
void MainWindow::on_WriteVolumeLevel_clicked()
{
  SerialPM->SetVolumeLevel(ui->VolumeLevel->currentText().toUShort());
  SerialPM->saveSettings();

}

void MainWindow::on_VerificationDate_userDateChanged(const QDate &date)
{
 storage.setVerificationDate(date);
  endVerificationDate();
}

void MainWindow::on_pushButton_clicked()
{
  SerialPM->SetVerficationDate();

}


//while its settup higher speed 57600
void MainWindow::on_AutoSpeed_clicked()
{
  SerialPM->autoSelectBaudRate();
}

//for debug manual spped set(for sofware side)
void MainWindow::on_speed_currentIndexChanged(const QString &arg1)
{
  SerialPM->setSpeed(arg1);
}

void  MainWindow::initVerificationDate(){
  SerialPM->GetVerificationDate();
  ui->VerificationDate->setDate(storage.getVerificationDate());
  endVerificationDate();
}

bool MainWindow::validationTimeDate(){
  return true;
}

void MainWindow::on_ReadDataFromDeviceButton_clicked()
{
  SerialPM->GetAllData();
}

void MainWindow::endVerificationDate(){
  QDate dt=storage.getVerificationDate();
  int endYear=dt.year()+2;
  ui->verificationDateEnd->setText(QString::number(dt.day())+"."+QString::number(dt.month())+"."+QString::number(endYear));
};

void MainWindow::on_ReportButton_clicked()
{

      window.setWindowTitle("Export to PDF and Print Example");
      window.resize(400, 300);
      window.show();
}


void MainWindow::on_progressBar_valueChanged(int value)
{
  value=SerialPM->processBarValue;
}

void MainWindow::setProgressBar(){
  MainWindow::ui->progressBar->setValue(SerialPM->processBarValue);
};

void MainWindow::on_JournalButton_clicked()
{
  parsed->ExportServiceAndDataPoints();
  auto lol = parsed->getArrayValues();
  Journal j("Moskow","Igor&Co","igor",lol);
  j.createJournal("test_report.pdf");
}


void MainWindow::on_SaveToDataBaseButton_clicked()
{
  SerialPM->getBlockSize();
  ui->progressBar->setMaximum(storage.getBlockSizeValue());
}


void MainWindow::on_test_clicked()
{
  QByteArray arr=storage.getDataBlock();

}


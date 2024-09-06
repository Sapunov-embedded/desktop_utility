#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "dialog.h"
#include "users.h"

#include <QDirIterator>
#include <QDebug>  //for debug
#include <QFileDialog>
#include <QIODevice>

MainWindow::MainWindow(Users *user,SerialPortManager *SerialPM, ExportCSV *CSV,ExportDataFromBytes *parsedData,ApplicationConfiguration *config, QWidget *parent)
  : QMainWindow(parent),
    ui(new Ui::MainWindow),
    SerialPM(SerialPM),
    storage(DeviceInfoStorage::getInstanse()),parsed(parsedData),appConfig(config),CSV(CSV),timer(new QTimer),layout(new QVBoxLayout),g(parsedData),us(user)
{
  us->hide();
  us->setWindowFlags (us->windowFlags() & ~Qt::WindowContextHelpButtonHint);//disable button "?" near close button

  ui->setupUi(this);
  ui->mainConsole_1->setPlainText("Устройство не подключено.\n");

  ui->progressBar->setMinimum(0);
  ui->progressBar->setMaximum(storage.getBlockSizeValue());

  layout->addWidget(ui->lcdDate);
  // setLayout(layout);

  layout->addWidget(ui->lcdHours);
  // setLayout(layout);

  ui->SaveToDataBaseButton->setEnabled(false);
  ui->JournalButton->setEnabled(false);
  ui->toolButton_12->setEnabled(false);
  ui->toolButton_13->setEnabled(false);

  QString stylebutton=R"(
      QToolButton {
background-color:rgb(197, 197, 197);

border: none;
      }
      QToolButton:pressed {
background-color:rgb(197, 197, 197);
      }
      QToolButton:checked {
          background-color: black; /* Keep background color when checked */
      }
  )";

  ui->indicate_8->setStyleSheet(stylebutton);
  ui->indicate_7->setStyleSheet(stylebutton);
  ui->indicate_6->setStyleSheet(stylebutton);
  ui->indicate_5->setStyleSheet(stylebutton);
  ui->indicate_4->setStyleSheet(stylebutton);
  ui->indicate_3->setStyleSheet(stylebutton);
  ui->indicate_2->setStyleSheet(stylebutton);
  ui->indicate_1->setStyleSheet(stylebutton);

  connect(ui->indicate_8, &QToolButton::toggled, this, [this](bool checked) {
      qDebug()<<"8 "<<checked;
      saveNewMask(8,checked);
      checkControlRange();
  });
  connect(ui->indicate_7, &QToolButton::toggled, this, [this](bool checked) {
      qDebug()<<"7 "<<checked;
      saveNewMask(7,checked);
      checkControlRange();
  });
  connect(ui->indicate_6, &QToolButton::toggled, this, [this](bool checked) {
      qDebug()<<"6 "<<checked;
      saveNewMask(6,checked);
      checkControlRange();
  });
  connect(ui->indicate_5, &QToolButton::toggled, this, [this](bool checked) {
      qDebug()<<"5 "<<checked;
      saveNewMask(5,checked);
      checkControlRange();
  });
  connect(ui->indicate_4, &QToolButton::toggled, this, [this](bool checked) {
      qDebug()<<"4 "<<checked;
      saveNewMask(4,checked);
      checkControlRange();
  });
  connect(ui->indicate_3, &QToolButton::toggled, this, [this](bool checked) {
      qDebug()<<"3 "<<checked;
      saveNewMask(3,checked);
      checkControlRange();
  });
  connect(ui->indicate_2, &QToolButton::toggled, this, [this](bool checked) {
      qDebug()<<"2 "<<checked;
      saveNewMask(2,checked);
      checkControlRange();
  });
  connect(ui->indicate_1, &QToolButton::toggled, this, [this](bool checked) {
      qDebug()<<"1 "<<checked;
      saveNewMask(1,checked);
      checkControlRange();
  });


  // connect(timer, &QTimer::timeout, this, &MainWindow::on_getTempButton_clicked);
  // connect(timer, &QTimer::timeout, this, &MainWindow::set_lcd_datatime);
  connect(timer, &QTimer::timeout, this, &MainWindow::setProgressBar);
  timer->start(200);

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
      SerialPM->portNumUpdate(ui->numberComPort_2->value());
      SerialPM->manualPortConnect();
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
      SerialPM->readDateTimeFromDevice();
      ui->textBrowser->insertPlainText("Дата и время устройства: "+storage.getDateTime().toString("dd.MM.yyyy hh:mm")+"\n");

      ui->dateEdit->setDate(storage.getDateTime().date());
      ui->timeEdit->setTime(storage.getDateTime().time());
    },Qt::AutoConnection);

  connect(ui->writeDateTimeFromDevice, &QPushButton::clicked,this, [this,SerialPM](){
      SerialPM->writeDateTimeFromDevice();
      ui->textBrowser->insertPlainText("Дата и время обновлено на устройстве.\n");
    },Qt::AutoConnection);

  connect(ui->pushButton_16, &QPushButton::clicked,this, [this](){
      ui->textBrowser->clear();
    },Qt::AutoConnection);

  connect(ui->pushButton_18, &QPushButton::clicked,this, [this](){
      ui->mainConsole_1->clear();
    },Qt::AutoConnection);

  //show not active func after get block
  connect(SerialPM,&SerialPortManager::blockDataReady,this,[this](){
      ui->SaveToDataBaseButton->setEnabled(true);
      ui->JournalButton->setEnabled(true);
      ui->toolButton_12->setEnabled(true);
      ui->toolButton_13->setEnabled(true);
      ui->progressBar->setValue(ui->progressBar->maximum()); //for compensation expected and current recived data size(temp)
    });
}

MainWindow::~MainWindow()
{
  delete layout;
  delete timer;
  delete ui;
}

//--------------------------------------------------------------------------
//table report window
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
 ui->model->setText("ТМФЦ "+storage.getModelDevice());
  ui->Disconnected_device->hide();
  ui->Connected_device->show();
  //------
//  SerialPM->readDateTimeFromDevice();
  set_lcd_datatime();
  ui->lcd_temp->display(storage.getTemperature());
  ui->lcd_humid->display(qRound(storage.getHumid()));
  ui->progressBar->setMaximum(storage.getBlockSizeValue());
  qDebug()<<"block size "<<storage.getBlockSizeValue();
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
  SerialPM->readDateTimeFromDevice();
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
  on_refresh_clicked();
  uint8_t VolumeLevel=storage.getVolumeLevel();
  qDebug()<<"VolumeLevel"<<VolumeLevel;
  ui->VolumeLevel->setCurrentText(QString::number(VolumeLevel));
}


//range squars
void MainWindow::on_refresh_clicked()
{
  for(uint8_t it=1;it<=8;++it){
      if(SerialPM->controllSettings[it]&&it==1&& !ui->indicate_1->isChecked()){
          ui->indicate_1->toggle();
        }
      if(SerialPM->controllSettings[it]&&it==2&& !ui->indicate_2->isChecked()){
          ui->indicate_2->toggle();
        }
      if(SerialPM->controllSettings[it]&&it==3&& !ui->indicate_3->isChecked()){
          ui->indicate_3->toggle();
        }
      if(SerialPM->controllSettings[it]&&it==4&& !ui->indicate_4->isChecked()){
          ui->indicate_4->toggle();
        }
      if(SerialPM->controllSettings[it]&&it==5&& !ui->indicate_5->isChecked()){
          ui->indicate_5->toggle();
        }
      if(SerialPM->controllSettings[it]&&it==6&& !ui->indicate_6->isChecked()){
          ui->indicate_6->toggle();
        }
      if(SerialPM->controllSettings[it]&&it==7&& !ui->indicate_7->isChecked()){
          ui->indicate_7->toggle();
        }
      if(SerialPM->controllSettings[it]&&it==8&& !ui->indicate_8->isChecked()){
          ui->indicate_8->toggle();
        }
    }
  checkControlRange();
}

//write to flash control settings
void MainWindow::on_WriteToFlash_clicked()
{
  SerialPM->setControlRange();
}

//mask squers
void MainWindow::saveNewMask(uint8_t cell_number, bool checked){
      SerialPM->controllSettings[cell_number]=checked;
};

//write to device new settings volume level
void MainWindow::on_WriteVolumeLevel_clicked()
{
  SerialPM->setVolumeLevel(ui->VolumeLevel->currentText().toUShort());
  SerialPM->saveSettings();

}

void MainWindow::on_VerificationDate_userDateChanged(const QDate &date)
{
  storage.setVerificationDate(date);
  endVerificationDate();
}

void MainWindow::on_pushButton_clicked()
{
  SerialPM->setVerficationDate();

}


//while its settup higher speed 57600
void MainWindow::on_AutoSpeed_clicked()
{
  SerialPM->setHighBaudRate();
}

//for debug manual spped set(for sofware side)
void MainWindow::on_speed_currentIndexChanged(const QString &arg1)
{
  SerialPM->setSpeed(arg1);
}

void  MainWindow::initVerificationDate(){
 // SerialPM->getVerificationDate();
  ui->VerificationDate->setDate(storage.getVerificationDate());
  endVerificationDate();
}

bool MainWindow::validationTimeDate(){
  return true;
}

void MainWindow::on_ReadDataFromDeviceButton_clicked()
{
  ui->progressBar->setMaximum(storage.getBlockSizeValue());
  SerialPM->getAllData();
  if(ui->generateAllReports->isChecked()){
      on_JournalButton_clicked();
      on_toolButton_13_clicked();
    }
}

void MainWindow::endVerificationDate(){
  QDate dt=storage.getVerificationDate();
  int endYear=dt.year()+2;
  ui->verificationDateEnd->setText(QString::number(dt.day())+"."+QString::number(dt.month())+"."+QString::number(endYear));
};

//void MainWindow::on_ReportButton_clicked()
//{

//  window.setWindowTitle("Export to PDF and Print Example");
//  window.resize(400, 300);
//  window.show();
//}


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
  auto arr = parsed->getArrayValues();
  Journal *j = new Journal(appConfig->getCityN(),appConfig->getCompanyN(),appConfig->getResPerson(), arr);

  // Correctly connect the signal and slot
  connect(j, &Journal::JournalCreateDone, this, &MainWindow::JournalCreated);

  j->createJournal(storage.getPdfPath()+"/TMFC_"+storage.getModelDevice()+storage.getFromDateDB().toString("_dd_MM_yyyy_hh_mm")+"_"+storage.getToDateDB().toString("dd_MM_yyyy_hh_mm")+".pdf");
  delete j;
}


void MainWindow::on_SaveToDataBaseButton_clicked()
{
  us->show();
}


void  MainWindow::JournalCreated(){
   ui->textBrowser->insertPlainText("Журнал успешно сохранен\n");
};

void MainWindow::checkControlRange(){
  uint8_t startTemp=0;
  uint8_t endTemp=0;
  uint8_t startHumid=0;
  uint8_t endHumid=0;

  for(uint8_t it=1;it<=4;++it){
      if(SerialPM->controllSettings[it]&&startTemp==0){
         startTemp=it;
         endTemp=it;
        } else if(SerialPM->controllSettings[it]){
          endTemp=it;
        }
    }
  for(uint8_t it=4;it<=8;++it){
      if(SerialPM->controllSettings[it]&&startHumid==0){
         startHumid=it;
         endHumid=it;
        } else if(SerialPM->controllSettings[it]){
          endHumid=it;
        }
    }

  uint8_t tempResult=endTemp-startTemp;
  uint8_t humidResult=endHumid-startHumid;
  qDebug()<<humidResult;

  if(tempResult>0){
      for(uint8_t it = tempResult; it > 1; --it) {
          QString str = QString("indicate_%1").arg(it);
          auto button = findChild<QToolButton*>(str);
          if (button&&!button->isChecked()) {
              button->toggle();
          }
      }
    }
  if(humidResult>0){
      for(uint8_t it = humidResult; it > 1; --it) {
          QString str = QString("indicate_%1").arg(it+4);
          auto button = findChild<QToolButton*>(str);
          if (button&&!button->isChecked()) {
              button->toggle();
          }
      }
    }
}


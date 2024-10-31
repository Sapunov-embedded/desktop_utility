#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "dialog.h"
#include "users.h"

//#include <QDirIterator>
//#include <QFileDialog>
//#include <QIODevice>
#include <QDebug>  //for debug

MainWindow::MainWindow(Users *user,SerialPortManager *SerialPM,ExportDataFromBytes *parsedData,ApplicationConfiguration *config, QWidget *parent)
  : QMainWindow(parent),
    ui(new Ui::MainWindow),
    SerialPM(SerialPM),
    storage(DeviceInfoStorage::getInstanse()),parsed(parsedData),appConfig(config),timer(new QTimer),layout(new QVBoxLayout),g(parsedData),us(user)
{
  us->hide();
  us->setWindowFlags (us->windowFlags() & ~Qt::WindowContextHelpButtonHint);//disable button "?" near close button

  ui->setupUi(this);
  ui->mainConsole_1->setPlainText("Устройство не подключено.\n");

  ui->progressBar->setMinimum(0);
  ui->progressBar->setMaximum(storage.getBlockSizeValue());

  layout->addWidget(ui->lcdDate);
  layout->addWidget(ui->lcdHours);

  ui->SaveToDataBaseButton->setEnabled(false);
  ui->JournalButton->setEnabled(false);
  ui->graphButton->setEnabled(false);
  ui->tableButton->setEnabled(false);



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
      saveNewMask(8,checked);
      checkControlRange();
    });
  connect(ui->indicate_7, &QToolButton::toggled, this, [this](bool checked) {
      saveNewMask(7,checked);
      checkControlRange();
    });
  connect(ui->indicate_6, &QToolButton::toggled, this, [this](bool checked) {
      saveNewMask(6,checked);
      checkControlRange();
    });
  connect(ui->indicate_5, &QToolButton::toggled, this, [this](bool checked) {
      saveNewMask(5,checked);
      checkControlRange();
    });
  connect(ui->indicate_4, &QToolButton::toggled, this, [this](bool checked) {
      saveNewMask(4,checked);
      checkControlRange();
    });
  connect(ui->indicate_3, &QToolButton::toggled, this, [this](bool checked) {
      saveNewMask(3,checked);
      checkControlRange();
    });
  connect(ui->indicate_2, &QToolButton::toggled, this, [this](bool checked) {
      saveNewMask(2,checked);
      checkControlRange();
    });
  connect(ui->indicate_1, &QToolButton::toggled, this, [this](bool checked) {
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
      ui->manualConButton->setEnabled(true);
      ui->SNumber_text->clear();
      ui->FW_version_text->clear();
      ui->SaveToDataBaseButton->setEnabled(false);
      ui->JournalButton->setEnabled(false);
      ui->graphButton->setEnabled(false);
      ui->tableButton->setEnabled(false);
      closeSerialPort();
    },Qt::AutoConnection);

  //manual connect
  connect(ui->manualConButton, &QPushButton::clicked,this, [this,SerialPM](){
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
  connect(ui->timeFromPcButton, &QPushButton::clicked,this, [this](){
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

  connect(ui->clearConsoleDisW, &QPushButton::clicked,this, [this](){
      ui->textBrowser->clear();
    },Qt::AutoConnection);

  connect(ui->clearConcoleConW, &QPushButton::clicked,this, [this](){
      ui->mainConsole_1->clear();
    },Qt::AutoConnection);

  //show not active func after get block
  connect(SerialPM,&SerialPortManager::blockDataReady,this,[this](){
      ui->SaveToDataBaseButton->setEnabled(true);
      ui->JournalButton->setEnabled(true);
      ui->graphButton->setEnabled(true);
      ui->tableButton->setEnabled(true);
      ui->progressBar->setValue(ui->progressBar->maximum()); //for compensation expected and current recived data size(temp)
    });
  connect(SerialPM, &SerialPortManager::verificationDateRecorded,this, [this](){
      ui->textBrowser->insertPlainText("Новая дата верификации "+storage.getVerificationDate().toString("dd.MM.yyyy")+" установлена");
    },Qt::AutoConnection);
  Logging::logInfo("The app start");
  ui->Button_connect->click();

}

MainWindow::~MainWindow()
{
  delete layout;
  delete timer;
  delete ui;
   Logging::logInfo("The app was closed");
}



void MainWindow::closeSerialPort()
{
  ui->mainConsole_1->setText("Устройство отключено.\n");
  ui->Disconnected_device->show();
  ui->Connected_device->hide();
  disableAllIndicates();
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


void MainWindow::readFwVersion(){
  SerialPM->readFwVersion();
  ui->FW_version_text->setText(storage.getFwVersion());
  qDebug()<<"fwVersion: "<<storage.getFwVersion();
  Logging::logInfo("getted fwVersion: "+storage.getFwVersion().toStdString());
}

void MainWindow::readSnDevice(){
  ui->SNumber_text->setText(storage.getSnDevice());
}

void MainWindow::updateData(){
  readFwVersion();
  readSnDevice();
  SerialPM->getTempHumid();
}


void MainWindow::on_dateEdit_userDateChanged(const QDate &date)
{
  QDateTime dt=storage.getDateTime();
  dt.setDate(date);
  storage.setDateTime(dt);
}

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
  ui->progressBar->setMaximum(storage.getBlockSizeValue());
  ui->progressBar->setValue(0);
  ui->SNumber_text->setText(storage.getSnDevice());
  ui->FW_version_text->setText(storage.getFwVersion());
  ui->model->setText("ТМФЦ "+storage.getModelDevice());
  ui->Disconnected_device->hide();
  ui->Connected_device->show();
  set_lcd_datatime();
  ui->lcd_temp->display(storage.getTemperature());
  ui->lcd_humid->display(qRound(storage.getHumid()));
  ui->progressBar->setMaximum(storage.getBlockSizeValue());
  qDebug()<<"block size "<<storage.getBlockSizeValue();
  Logging::logInfo("block size "+QString::number(storage.getBlockSizeValue()).toStdString());
  initVerificationDate();
  on_readFlash_clicked();
  //-----------------------------------------------------
  if(storage.getModelDevice()==DEV_1XX){
  //    ui->interval_211_inside->setEnabled(false);
   //   ui->interval_211_outside->setEnabled(false);
   //   ui->intervals_101->setEnabled(true);
      ui->interval_211_inside->hide();
      ui->interval_211_outside->hide();
      ui->intervals_101->show();
      ui->switchSensorBox->setEnabled(false);
      storage.setSensorType211(true);
    }
  if(storage.getModelDevice()==DEV_2XX){
    //  ui->intervals_101->setEnabled(false);
    //  ui->interval_211_inside->setEnabled(true);
    //  ui->interval_211_outside->setEnabled(true);
      ui->interval_211_inside->show();
      ui->interval_211_outside->show();
      ui->intervals_101->hide();
      ui->switchSensorBox->setEnabled(true);
      auto ranges=storage.getRangeFor211();
      ui->InTempLower->setValue(std::get<0>(ranges));
      ui->InTempUpper->setValue(std::get<1>(ranges));
      ui->InHumidLower->setValue(std::get<2>(ranges));
      ui->InHumidUpper->setValue(std::get<3>(ranges));
      ui->OutTempLower->setValue(std::get<4>(ranges));
      ui->OutTempUpper->setValue(std::get<5>(ranges));
      ui->OutHumidLower->setValue(std::get<6>(ranges));
      ui->OutHumidUpper->setValue(std::get<7>(ranges));
    }
  //-----------------------------------------------------
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

// its settup higher speed 230000
void MainWindow::on_AutoSpeed_clicked()
{
  SerialPM->setHighBaudRate();
}


void  MainWindow::initVerificationDate(){
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
      emit ui->tableButton->clicked();
    }
}

void MainWindow::endVerificationDate(){
  QDate dt=storage.getVerificationDate();
  QDate dt2=dt.addYears(2);
  ui->verificationDateEnd->setText(dt2.toString("dd.MM.yyyy"));
};

//void MainWindow::on_progressBar_valueChanged(int value)
//{
//  value=SerialPM->processBarValue;
//}

void MainWindow::setProgressBar(){
  MainWindow::ui->progressBar->setValue(SerialPM->processBarValue);
};

void MainWindow::on_JournalButton_clicked()
{
  parsed->ExportServiceAndDataPoints();
  auto arr = parsed->getArrayValues();
  Journal *j = new Journal(appConfig->getCityN(),appConfig->getCompanyN(),appConfig->getResPerson(), arr);

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
  for(uint8_t it=5;it<=8;++it){
      if(SerialPM->controllSettings[it]&&startHumid==0){
          startHumid=it;
          endHumid=it;
        } else if(SerialPM->controllSettings[it]){
          endHumid=it;
        }
    }

  uint8_t tempResult=endTemp-startTemp;
  uint8_t humidResult=endHumid-startHumid;

  if(tempResult>1){
      for(uint8_t it = endTemp; it !=1 ; --it) {
          QString str = QString("indicate_%1").arg(it);
          auto button = findChild<QToolButton*>(str);
          if (button&&!button->isChecked()) {
              button->toggle();
            }
        }
    }
  if(humidResult>1){
      for(uint8_t it = endHumid; it != 5; --it) {
          QString str = QString("indicate_%1").arg(it);
          auto button = findChild<QToolButton*>(str);
          if (button&&!button->isChecked()) {
              button->toggle();
            }
        }
    }
}

void MainWindow::updateRangeValues(){
  int8_t InTempLowerControl=ui->InTempLower->value();

  int8_t InTempUpperControl=ui->InTempUpper->value();

  if(InTempUpperControl<InTempLowerControl){
      InTempUpperControl=InTempLowerControl;
      ui->InTempUpper->setValue(InTempUpperControl);
    }

  uint8_t InHumidLowerControl=ui->InHumidLower->value();

  uint8_t InHumidUpperControl=ui->InHumidUpper->value();

  if(InHumidUpperControl<InHumidLowerControl){
      InHumidUpperControl=InHumidLowerControl;
      ui->InHumidUpper->setValue(InHumidLowerControl);
    }

  int8_t OutTempLowerControl=ui->OutTempLower->value();

  int8_t OutTempUpperControl=ui->OutTempUpper->value();

  if(OutTempUpperControl<OutTempLowerControl){
      OutTempUpperControl=OutTempLowerControl;
      ui->OutTempUpper->setValue(OutTempLowerControl);
    }

  uint8_t OutHumidLowerControl=ui->OutHumidLower->value();

  uint8_t OutHumidUpperControl=ui->OutHumidUpper->value();

  if(OutHumidUpperControl<OutHumidLowerControl){
      OutHumidUpperControl=OutHumidLowerControl;
      ui->OutHumidUpper->setValue(OutHumidLowerControl);
    }

  storage.setRangeFor211(InTempLowerControl,InTempUpperControl,InHumidLowerControl,InHumidUpperControl,
                         OutTempLowerControl,OutTempUpperControl,OutHumidLowerControl,OutHumidUpperControl);
};

void MainWindow::on_InTempLower_valueChanged(int arg1)
{
  updateRangeValues();
}

void MainWindow::on_InHumidLower_valueChanged(int arg1)
{
  updateRangeValues();
}

void MainWindow::on_InTempUpper_valueChanged(int arg1)
{
  updateRangeValues();
}

void MainWindow::on_OutTempLower_valueChanged(int arg1)
{
  updateRangeValues();
}

void MainWindow::on_OutHumidLower_valueChanged(int arg1)
{
  updateRangeValues();
}

void MainWindow::on_OutTempUpper_valueChanged(int arg1)
{
  updateRangeValues();
}

void MainWindow::on_OutHumidUpper_valueChanged(int arg1)
{
  updateRangeValues();
}

void MainWindow::on_InHumidUpper_valueChanged(int arg1)
{
  updateRangeValues();
}

void MainWindow::on_setVerDate_clicked()
{
  SerialPM->setVerificationDate();
}

void MainWindow::on_graphButton_clicked()
{
  g.show();
}

void MainWindow::on_tableButton_clicked()
{
  Dialog d(this);
  d.setWindowFlags (d.windowFlags() & ~Qt::WindowContextHelpButtonHint);//disable button "?" near close button
  d.exec();
}




void MainWindow::on_externalSensorButton_toggled(bool checked)
{
    storage.setSensorType211(!checked);
}

void MainWindow::disableAllIndicates(){
  ui->indicate_8->setChecked(false);
  ui->indicate_7->setChecked(false);
  ui->indicate_6->setChecked(false);
  ui->indicate_5->setChecked(false);
  ui->indicate_4->setChecked(false);
  ui->indicate_3->setChecked(false);
  ui->indicate_2->setChecked(false);
  ui->indicate_1->setChecked(false);
};

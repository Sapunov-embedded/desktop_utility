#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "dialog.h"
#include "users.h"

#include <QDebug>

#define INNER 1
#define OUTER 0


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
  ui->progressBar_for_journal->hide();
  ui->progressBar_for_journal->setMinimum(0);
  ui->progressBar_for_journal->setMaximum(storage.getBlockSizeValue());



  layout->addWidget(ui->lcdDate);
  layout->addWidget(ui->lcdHours);

  ui->SaveToDataBaseButton->setEnabled(false);
  if(enableSaveToDb){
      ui->SaveToDataBaseButton->show();
    }else{
      ui->SaveToDataBaseButton->hide();
    }

  ui->JournalButton->setEnabled(false);
  ui->graphButton->setEnabled(false);
  ui->tableButton->setEnabled(false);


  ui->innerTempComboBox->addItems(tempList);

  ui->innerHumidComboBox->addItems(humidList);
  ui->outerTempComboBox->addItems(tempList);
  ui->outerHumidComboBox->addItems(humidList);

  if(enableMainFunctionality){
      ui->mainTargetRadioButton->setChecked(enableMainFunctionality);
      ui->switchSensorBoxDisWin->setEnabled(false);
    }else{
      ui->journalTargetRadioButton->setChecked(true);
      ui->switchSensorBoxDisWin->setEnabled(true);
    }
  matchValueInRange();

  connect(ui->innerTempComboBox, QOverload<int>::of(&QComboBox::currentIndexChanged),
          [this](){ rangeProcess();});
  connect(ui->innerHumidComboBox, QOverload<int>::of(&QComboBox::currentIndexChanged),
          [this](){ rangeProcess() ;});
  connect(ui->outerTempComboBox, QOverload<int>::of(&QComboBox::currentIndexChanged),
          [this](){ rangeProcess(); });
  connect(ui->outerHumidComboBox, QOverload<int>::of(&QComboBox::currentIndexChanged),
          [this](){ rangeProcess(); });

  /**
 * This "connects" show the type of sensor that was chosen
 */
  connect(ui->innerTempComboBox,&CustomComboBox::popupOpened,[this](){
      setActiveSensorDisplay(INNER);
      if(storage.getModelDevice()==DEV_1XX)refreash102();
    });
  connect(ui->innerHumidComboBox,&CustomComboBox::popupOpened,[this](){
      setActiveSensorDisplay(INNER);
      if(storage.getModelDevice()==DEV_1XX)refreash102();
    });
  connect(ui->outerTempComboBox,&CustomComboBox::popupOpened,[this](){
      setActiveSensorDisplay(OUTER);
    });
  connect(ui->outerHumidComboBox,&CustomComboBox::popupOpened,[this](){
      setActiveSensorDisplay(OUTER);
    });

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

  //auto update lcd screen
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
          if(enableMainFunctionality){
              ShowHideConnectWindow();
            }else{
              onlyJournalFunc();
            }
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
          if(enableMainFunctionality){
              ShowHideConnectWindow();
            }else{
              onlyJournalFunc();
            }
        }
    },Qt::AutoConnection);

  //------date and time pc
  connect(ui->timeFromPcButton, &QPushButton::clicked,this, [this](){
      storage.setDateTime(QDateTime::currentDateTime());
      ui->textBrowser->insertPlainText("Дата и время ПК: "+storage.getDateTime().toString("dd.MM.yyyy hh:mm")+"\n");
      ui->dateEdit->setDateTime(storage.getDateTime());
      ui->timeEdit->setTime(storage.getDateTime().time());
      storage.getDateTime().currentDateTimeUtc();


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
      setBackgroundButton(ui->writeDateTimeFromDevice,false);
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
      ui->progressBar_for_journal->setValue(ui->progressBar->maximum());
    });
  connect(SerialPM, &SerialPortManager::verificationDateRecorded,this, [this](){
      ui->textBrowser->insertPlainText("Новая дата верификации "+storage.getVerificationDate().toString("dd.MM.yyyy")+" установлена");
    },Qt::AutoConnection);
  Logging::logInfo("The app start");
  ui->Button_connect->click();
  setActiveSensorDisplay(INNER);
  // matchValueInRange();
  if(storage.getModelDevice()==DEV_1XX){
      ui->switchSensorBoxDisWin->hide();
    }
  setBackgroundButton(ui->setVerDate,false);
  setBackgroundButton(ui->writeDateTimeFromDevice,false);
  setBackgroundButton(ui->WriteVolumeLevel,false);
  setBackgroundButton(ui->WriteToFlash,false);
  ui->progressBar->hide();
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
  setBackgroundButton(ui->writeDateTimeFromDevice,true);
}

void MainWindow::on_timeEdit_userTimeChanged(const QTime &time)
{
  QDateTime dt=storage.getDateTime();
  dt.setTime(time);
  storage.setDateTime(dt);
  setBackgroundButton(ui->writeDateTimeFromDevice,true);
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
  ui->OuterFrame->hide();
  //-----------------------------------------------------
  if(storage.getModelDevice()==DEV_1XX){
      ui->intervals_101->show();
      ui->switchSensorBox->hide();
      storage.setSensorType211(true);
      ui->settingsOuterSensor->hide();
      ui->controlLabel->show();
      ui->interval_211_inside->setEnabled(false);

      updateBoxRange102();
    }
  if(storage.getModelDevice()==DEV_2XX){
      ui->controlLabel->hide();
      ui->settingsOuterSensor->show();
      ui->intervals_101->hide();
      ui->interval_211_inside->setEnabled(true);
      ui->interval_211_outside->show();
      ui->intervals_101->hide();
      ui->switchSensorBox->show();
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
  setBackgroundButton(ui->setVerDate,false);
  setBackgroundButton(ui->writeDateTimeFromDevice,false);
  setBackgroundButton(ui->WriteVolumeLevel,false);
  setBackgroundButton(ui->WriteToFlash,false);
};


void MainWindow::on_getTempButton_clicked()
{
  SerialPM->getTempHumid();
  ui->lcd_temp->display(storage.getTemperature());
  ui->lcd_humid->display(qRound(storage.getHumid()));
  SerialPM->readDateTimeFromDevice();
  set_lcd_datatime();
  setBackgroundButton(ui->writeDateTimeFromDevice,false);
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
  if(storage.getModelDevice()==DEV_1XX)  on_refresh_clicked();
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
  if(storage.getModelDevice()==DEV_1XX){
      updateBoxRange102();
      ui->textBrowser->insertPlainText("Диапозоны контроля: от "+QString::number(ui->InTempLower->value())+" до "+QString::number(ui->InTempUpper->value())+" °C, от "+
                                       QString::number(ui->InHumidLower->value())+" до "+ QString::number(ui->InHumidUpper->value())+"%\n");
    }else{
      auto ranges=storage.getRangeFor211();
      ui->textBrowser->insertPlainText("Диапозоны контроля:\nВнутренний датчик: от " + QString::number(std::get<0>(ranges))+"..."+QString::number(std::get<1>(ranges))+" °C, "+
                                       QString::number(std::get<2>(ranges))+"..."+QString::number(std::get<3>(ranges))+" %\n");
      ui->textBrowser->insertPlainText("Внешний датчик: от "+QString::number(std::get<4>(ranges))+ "..."+QString::number(std::get<5>(ranges))+" °C, "+
                                       QString::number(std::get<6>(ranges))+ "..." +QString::number(std::get<7>(ranges))+"% установлены.\n");
    }
  // matchValueInRange();
  if(storage.getModelDevice()==DEV_1XX){
      // updateBoxRange102();
    }else{
    }
  setBackgroundButton(ui->WriteToFlash,false);
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
  setBackgroundButton(ui->WriteVolumeLevel,false);
}

void MainWindow::on_VerificationDate_userDateChanged(const QDate &date)
{
  storage.setVerificationDate(date);
  endVerificationDate();
  setBackgroundButton(ui->setVerDate,true);
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
  SerialPM->processBarValue=0;
  ui->progressBar->show();
  ui->progressBar_for_journal->setMaximum(storage.getBlockSizeValue());
  SerialPM->getAllData();
  if(ui->generateAllReports->isChecked()){
      on_JournalButton_clicked();
      emit ui->tableButton->clicked();
    }
  ui->progressBar->hide();
}

void MainWindow::endVerificationDate(){
  QDate dt=storage.getVerificationDate();
  QDate dt2=dt.addYears(2);
  ui->verificationDateEnd->setText(dt2.toString("dd.MM.yyyy"));
};


void MainWindow::setProgressBar(){
  MainWindow::ui->progressBar->setValue(SerialPM->processBarValue);
  MainWindow::ui->progressBar_for_journal->setValue(SerialPM->processBarValue);
};

void MainWindow::on_JournalButton_clicked()
{
  parsed->ExportServiceAndDataPoints();
  auto arr = parsed->getArrayValues();
  Journal *j = new Journal(appConfig->getCityN(),appConfig->getCompanyN(),appConfig->getResPerson(), arr);

  connect(j, &Journal::JournalCreateDone, this, &MainWindow::JournalCreated);

  j->createJournal(storage.getPdfPath()+"/TMFC_"+storage.getModelDevice()+storage.getFromDateDB().toString("_dd_MM_yyyy_hh_mm")+"_"+storage.getToDateDB().toString("dd_MM_yyyy_hh_mm"));
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
  setBackgroundButton(ui->WriteToFlash,true);
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

  //--
  std::pair<int, int> tempRange={0,0};
  std::pair<int, int> humidRange={0,0};
  if(startTemp!=0&&endTemp!=0){
      tempRange=get_combined_range(startTemp,endTemp,true);
      humidRange=get_combined_range(startHumid,endHumid,false);
    }
  //  QVector<QPair<int, int>> tempRanges = {
  //    {-40, -18}, {-18, -5}, {2, 8}, {2, 15}, {2, 25}, {2, 30}, {8, 15}, {8, 25}, {15, 25}, {2, 70}
  //  };
  //  QVector<QPair<int, int>> humidRanges = {
  //    {20, 45}, {20, 50}, {20, 60}, {20, 65}
  //  };

  //--

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
  setBackgroundButton(ui->WriteToFlash,true);
};

void MainWindow::on_InTempLower_valueChanged(int arg1)
{
  updateRangeValues();
  setActiveSensorDisplay(INNER);
}

void MainWindow::on_InHumidLower_valueChanged(int arg1)
{
  updateRangeValues();
  setActiveSensorDisplay(INNER);
}

void MainWindow::on_InTempUpper_valueChanged(int arg1)
{
  updateRangeValues();
  setActiveSensorDisplay(INNER);
}

void MainWindow::on_OutTempLower_valueChanged(int arg1)
{
  updateRangeValues();
  setActiveSensorDisplay(OUTER);
}

void MainWindow::on_OutHumidLower_valueChanged(int arg1)
{
  updateRangeValues();
  setActiveSensorDisplay(OUTER);
}

void MainWindow::on_OutTempUpper_valueChanged(int arg1)
{
  updateRangeValues();
  setActiveSensorDisplay(OUTER);
}

void MainWindow::on_OutHumidUpper_valueChanged(int arg1)
{
  updateRangeValues();
  setActiveSensorDisplay(OUTER);
}

void MainWindow::on_InHumidUpper_valueChanged(int arg1)
{
  updateRangeValues();
  setActiveSensorDisplay(INNER);
}

void MainWindow::on_setVerDate_clicked()
{
  SerialPM->setVerificationDate();
  setBackgroundButton(ui->setVerDate,false);
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

void MainWindow::setActiveSensorDisplay(bool isInner){
  if(isInner){
      ui->InnerFrame->show();
      ui->OuterFrame->hide();
    }else{
      ui->InnerFrame->hide();
      ui->OuterFrame->show();
    }
};

void MainWindow::rangeProcess(){
  uint8_t inTempIndex = ui->innerTempComboBox->currentIndex();
  uint8_t inHumidIndex = ui->innerHumidComboBox->currentIndex();
  uint8_t exTempIndex = ui->outerTempComboBox->currentIndex();
  uint8_t exHumidIndex = ui->outerHumidComboBox->currentIndex();
  auto inTempValues=tempProcess(inTempIndex);
  auto inHumidValues=humidProcess(inHumidIndex);
  std::pair<int8_t,int8_t>exTempValues;
  std::pair<int8_t,int8_t>exHumidValues;
  if(storage.getModelDevice()!=DEV_1XX){
      exTempValues=tempProcess(exTempIndex);
      exHumidValues=humidProcess(exHumidIndex);
    }
  ui->InTempLower->setValue(inTempValues.first);
  ui->InTempUpper->setValue(inTempValues.second);
  ui->InHumidLower->setValue(inHumidValues.first);
  ui->InHumidUpper->setValue(inHumidValues.second);

  ui->OutTempLower->setValue(exTempValues.first);
  ui->OutTempUpper->setValue(exTempValues.second);
  ui->OutHumidLower->setValue(exHumidValues.first);
  ui->OutHumidUpper->setValue(exHumidValues.second);

  if(storage.getModelDevice()==DEV_1XX){
      setNewMaskRange102(inTempValues.first,inTempValues.second,inHumidValues.first,inHumidValues.second);
    }
      setBackgroundButton(ui->WriteToFlash,true);
};

std::pair<int8_t,int8_t> MainWindow::tempProcess(uint8_t index){
  int8_t tempL=0;
  int8_t tempH=0;
  switch (index) {
    case 0 : {
        if(storage.getModelDevice()==DEV_1XX){
            tempL=2;
            tempH=8;
            ui->textBrowser->insertPlainText("В данном устройстве этот диапозон не доступен.\nБыл установлен минимальный диапозон для данного устройства.\n");
            break;
          }else{
            tempL=-40;
            tempH=-18;
            break;
          }
      }
    case 1 : {
        if(storage.getModelDevice()==DEV_1XX){
            tempL=2;
            tempH=8;
            ui->textBrowser->insertPlainText("В данном устройстве этот диапозон не доступен.\nБыл установлен минимальный диапозон для данного устройства.\n");
            break;
          }else{
            tempL=-18;
            tempH=-5;
            break;
          }
      }
    case 2 : {
        tempL=2;
        tempH=8;
        break;
      }
    case 3 : {
        tempL=2;
        tempH=15;
        break;
      }
    case 4 : {
        tempL=2;
        tempH=25;
        break;
      }
    case 5 : {
        tempL=2;
        tempH=30;
        break;
      }
    case 6 : {
        tempL=8;
        tempH=15;
        break;
      }
    case 7 : {
        tempL=8;
        tempH=25;
        break;
      }
    case 8 : {
        tempL=15;
        tempH=25;
        break;
      }
    case 9 : {
        tempL=2;
        tempH=70;
        break;
      }

    }
  return std::make_pair(tempL,tempH);
};

std::pair<uint8_t,uint8_t> MainWindow::humidProcess(uint8_t index){
  uint8_t humidL=0;
  uint8_t humidH=0;

  if(storage.getModelDevice()==DEV_1XX){
      humidL=20;
    }else{
      humidL=0;
    }
  switch (index) {
    case 0 : {
        humidH=30;
        break;
      }
    case 1 : {
        humidH=50;
        break;
      }
    case 2 : {
        humidH=65;
        break;
      }
    case 3 : {
        humidH=80;
        break;
      }
    }
  return std::make_pair(humidL,humidH);
};


void MainWindow::matchValueInRange(){
  //-40...-18*, -18..-5*,+2..+8,+2..+15, +2..+25, +2..+30, +8..+15, +8..+25, +15..+25, +2..+70
  //0..45, 0..50, 0..60*, 0..65
  //* Range is not avalible in 102 version
  QVector<QPair<int, int>> tempRanges = {
    {-40, -18}, {-18, -5}, {2, 8}, {2, 15}, {2, 25}, {2, 30}, {8, 15}, {8, 25}, {15, 25}, {2, 70}
  };
  QVector<QPair<int, int>> humidRanges = {
    {0, 30}, {0, 50}, {0, 65}, {0, 80}
  };

  int indexInnerTemp=findBestRangeIndex(ui->InTempLower->value(),ui->InTempUpper->value(),tempRanges);
  int indexInnerHumid=findBestRangeIndex(static_cast<int>(ui->InHumidLower->value()),static_cast<int>(ui->InHumidUpper->value()),humidRanges);
  int indexOuterTemp=findBestRangeIndex(ui->OutTempLower->value(),ui->OutTempUpper->value(),tempRanges);
  int indexOuterHumid=findBestRangeIndex(static_cast<int>(ui->OutHumidLower->value()),static_cast<int>(ui->OutHumidUpper->value()),humidRanges);

  ui->innerTempComboBox->setCurrentIndex(std::move(indexInnerTemp));
  ui->innerHumidComboBox->setCurrentIndex(std::move(indexInnerHumid));
  ui->outerTempComboBox->setCurrentIndex(std::move(indexOuterTemp));
  ui->outerHumidComboBox->setCurrentIndex(std::move(indexOuterHumid));
};

// Function to find the most suitable range index for given TempL and TempH values
int findBestRangeIndex(int lowerValue, int upperValue, const QVector<QPair<int, int>>& ranges) {
  int bestIndex = -1;
  int largestRangeSize = -1;

  for (int i = 0; i < ranges.size(); ++i) {
      int rangeL = ranges[i].first;
      int rangeH = ranges[i].second;

      // Check if the range is fully within lowerValue and upperValue bounds
      if (lowerValue <= rangeL && upperValue >= rangeH) {
          int rangeSize = rangeH - rangeL;
          // Choose the range with the largest size that fits within the bounds
          if (rangeSize > largestRangeSize) {
              largestRangeSize = rangeSize;
              bestIndex = i;
            }
        }
    }

  // If no range was fully contained, fall back to the closest by boundary
  if (bestIndex == -1) {
      int minDiff = std::numeric_limits<int>::max();
      for (int i = 0; i < ranges.size(); ++i) {
          int rangeL = ranges[i].first;
          int rangeH = ranges[i].second;
          int diff = std::min(std::abs(lowerValue - rangeL), std::abs(upperValue - rangeH));
          if (diff < minDiff) {
              minDiff = diff;
              bestIndex = i;
            }
        }
    }

  return bestIndex;
}

std::pair<int, int> get_combined_range(int start, int end, bool isTemp) {
  // Define the mapping for each range
  std::unordered_map<int, std::pair<int, int>> range_map;
  if(isTemp){
      range_map = {
        {1, {2, 8}},
        {2, {8, 15}},
        {3, {15, 25}},
        {4, {25, 30}}
      };
    }else{
      range_map = {
        {5, {20, 30}},
        {6, {30, 50}},
        {7, {50, 65}},
        {8, {65, 80}}
      };
    }
  int lower_bound=0;
  int upper_bound=0;
  // Validate input
  if (range_map.find(start) != range_map.end() || range_map.find(end) != range_map.end()) {
      lower_bound = range_map[start].first;
      upper_bound = range_map[end].second;
    }
  if (start > end) {
      throw std::invalid_argument("Start value must not be greater than end value");
    }

  return {lower_bound, upper_bound};
}

void MainWindow::setNewMaskRange102(int tempL, int tempH, int humidL, int humidH){
  QVector<int>range={2,8,15,25,30};
  QVector<int>hRange={30,50,65,80};
  int start_index=0;
  int end_index=0;
  for(int i=0;i<5;++i){
      if(range[i]==tempL){
          start_index=i;
          break;
        }
    }
  for(int i=0;i<5;++i){
      if(range[i]==tempH){
          end_index=i;
          break;
        }
    }
  for(int i=start_index;i!=end_index;++i){
      saveNewMask(i+1,true);
      qDebug()<<"temp item "<<i;
    }
  if(humidH==60)humidH-=10;

  saveNewMask(5,true);
  for(int i=0;i<4;++i){
      if(humidH==hRange[i]){
          saveNewMask(i+5,true);
          qDebug()<<"humid item "<<i;
        }
    }

  on_refresh_clicked();
}

void MainWindow::refreash102(){
  for(uint8_t it = 1; it <9 ; ++it) {
      QString str = QString("indicate_%1").arg(it);
      auto button = findChild<QToolButton*>(str);
      if (button&&button->isChecked()) {
          button->toggle();
        }
    }
};

void MainWindow::updateBoxRange102(){
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

 // uint8_t tempResult=endTemp-startTemp;
 // uint8_t humidResult=endHumid-startHumid;

  //--
  std::pair<int, int> tempRange={0,0};
  std::pair<int, int> humidRange={0,0};
  if(startTemp!=0&&endTemp!=0){
      tempRange=get_combined_range(startTemp,endTemp,true);
      humidRange=get_combined_range(startHumid,endHumid,false);
    }
  QVector<QPair<int, int>> tempRanges = {
    {-40, -18}, {-18, -5}, {2, 8}, {2, 15}, {2, 25}, {2, 30}, {8, 15}, {8, 25}, {15, 25}, {2, 70}
  };
  QVector<QPair<int, int>> humidRanges = {
    {20, 30}, {20, 50}, {20, 65}, {20, 80}
  };
  if(storage.getModelDevice()==DEV_1XX&&tempRange.first!=0&&tempRange.second!=0){
      ui->innerTempComboBox->setCurrentIndex(findBestRangeIndex(tempRange.first,tempRange.second,tempRanges));
      ui->innerHumidComboBox->setCurrentIndex(findBestRangeIndex(humidRange.first,humidRange.second, humidRanges));
      ui->InTempLower->setValue(tempRange.first);
      ui->InTempUpper->setValue(tempRange.second);
      ui->InHumidLower->setValue(humidRange.first);
      ui->InHumidUpper->setValue(humidRange.second);
    }
};

void MainWindow::on_mainTargetRadioButton_clicked()
{
  enableMainFunctionality=true;
  ui->switchSensorBoxDisWin->setEnabled(false);
}




void MainWindow::onlyJournalFunc(){
  if(storage.getModelDevice()==DEV_1XX){
      on_innerlSensorButtonDisWin_clicked();
    }
  ui->mainConsole_1->clear();
  ui->mainConsole_1->insertPlainText("Загрузка данных с устройства...\n");
  ui->progressBar_for_journal->setValue(0);
  ui->progressBar_for_journal->show();
  on_ReadDataFromDeviceButton_clicked();
  ui->mainConsole_1->clear();
  ui->mainConsole_1->insertPlainText("Создание журнала...\n");
  on_JournalButton_clicked();
  ui->progressBar_for_journal->hide();
  ui->mainConsole_1->clear();
  ui->mainConsole_1->insertPlainText("Журнал успешно сохранен.");
};
void MainWindow::on_journalTargetRadioButton_clicked()
{
  enableMainFunctionality=false;
  ui->switchSensorBoxDisWin->setEnabled(true);
}


void MainWindow::on_externalSensorButtonDisWin_clicked()
{
  storage.setSensorType211(false);
}


void MainWindow::on_innerlSensorButtonDisWin_clicked()
{
  storage.setSensorType211(true);
}

void MainWindow::setBackgroundButton(QPushButton *button, bool needAction){
  if(needAction){
      button->setStyleSheet("background-color: #FFEB3B; color: black; border: 2px solid #FBC02D;  transition: all 0.3s ease;");

    }else{
      button->setStyleSheet("");
    }
};

void MainWindow::on_VolumeLevel_currentIndexChanged(const QString &arg1)
{
  setBackgroundButton(ui->WriteVolumeLevel,true);
}


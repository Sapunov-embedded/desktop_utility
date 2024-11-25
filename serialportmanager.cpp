#include "serialportmanager.h"

#include <QDebug>//for debug


SerialPortManager::SerialPortManager():serial(new QSerialPort(this)),storage(DeviceInfoStorage::getInstanse())
{
  //setting port to default for 101/211 TMFC
  serial->setBaudRate(QSerialPort::Baud9600);
  serial->setDataBits(QSerialPort::Data8);
  serial->setParity(QSerialPort::NoParity);
  serial->setStopBits(QSerialPort::TwoStop);
  serial->setFlowControl(QSerialPort::NoFlowControl);
  connect(serial, &QSerialPort::readyRead, this, &SerialPortManager::handleReadyRead);
};


SerialPortManager::~SerialPortManager()
{
  delete serial;
};


//This function is asking "Who are you?" the connected device
//array hex commands for building request "Who are you?", command 57.
/*
#0      0xF0            - to flash interrupt, ignored at MCU side
#1      0xEA,0xE5       - header
#2      comd            - command
#3      data0           - actual temperature in milliCelsius, MSB
#4      data1           - actual temperature in milliCelsius, LSB
#5      cs              - control sum = sum of bytes 1..4
*/
bool SerialPortManager::askSerialPort()
{
  if(serial->isOpen()){
      receivedData.clear();
      writeData(createCommand(0x15,0x00,0x00));
      serial->waitForBytesWritten(300);

      //* ------------update
      QEventLoop loop;
      connect(this, &SerialPortManager::dataReady, &loop, &QEventLoop::quit);
      QTimer::singleShot(500, &loop, &QEventLoop::quit);
      loop.exec();
      //*--------------update

      QString deviceName;
      storage.setDeviceInfo(receivedData);
      QByteArray deviceInfo=storage.getDeviceInfo();

      for(uint8_t it=0;it<3;++it){
          deviceName+=deviceInfo[it];
        }

      storage.setModelDevice(deviceName);

      if(deviceName != DEV_1XX && deviceName != DEV_2XX){
          qDebug()<<serial->portName()<<" no get correct answer not given\n";
          Logging::logWarning(serial->portName().toStdString()+" no get correct answer not given");
          closeSerialPort();
          return false;
        }
      else{
          qDebug()<<"hello "<<deviceName<<"\n";
          Logging::logInfo(serial->portName().toStdString()+" connected device "+deviceName.toStdString());
          QString PortName=serial->portName();
          QString numberPort="";

          for(int chr=3;chr<PortName.size();++chr){
              numberPort+=PortName[chr];
            }
          return true;
        }
    }
  else{
      return false;
    }
};

void SerialPortManager::auto_search_com_ports(){
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
  Logging::logWarning("No one devices in the system not given correct answer");
};

//connect button on manual connection section
void SerialPortManager::manualPortConnect(){
  if (serial->open(QIODevice::ReadWrite)) {
      if(askSerialPort()){
          updateData();
        }
      else{
          qDebug()<<"can't open port\n";
          Logging::logWarning("Manual chosed port is not response");
        }
    }
};

void SerialPortManager::closeSerialPort(){
  if (serial->isOpen()){
      qDebug()<<serial->portName()<<" serial was closed\n";
      Logging::logInfo(serial->portName().toStdString()+" was closed");
      serial->close();
      statusUpdate(false);
    }
};

//prepared query to send to device
void SerialPortManager::writeData(const QByteArray &data){
  const qint64 written = serial->write(data);//maybe we can use less capacity value
  if (written == data.size()) {
      qDebug()<<written<<" bytes was written";
      Logging::logInfo(QString::number(written).toStdString()+" bytes written to port");
    } else {
      qDebug()<<"Failed to write all data to port %1.\n"
                "Error: "<<serial->errorString()<<"\n";
      Logging::logError("Failed to write all data to"+serial->portName().toStdString()+"Error: "+serial->errorString().toStdString());
    }
};


//-------------------------------------------------------------
//separate command to get fw
void SerialPortManager::readFwVersion(){
  if(serial->isOpen()){
      QString fwVersion;
      delay(300);
      writeData(createCommand(0x3D,0x00,0x00)); //command #58, data1=0, data2=0
      serial->waitForBytesWritten(500);

      //----------------update
      QEventLoop loop;
      connect(this, &SerialPortManager::dataReady, &loop, &QEventLoop::quit);
      QTimer::singleShot(500, &loop, &QEventLoop::quit);
      loop.exec();
      //----------------update

      if(receivedData.size()==5){
          for(const char& it:qAsConst(receivedData)){
              fwVersion+=QString::number(it);
            }
          storage.setFwVersion(fwVersion);
          repeatTimes=0;
        }
      else{
          if(repeatTimes<10){
              qDebug()<<"fw getting fail, repeat";
              Logging::logWarning("fw getting fail, repeat for get");
              receivedData.clear();
              ++repeatTimes;
              delay(500);
              readFwVersion();
            }else{
              Logging::logWarning("fw getting fail, tryed 10 times");
            }
        }
      qDebug()<<"fw: "<<fwVersion;
    }
};

//s.n included in answear from device(question "who are you?")
void SerialPortManager::readSnDevice(){
  if(serial->isOpen()){
      QByteArray deviceInfo= storage.getDeviceInfo();
      if(deviceInfo.size()==32){
          QString str;
          for(int it=10;it<16;++it){
              str+=QString::number(deviceInfo[it]);
            }
          storage.setSnDevice(str);
          qDebug()<<"s.n "<<str;
        }
    }
};

//----------------------------------------------------------
//build correct request for send to device
QByteArray SerialPortManager::createCommand(uint8_t cmd, uint8_t data_1, uint8_t data_2){
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
};

//The list of func for filling begin information from device
void SerialPortManager::updateData(){
  receivedData.clear();
  readSnDevice();
  receivedData.clear();
  readFwVersion();
  receivedData.clear();
  getTempHumid();
  receivedData.clear();
  getVerificationDate();
  receivedData.clear();
  getVolumeLevel();
  receivedData.clear();
  getControlRange();
  receivedData.clear();
  getBlockSize();
  receivedData.clear();
  readDateTimeFromDevice();
  receivedData.clear();
  statusUpdate(true);
};

void SerialPortManager::readDateTimeFromDevice(){
  if(serial->isOpen()){
      receivedData.clear();
      QDateTime dataTime;

      writeData(createCommand(0x3E,0x00,0x00));
      serial->waitForBytesWritten(500);

      //-------for async read
      QEventLoop loop;
      connect(this, &SerialPortManager::dataReady, &loop, &QEventLoop::quit);
      QTimer::singleShot(1000, &loop, &QEventLoop::quit);
      loop.exec();
      //------------

      if(receivedData.size()==5){
          dataTime.setTime(QTime(receivedData[0],receivedData[1],0));
          dataTime.setDate(QDate(2000+receivedData[4],receivedData[3],receivedData[2]));
          qDebug()<<"dataTime"<<dataTime.toString("dd.MM.yyyy hh:mm");
          storage.setDateTime(dataTime);
          repeatTimes=0;
        }
      else{
          if(repeatTimes<10){
              delay(500);
              qDebug()<<"Read divice datetime fail, repeat";
              Logging::logWarning("Read divice datetime fail, repeat to read");
              ++repeatTimes;
              readDateTimeFromDevice();
            }else{
              Logging::logError("Read divice datetime fail, tryed more 10 times");
            }
        }


    }
};

void SerialPortManager::writeDateTimeFromDevice(){
  if(serial->isOpen()){
      receivedData.clear();
      serial->waitForReadyRead(500);//timeout

      QDateTime dataTime=storage.getDateTime();
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
};

void SerialPortManager::statusUpdate(const bool &status){
  isConnected=status;
};

void SerialPortManager::portNumUpdate(const uint8_t &Nport){
  QString str="COM"+QString::number(Nport);
  serial->setPortName(str);
};

bool SerialPortManager::getConnectStatus()const{
  return isConnected;
};

QString SerialPortManager::getPortName()const{
  return serial->portName();
};

uint8_t SerialPortManager::getPortNumber()const{
  QString PortName=serial->portName();
  QString numberPort="";
  for(int chr=3;chr< serial->portName() .size();++chr){
      numberPort+=PortName[chr];
    }
  return numberPort.toInt();
};

//----------- slot for update
void SerialPortManager::handleReadyRead(){
  while(serial->bytesAvailable()){
      receivedData.append(serial->readAll());
    }
  emit dataReady();
};

void SerialPortManager::delay(int milliseconds) {
  QEventLoop loop;
  QTimer::singleShot(milliseconds, &loop, &QEventLoop::quit);
  loop.exec();
};

//get current values from device
void SerialPortManager::getTempHumid(){
  if(serial->isOpen()){
      receivedData.clear();
      delay(300);
      writeData(createCommand(0x14,0x00,0x00));
      serial->waitForBytesWritten(500);

      QEventLoop loop;
      connect(this, &SerialPortManager::dataReady, &loop, &QEventLoop::quit);
      QTimer::singleShot(1000, &loop, &QEventLoop::quit);
      loop.exec();

      if(receivedData.size()==8){
          uint16_t temp = (static_cast<uint16_t>(static_cast<uint8_t>(receivedData[0]) << 8)) | static_cast<uint8_t>(receivedData[1]);
          uint16_t humid = (static_cast<uint16_t>(static_cast<uint8_t>(receivedData[4]) << 8)) | static_cast<uint8_t>(receivedData[5]);
          storage.setTemperature(temp*0.1);
          storage.setHumid(humid*0.1);
          qDebug()<<"temp: "<<temp*0.1<<"humid"<<humid*0.1;
          Logging::logInfo("temp: "+QString::number(temp*0.1).toStdString()+", humid: "+QString::number(humid*0.1).toStdString());
          repeatTimes=0;
        }else{
          if(repeatTimes<10){
              delay(500);
              Logging::logInfo("tempHumid getting fail,repeat to get");
              qDebug()<<"tempHumid getting fail,repeat";
              ++repeatTimes;
              getTempHumid();
            }else{
              Logging::logError("tempHumid getting fail,tryed more 10 times");
            }
        }
    }
};

////only up to 32byte, mean need loop for receive all data
//void SerialPortManager::readFlashAddr(const uint8_t cmd,const uint8_t lng, const uint8_t addr,const  int byteCount){
//  if(serial->isOpen()){
//      receivedData.clear();
//      serial->clear();
//      writeData(createCommand(cmd,lng,addr));
//      delay(800);
//      while(receivedData.size()<byteCount){
//          QEventLoop loop;
//          connect(this, &SerialPortManager::dataReady, &loop, &QEventLoop::quit);
//          QTimer::singleShot(1000, &loop, &QEventLoop::quit);
//          loop.exec();
//          processBarValue=receivedData.size();
//        }
//      qDebug()<<"receivedData total:"<<receivedData.size();
//    }
//};
void SerialPortManager::readFlashAddr(const uint8_t cmd, const uint8_t lng, const uint8_t addr, const int byteCount) {
  if (serial->isOpen()) {
      receivedData.clear();
      serial->clear();
      writeData(createCommand(cmd, lng, addr));
      delay(800);

      int retryCount = 0;
      const int maxRetries = 10; // максимальное количество попыток ожидания
      const int waitTimeMs = 1000; // время ожидания в миллисекундах

      while (receivedData.size() < byteCount && retryCount < maxRetries) {
          QEventLoop loop;
          connect(this, &SerialPortManager::dataReady, &loop, &QEventLoop::quit);
          QTimer::singleShot(waitTimeMs, &loop, &QEventLoop::quit);
          loop.exec();

          if (receivedData.size() > static_cast<int>(processBarValue)) {
              retryCount = 0; // если получены новые данные, сбрасываем счетчик попыток
            } else {
              ++retryCount; // увеличиваем счетчик при отсутствии новых данных
            }

          processBarValue = receivedData.size();
        }

      qDebug() << "receivedData total:" << receivedData.size();

      if (retryCount == maxRetries) {
          qWarning() << "Timeout reached: data not fully received.";
        }
    }
}


//create mask user control range
void SerialPortManager::controlSettings(){
  uint8_t count=0;
  uint8_t res=receivedData[1];

  storage.setRangeValue(receivedData[1]);

  for(int it=1;it<=128;it<<=1){
      ++count;
      if(res & it){
          controllSettings[count]=true;
        }
      else{
          controllSettings[count]=false;
        }
    }
  storage.setControlSettings(controllSettings);
  qDebug()<<"controllSettings"<<controllSettings;
};

//write one byte to eeprom device
void SerialPortManager::writeToFlashByte(const uint8_t address, const int8_t byte){
  if(serial->isOpen()){
      writeData(createCommand(0x08,byte,address));
    }
  if(serial->waitForBytesWritten()){
      qDebug()<<"Byte written";
    }
};

//func for create button push value from true/false mask
//   1    2   4   8
// | # | # | # | # | T C
//   16  48  112 240
// | # | # | # | # |  RH%
//or 0b00000000 where each bit = cell status
void SerialPortManager::createButtonsMaskByte(){
  uint8_t count_true=0;

  for(uint8_t it=0;it<8;++it){
      if(controllSettings[it+1]==true&&it+1==1){
          count_true+=1;
        }
      else{
          if(controllSettings[it+1]){
              count_true+=qPow(2,it);
            }
        }
    }
  storage.setRangeValue(count_true);
};

//write to device new controll periods
void SerialPortManager::writeControlSettings(){
  if(storage.getModelDevice()==DEV_1XX){
      createButtonsMaskByte();
      writeToFlashByte(0x19,storage.getRangeValue());
      delay(300);
    }
  if(storage.getModelDevice()==DEV_2XX){
      auto ranges=storage.getRangeFor211();
      writeToFlashByte(0x44,std::get<0>(ranges));
      qDebug()<<"iTempL "<<std::get<0>(ranges);
      delay(400);
      writeToFlashByte(0x45,std::get<1>(ranges));
      qDebug()<<"iTempH "<<std::get<1>(ranges);
      delay(400);
      writeToFlashByte(0x46,std::get<4>(ranges));
      qDebug()<<"eTempL "<<std::get<4>(ranges);
      delay(400);
      writeToFlashByte(0x47,std::get<5>(ranges));
      qDebug()<<"eTempH "<<std::get<5>(ranges);
      delay(400);
      writeToFlashByte(0x48,std::get<2>(ranges));
      qDebug()<<"iHumidL "<<std::get<2>(ranges);
      delay(400);
      writeToFlashByte(0x49,std::get<3>(ranges));
      qDebug()<<"iHumidH "<<std::get<3>(ranges);
      delay(400);
      writeToFlashByte(0x4A,std::get<6>(ranges));
      qDebug()<<"eHumidL "<<std::get<6>(ranges);
      delay(400);
      writeToFlashByte(0x4B,std::get<7>(ranges));
      qDebug()<<"eHumidH "<<std::get<7>(ranges);
      delay(400);
    }
};

//get volume level from device
void SerialPortManager::getVolumeLevel(){
  delay(500);
  readFlashAddr(0x0A,0x01,0x4D,5);
  storage.setVolumeLevel(receivedData[0]);
};

//set volume level on device
void SerialPortManager::setVolumeLevel(uint8_t level){
  storage.setVolumeLevel(level);
  writeToFlashByte(0x4D,level);
  delay(300);
};

//receive verification date from device(BCD format) YY YY MM DD
void SerialPortManager::getVerificationDate(){
  QString year;
  QString mounth;
  QString day;

  QByteArray deviceInfo=storage.getDeviceInfo();

  year=QString::number(BcdToByte(deviceInfo[28]));
  year+=QString::number(BcdToByte(deviceInfo[29]));
  mounth=QString::number(BcdToByte(deviceInfo[30]));
  day=QString::number(BcdToByte(deviceInfo[31]));

  QDate VerificationDate;
  VerificationDate.setDate(year.toInt(), mounth.toInt(),day.toInt());
  if(!VerificationDate.isValid()||VerificationDate.isNull()){
      qDebug()<<"no valid ver date, getted "<<deviceInfo;
      Logging::logError("no valid ver date, getted "+deviceInfo.toStdString());
      // askSerialPort(); without repeat request, healfing in future
    }
  storage.setVerificationDate(VerificationDate);
  qDebug()<<"VerificationDate "<<VerificationDate.toString("dd.MM.yyyy");
  Logging::logInfo("VerificationDate "+VerificationDate.toString("dd.MM.yyyy").toStdString());
};

//write new current verification date to device
void SerialPortManager::setVerificationDate(){
  delay(300);
  QDate VerificationDate=storage.getVerificationDate();

  uint8_t date[4];
  date[0] = static_cast<uint8_t>(VerificationDate.year()/100);
  date[1] = static_cast<uint8_t>((VerificationDate.year()%100)&0xFF);
  date[2] = static_cast<uint8_t>((VerificationDate.month()%100)&0xFF);
  date[3] = static_cast<uint8_t>((VerificationDate.day()%100)&0xFF);

  for(uint8_t it=0;it<4;++it){
      writeToFlashByte((0x12+it),ByteToBcd(date[it]));
      delay(500);
    }
  //erase cmd 33 on device
  // writeData(createCommand(0x21,0x00,0x00));
  //delay(500);
  storage.setVerificationDate(VerificationDate);
  qDebug()<<"new verification date setted";
  Logging::logWarning("new verification date setted");
  emit verificationDateRecorded();
};

//convert bcd to byte value, 0x20(hex) = 20(dec)
uint8_t SerialPortManager::BcdToByte(uint8_t Value)
{
  uint8_t higher=(Value>>4)&0x0F;
  uint8_t lower = Value&0x0F;
  return higher*10+lower;
};

//convert byte value to bcd, 20(dec) = 0x20(hex)
uint8_t SerialPortManager::ByteToBcd(uint8_t Value)
{
  uint8_t bcdhigh = 0;

  while (Value >= 10)
    {
      bcdhigh++;
      Value -= 10;
    }
  return  (uint8_t)((uint8_t)(bcdhigh << 4) | Value);
};

//after find device try to set high speed uart
void SerialPortManager::setHighBaudRate(){
  if(serial->isOpen()){
      unsigned int b = 230400;
      uint8_t byte_1= b/10000;
      uint8_t byte_2 = (b%10000)/100;
      writeData(createCommand(0x3C,byte_1,byte_2));
      delay(800);
      serial->setBaudRate(b);
      qDebug()<<"speed is setted: "<<serial->baudRate();
      Logging::logInfo("speed is setted: "+QString::number(serial->baudRate()).toStdString());
    }
};



//send to device cmd for set baudrate 9600
void SerialPortManager::resetSpeed(){
  if(serial->isOpen()){
      if(storage.getModelDevice()==DEV_1XX){
      writeData(createCommand(0x3C,0x00,0x60));
      delay(500);
        }
      writeData(createCommand(0x3C,0x00,0x60));
      delay(500);
      serial->setBaudRate(9600);
    }
};

//this cmd init new values saved in eeprom
void SerialPortManager::saveSettings(){
  if(serial->isOpen()){
      writeData(createCommand(0x40,0x00,0x00));
      serial->waitForBytesWritten();
    }
};

//one byte contain value setted control range on device
void SerialPortManager::getControlRange(){
  if(storage.getModelDevice()==DEV_1XX){
      readFlashAddr(0x0A,0x04,0x18,8);
      controlSettings();
    }
  if(storage.getModelDevice()==DEV_2XX){
      readFlashAddr(0x0A,0x08,0x44,12);
      controlSettings211();
    }
};

void SerialPortManager::setControlRange(){
  writeControlSettings();
  saveSettings();
};

void SerialPortManager::getBlock(){
  if(serial->isOpen()&&storage.getBlockSizeValue()!=0){
      int32_t bytes=storage.getBlockSizeValue();
      delay(1000);
      if(bytes>1000){
          setHighBaudRate();
        }
      delay(1000);
      readFlashAddr(0x38,0x00,0x00,bytes);

      storage.setDataBlock(receivedData);
      QString devName=storage.getModelDevice();
      uint8_t from=21;
      uint16_t DaysAfter2015=exportBits(receivedData,from,16,0);
      uint16_t Minutes=exportBits(receivedData,from,11,0);
      QDate start(2015,1,1);
      QDate dateFrom= start.addDays(DaysAfter2015);
      QTime time(0,0,0);
      QTime timeFrom= time.addSecs(Minutes*60);
      QDateTime fromDateTime;
      fromDateTime.setDate(dateFrom);
      fromDateTime.setTime(timeFrom);
      storage.setFromDateDB(fromDateTime);
      for(int it=receivedData.size()-6;it>0;it-=6){
          QByteArray temp;
          temp.push_back(receivedData[it]);
          uint8_t flags=receivedData[it]>>4;
          bool flag1 = flags & 0x01;
          bool flag2 = flags & 0x02;
          bool flag3 = flags & 0x04;
          bool flag4 = flags & 0x08;
          if(flag2&&!flag1&&!flag3&&!flag4&&devName==DEV_1XX){
              uint8_t startBit=5;
              uint16_t ranges=exportBits(receivedData,startBit,16,it);
              uint16_t DaysAfter2015=exportBits(receivedData,startBit,16,it);
              uint16_t Minutes=exportBits(receivedData,startBit,11,it);

              QDateTime ToDb;
              ToDb.setDate(start.addDays(DaysAfter2015));
              ToDb.setTime(time.addSecs(Minutes*60));
              storage.setToDateDB(ToDb);
              qDebug()<<"we find end date!block number "<<it;
              break;
            }else if(static_cast<unsigned char>(receivedData[it])==0x20||static_cast<unsigned char>(receivedData[it])==0x30){
              uint8_t startBit=5;
              uint16_t ranges=exportBits(receivedData,startBit,16,it);
              uint16_t DaysAfter2015=exportBits(receivedData,startBit,16,it);
              uint16_t Minutes=exportBits(receivedData,startBit,11,it);
              QByteArray temp;
              temp.push_back(receivedData[it]);

              qDebug()<<"byte array: "<<temp.toHex().toUpper();

              QDateTime ToDb;
              ToDb.setDate(start.addDays(DaysAfter2015));
              ToDb.setTime(time.addSecs(Minutes*60));
              storage.setToDateDB(ToDb);
              qDebug()<<"we find end date for 211!block number "<<it;
              break;
            }else{
              storage.setToDateDB(fromDateTime);
            }
        }
      resetSpeed();
      emit blockDataReady();
    }
};

void SerialPortManager::getAllData(){
  getBlock();
};

uint16_t SerialPortManager::exportBits(QByteArray &data,uint8_t &startBit,uint8_t bitlengh,uint32_t iter ){
  uint16_t value=0;
  for(int i=0;i<bitlengh;++i){
      uint32_t byteIndex = (startBit+i)/8+iter;
      uint8_t bitIndex = 7-((startBit+i)%8);
      value<<=1;
      value|=data[byteIndex]>>bitIndex & 0x01;
    }
  startBit+=bitlengh;
  return value;
};

void SerialPortManager::getBlockSize(){
  receivedData.clear();
  if(serial->isOpen()){

      writeData(createCommand(0x37,0x00,0x00));
      //serial->waitForBytesWritten();
      delay(800);

      QEventLoop loop;
      connect(this, &SerialPortManager::dataReady, &loop, &QEventLoop::quit);
      QTimer::singleShot(1000, &loop, &QEventLoop::quit);
      loop.exec();

      uint8_t byte1=receivedData[0];
      uint8_t byte2=receivedData[1];
      uint8_t byte3=receivedData[2];
      uint8_t byte4=receivedData[3];
      uint32_t result=(byte1<<0)|(byte2<<8)|(byte3<<16)|(byte4<<24);

      storage.setBlockSizeValue(result);
    }
};

void SerialPortManager::controlSettings211(){
  int8_t inTempLower=receivedData[0];
  int8_t inTempUpper=receivedData[1];
  uint8_t inHumidLower=receivedData[4];
  uint8_t inHumidUpper=receivedData[5];
  int8_t outTempLower=receivedData[2];
  int8_t outTempUpper=receivedData[3];
  uint8_t outHumidLower=receivedData[6];
  uint8_t outHumidUpper=receivedData[7];
  storage.setRangeFor211(inTempLower,inTempUpper,inHumidLower,inHumidUpper,outTempLower,outTempUpper,outHumidLower,outHumidUpper);
};


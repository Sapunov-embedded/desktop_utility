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
  connect(serial, &QSerialPort::readyRead, this, &SerialPortManager::handleReadyRead);//*
};


SerialPortManager::~SerialPortManager()
{
  delete serial;
};
void SerialPortManager::on_Button_connect_clicked(){};

//This function is asking "Who are you?" the connected device
bool SerialPortManager::askSerialPort()
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
      receivedData.clear();
      writeData(createCommand(0x15,0x00,0x00));
      serial->waitForBytesWritten(300);


      //* ------------update
      QEventLoop loop;
      connect(this, &SerialPortManager::dataReady, &loop, &QEventLoop::quit);
      QTimer::singleShot(500, &loop, &QEventLoop::quit);
      loop.exec();

      qDebug() << "Data received in ask port " << receivedData;
      //*--------------update
      QString deviceName;
      //   deviceInfo=receivedData;                                                //*

      storage.setDeviceInfo(receivedData);

      QByteArray deviceInfo=storage.getDeviceInfo();

      for(uint8_t it=0;it<3;++it){
          deviceName+=deviceInfo[it];                                       //*

        }

      storage.setModelDevice(deviceName);


      if(deviceName != "101" && deviceName != "211"){
          qDebug()<<serial->portName()<<" no get correct answer not given\n";
          closeSerialPort();
          return false;
        }
      else{
          qDebug()<<"hello "<<deviceName<<"\n";
          QString ModelDevice=deviceName;
          QString PortName=serial->portName();
          QString numberPort="";

          for(int chr=3;chr<PortName.size();++chr){
              numberPort+=PortName[chr];
            }
          return true;
        }
    }
  else{
      qDebug()<<serial->portName()<<"not open\n";
      return false;
    }
}

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
};

//connect button on manual connection section
void SerialPortManager::on_pushButton_11_clicked(){
  if (serial->open(QIODevice::ReadWrite)) {
      if(askSerialPort()){
          updateData();
        }
      else{
          qDebug()<<"can't open port\n";
        }
    }
};

void SerialPortManager::closeSerialPort(){
  if (serial->isOpen()){
      qDebug()<<serial->portName()<<" serial was closed\n";
      resetSpeed();
      serial->close();
      StatusUpdate(false);
    }
};

void SerialPortManager::writeData(const QByteArray &data){
  const qint64 written = serial->write(data);
  if (written == data.size()) {
      qDebug()<<written<<" bytes was written\n";
    } else {
      qDebug()<<"Failed to write all data to port %1.\n"
                "Error: "<<serial->errorString()<<"\n";
    }
};


void SerialPortManager::on_Disconnect_device_clicked(){
  closeSerialPort();
};


//-------------------------------------------------------------
void SerialPortManager::readFwVersion(){
  if(serial->isOpen()){
      // fwVersion.clear();
      QString fwVersion;

      delay(300);
      writeData(createCommand(0x3D,0x00,0x00)); //command #58, data1=0, data2=0
      serial->waitForBytesWritten(200);

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
        }
      storage.setFwVersion(fwVersion);
    }

};

void SerialPortManager::readSnDevice(){
  if(serial->isOpen()){
      QString snDevice;
      QByteArray deviceInfo= storage.getDeviceInfo();
      if(deviceInfo.size()==32){
          QString str;
          for(int it=10;it<16;++it){
              str+=QString::number(deviceInfo[it]);
            }
          snDevice=str;
          storage.setSnDevice(str);
          qDebug()<<"s.n "<<str;
        }
    }
};

//----------------------------------------------------------
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

void SerialPortManager::updateData(){
  receivedData.clear();
  readSnDevice();
  receivedData.clear();
  readFwVersion();
  receivedData.clear();
  getTempHumid();
  GetVerificationDate();
  StatusUpdate(true);
};

void SerialPortManager::on_readDateTimeFromDevice_clicked(){
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
          qDebug()<<dataTime;
        }
      storage.setDateTime(dataTime);
    }
};

void SerialPortManager::on_writeDateTimeFromDevice_clicked(){
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

void SerialPortManager::StatusUpdate(const bool &status){
  isConnected=status;
};

void SerialPortManager::PortNumUpdate(const uint8_t &Nport){
  QString str="COM"+QString::number(Nport);
  serial->setPortName(str);
};

bool SerialPortManager::getConnectStatus(){
  return isConnected;
};

QString SerialPortManager::getPortName(){
  return serial->portName();
};

uint8_t SerialPortManager::getPortNumber(){
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
}


void SerialPortManager::getTempHumid(){
  if(serial->isOpen()){
      receivedData.clear();
      writeData(createCommand(0x14,0x00,0x00));
      serial->waitForBytesWritten(500);

      QEventLoop loop;
      connect(this, &SerialPortManager::dataReady, &loop, &QEventLoop::quit);
      QTimer::singleShot(1000, &loop, &QEventLoop::quit);
      loop.exec();


      uint16_t temp = (static_cast<uint16_t>(static_cast<uint8_t>(receivedData[0]) << 8)) | static_cast<uint8_t>(receivedData[1]);
      uint16_t humid = (static_cast<uint16_t>(static_cast<uint8_t>(receivedData[4]) << 8)) | static_cast<uint8_t>(receivedData[5]);

      storage.setTemperature(temp*0.1);
      storage.setHumid(humid*0.1);
    }
}

//only up to 32byte, because need loop for recive all data
void SerialPortManager::ReadFlashAddr(const uint8_t cmd,const uint8_t lng, const uint8_t addr,const  int countBytes){
  if(serial->isOpen()){
      receivedData.clear();
      serial->clear();
      writeData(createCommand(cmd,lng,addr));
      delay(800);

      while(receivedData.size()<countBytes){
          QEventLoop loop;
          connect(this, &SerialPortManager::dataReady, &loop, &QEventLoop::quit);
          QTimer::singleShot(1000, &loop, &QEventLoop::quit);
          loop.exec();
          processBarValue=receivedData.size();

        }
    }
}

//create mask user control range
void SerialPortManager::ControlSettings(){
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
}

//write one byte to eeprom device
void SerialPortManager::WriteToFlashByte(const uint8_t address, const uint8_t byte){
  if(serial->isOpen()){
      writeData(createCommand(0x08,byte,address));
    }
  if(serial->waitForBytesWritten()){
      qDebug()<<"Byte written";
    }
}

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
void SerialPortManager::WriteControllSettings(){
  createButtonsMaskByte();
  WriteToFlashByte(0x19,storage.getRangeValue());
  delay(300);
};

//get volume level from device
void SerialPortManager::GetVolumeLevel(){
  delay(500);
  ReadFlashAddr(0x0A,0x01,0x4D,4);
  storage.setVolumeLevel(receivedData[0]);
};

//set volume level on device
void SerialPortManager::SetVolumeLevel(uint8_t level){
  storage.setVolumeLevel(level);
  WriteToFlashByte(0x4D,level);
  delay(300);
  qDebug()<<level<<" level sound saved";
};

//receive verification date from device(BCD format) YY YY MM DD
void SerialPortManager::GetVerificationDate(){

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
//  QDate verDate;
 // verDate.setDate(year.toInt(), mounth.toInt(),day.toInt());
  storage.setVerificationDate(VerificationDate);
  qDebug()<<VerificationDate;

};

//write new current verification date to device
void SerialPortManager::SetVerficationDate(){
  delay(300);
  QDate VerificationDate;

  uint8_t date[4];
  date[0] = static_cast<uint8_t>(VerificationDate.year()/100);
  date[1] = static_cast<uint8_t>((VerificationDate.year()%100)&0xFF);
  date[2] = static_cast<uint8_t>((VerificationDate.month()%100)&0xFF);
  date[3] = static_cast<uint8_t>((VerificationDate.day()%100)&0xFF);

  for(uint8_t it=0;it<4;++it){
      WriteToFlashByte((0x12+it),ByteToBcd(date[it]));
      delay(500);
    }
  writeData(createCommand(0x21,0x00,0x00));
  delay(500);
  storage.setVerificationDate(VerificationDate);
  qDebug()<<"new verification date setted with erase memory";
};

//convert bcd to byte value, 0x20(hex) = 20(dec)
uint8_t SerialPortManager::BcdToByte(uint8_t Value)
{
  uint8_t higher=(Value>>4)&0x0F;
  uint8_t lower = Value&0x0F;
  return higher*10+lower;
}

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
}

//after find device try to set high speed uart
void SerialPortManager::autoSelectBaudRate(){
  if(serial->isOpen()){
      unsigned int b = 230400;
      uint8_t byte_1= b/10000;
      uint8_t byte_2 = (b%10000)/100;
      writeData(createCommand(0x3C,byte_1,byte_2));
      delay(800);
      serial->setBaudRate(b);
      qDebug()<<"speed is setted: "<<serial->baudRate();
    }
};

//manual set speed, for debug
void SerialPortManager::setSpeed(QString speed){
  serial->setBaudRate(speed.toULong());
}

//send to device cmd for set baudrate 9600
void SerialPortManager::resetSpeed(){
  if(serial->isOpen()){
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
}

//debug func
void SerialPortManager::readloop(){
  receivedData.clear();
  writeData((createCommand(0x39,0x00,0x00)));
  if(serial->waitForBytesWritten()){
    }
};

void SerialPortManager::GetControlRange(){
  ReadFlashAddr(0x0A,0x04,0x18,8);
  ControlSettings();
};

void SerialPortManager::SetControlRange(){
  WriteControllSettings();
  saveSettings();
};

int SerialPortManager::getBlock(){
  if(serial->isOpen()&&storage.getBlockSizeValue()!=0){
      delay(1000);
      int32_t bytes=storage.getBlockSizeValue();
      ReadFlashAddr(0x38,0x00,0x00,bytes);

      //      if(receivedData.size()==bytes){
      //      qDebug()<<"i all receve";
      storage.setDataBlock(receivedData);

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
          uint8_t flags=receivedData[it]>>4;
          bool flag1 = flags & 0x01;
          bool flag2 = flags & 0x02;
          bool flag3 = flags & 0x04;
          bool flag4 = flags & 0x08;

          if((flag2&&!flag1&&!flag3&&!flag4)){
              uint8_t startBit=5;
              uint16_t ranges=exportBits(receivedData,startBit,16,it);
              uint16_t DaysAfter2015=exportBits(receivedData,startBit,16,it);
              uint16_t Minutes=exportBits(receivedData,startBit,11,it);

              QDateTime ToDb;
              ToDb.setDate(start.addDays(DaysAfter2015));
              ToDb.setTime(time.addSecs(Minutes*60));
              storage.setToDateDB(ToDb);
              break;
            }
        }


      HexToFile("C:/Users/sapunov.i/Documents/101/report.txt",receivedData);
      //        }
      //      else{
      //          qDebug()<<"receve only "<<receivedData.size();
      //        }
      //serial->setBaudRate(9600);
    }
  return 0;
};

void SerialPortManager::GetAllData(){
  getBlock();
};

//temp just for export to txt
void SerialPortManager::HexToFile(QString FileName,  QByteArray &arr)
{
  QFile file(FileName);

  if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
      QTextStream out(&file);
      // QString str="";

      for(int i = 0; i < arr.size(); i+=6){
          uint8_t flags=arr[i]>>4;
          bool flag1=flags & 0x01;
          bool flag2 = flags & 0x02;
          bool flag3 = flags & 0x04;
          bool flag4 = flags & 0x08;

          out<<i<<". ";
          if(flag1&&!flag2){
              uint8_t startBit=6;
              int16_t extTemp=exportBits(arr,startBit,11,i);
              if(extTemp>1023){
                  extTemp|=1<<15;
                  extTemp|=1<<14;
                  extTemp|=1<<13;
                  extTemp|=1<<12;
                  extTemp|=1<<11;
                }
              uint16_t extHumid=exportBits(arr,startBit,10,i);
              int16_t intTemp=exportBits(arr,startBit,11,i);
              if(intTemp>1023){
                  intTemp|=1<<15;
                  intTemp|=1<<14;
                  intTemp|=1<<13;
                  intTemp|=1<<12;
                  intTemp|=1<<11;
                }
              uint16_t intHumid=exportBits(arr,startBit,10,i);

              float eTemp= (float)extTemp/10;
              float eHumid = (float)extHumid/10;
              float iTemp= (float)intTemp/10;
              float iHumid = (float) intHumid/10;
              out<<"ExpT: "+QString::number(eTemp)+", ExpHumid: "+QString::number(eHumid)+", Temp: "+QString::number(iTemp)+", Humid: "+QString::number(iHumid)+"\n";

            }
          if((flag2&&flag1)||(flag2&&!flag1)){
              uint8_t startBit=5;
              uint16_t ranges=exportBits(arr,startBit,16,i);
              uint16_t DaysAfter2015=exportBits(arr,startBit,16,i);
              uint16_t Minutes=exportBits(arr,startBit,11,i);

              if(flag2&&flag1){
                  out<<"Startup range ";
                }

              out<<"Range value: "+QString::number(ranges)+", Days after 01.01.2015: "+QString::number(DaysAfter2015)+", Minutes: "+QString::number(Minutes)+"\n";
            }
        }
    }
  file.close();
}


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

void SerialPortManager::ReadAllMem(){
  if(serial->isOpen()){
      int32_t bytes=storage.getBlockSizeValue();
      ReadFlashAddr(0x38,0x01,0x00,bytes);
      //     if(receivedData.size()==bytes+4){
      //   qDebug()<<"i all receve";
      HexToFile("C:/Users/sapunov.i/Documents/101/report.txt",receivedData);
      //        }
      //      else{
      //          qDebug()<<"receve only "<<receivedData.size();
      //        }
      //      serial->setBaudRate(9600);
    }

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
      qDebug()<<storage.getBlockSizeValue();
    }
};



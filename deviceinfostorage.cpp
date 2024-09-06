#include "deviceinfostorage.h"

DeviceInfoStorage::DeviceInfoStorage(){};

DeviceInfoStorage&  DeviceInfoStorage::getInstanse(){
  static DeviceInfoStorage instanse;
  return instanse;
} ;

//getters
QString DeviceInfoStorage::getModelDevice(){
  return ModelDevice;
};
QByteArray DeviceInfoStorage::getDeviceInfo(){
  return deviceInfo;
};
QDateTime DeviceInfoStorage::getDateTime(){
  return dataTime;
};
QString DeviceInfoStorage::getFwVersion(){
  return fwVersion;
};
QString DeviceInfoStorage::getSnDevice(){
  return snDevice;
};
float DeviceInfoStorage::getTemperature(){
  return temperature;
};
float DeviceInfoStorage::getHumid(){
  return humids;
};
uint8_t DeviceInfoStorage::getVolumeLevel(){
  return VolumeLevel;
};
QDate DeviceInfoStorage::getVerificationDate(){
  return VerificationDate;
};
uint32_t DeviceInfoStorage::getBlockSizeValue(){
  return BlockSizeValue;
};
uint8_t DeviceInfoStorage::getRangeValue(){
  return RangeValue;
};

QByteArray DeviceInfoStorage::getDataBlock(){
  return DataBlock;
};

QDateTime DeviceInfoStorage::getFromDateDB(){
  return FromDB;
};

QDateTime DeviceInfoStorage::getToDateDB(){
  return ToDB;
};

bool* DeviceInfoStorage::getControlSettings(){
  return controllSettings;
};
QString  DeviceInfoStorage::getDataBasePath(){
  return DBasePath;
};
QString  DeviceInfoStorage::getCsvPath(){
  return CsvPath;
};
QString  DeviceInfoStorage::getPdfPath(){
  return PdfPath;
};

//setters
void DeviceInfoStorage::setModelDevice(QString deviceModel){
  ModelDevice=deviceModel;
};
void DeviceInfoStorage::setDeviceInfo(QByteArray Info){
  deviceInfo=Info;
};
void DeviceInfoStorage::setDateTime(QDateTime dt){
  dataTime=dt;
};
void DeviceInfoStorage::setFwVersion(QString fw){
  fwVersion=fw;
};
void DeviceInfoStorage::setSnDevice(QString sn){
  snDevice=sn;
};
void DeviceInfoStorage::setTemperature(float t){
  temperature=t;
};
void DeviceInfoStorage::setHumid(float h){
  humids=h;
};
void DeviceInfoStorage::setVolumeLevel(uint8_t level){
  VolumeLevel=level;
};
void DeviceInfoStorage::setVerificationDate(QDate d){
  VerificationDate=d;
};
void DeviceInfoStorage::setBlockSizeValue(uint32_t size){
  BlockSizeValue=size;
};
void DeviceInfoStorage::setRangeValue(uint8_t range){
  RangeValue=range;
};

void DeviceInfoStorage::setDataBlock(QByteArray db){
  DataBlock=db;
}

void DeviceInfoStorage::setFromDateDB(QDateTime from){
   FromDB=from;
};

void DeviceInfoStorage::setToDateDB(QDateTime to){
  ToDB=to;
};

void DeviceInfoStorage::setControlSettings(bool* array){
  for(uint8_t it=1;it<9;++it){
    controllSettings[it]=array[it];
    }
};

void  DeviceInfoStorage::setDataBasePath(QString path){
  DBasePath=path;
};
void  DeviceInfoStorage::setCsvPath(QString path){
  CsvPath=path;
};
void  DeviceInfoStorage::setPdfPath(QString path){
  PdfPath=path;
};
DeviceInfoStorage::~DeviceInfoStorage(){};

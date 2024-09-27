#ifndef DEVICEINFOSTORAGE_H
#define DEVICEINFOSTORAGE_H

#include <QByteArray>
#include <QDateTime>

#include <QObject>
#include<QDebug>
#include <tuple>

class DeviceInfoStorage:public QObject
{
public:
  static DeviceInfoStorage& getInstanse();
  //getters
  QString getModelDevice();
  QByteArray getDeviceInfo();
  QDateTime getDateTime();
  QString getFwVersion();
  QString getSnDevice();
  float getTemperature();
  float getHumid();
  uint8_t getVolumeLevel();
  QDate getVerificationDate();
  uint32_t getBlockSizeValue();
  uint8_t getRangeValue();
  QByteArray getDataBlock();
  QDateTime getFromDateDB();
  QDateTime getToDateDB();
  bool* getControlSettings();
  QString getDataBasePath();
  QString getCsvPath();
  QString getPdfPath();
  std::tuple<int8_t, int8_t, uint8_t, uint8_t,int8_t, int8_t, uint8_t, uint8_t> getRangeFor211();
  bool isInnerSensor();
  //setters
  void setModelDevice(QString deviceModel);
  void setDeviceInfo(QByteArray Info);
  void setDateTime(QDateTime dt);
  void setFwVersion(QString fw);
  void setSnDevice(QString sn);
  void setTemperature(float t);
  void setHumid(float h);
  void setVolumeLevel(uint8_t level);
  void setVerificationDate(QDate d);
  void setBlockSizeValue(uint32_t size);
  void setRangeValue(uint8_t range);
  void setDataBlock(QByteArray db);
  void setFromDateDB(QDateTime from);
  void setToDateDB(QDateTime to);
  void setControlSettings(bool* array);
  void setDataBasePath(QString path);
  void setCsvPath(QString path);
  void setPdfPath(QString path);
  void setRangeFor211(int8_t InTempLower, int8_t InTempUpper, uint8_t InHumidLower, uint8_t InHumidUpper,
                      int8_t OutTempLower, int8_t OutTempUpper, uint8_t OutHumidLower, uint8_t OutHumidUpper);
  void setSensorType211(bool isInner);
  ~ DeviceInfoStorage();

private:
  DeviceInfoStorage();
  DeviceInfoStorage(const DeviceInfoStorage&)=delete;
  DeviceInfoStorage& operator=(const DeviceInfoStorage)=delete;
  //------------------
  QString ModelDevice;
  QByteArray deviceInfo;
  QDateTime dataTime;
  QString fwVersion;
  QString snDevice;
  float temperature=0.0;
  float humids=0.0;
  uint8_t VolumeLevel=0;
  QDate VerificationDate;
  uint32_t BlockSizeValue=0;
  uint8_t RangeValue=0;
  QByteArray DataBlock;
  QDateTime FromDB;
  QDateTime ToDB;
  QString AdobePath;
  QString DBasePath;
  QString CsvPath;
  QString PdfPath;
  int8_t InTempLowerControl;
  int8_t InTempUpperControl;
  uint8_t InHumidLowerControl;
  uint8_t InHumidUpperControl;
  int8_t OutTempLowerControl;
  int8_t OutTempUpperControl;
  uint8_t OutHumidLowerControl;
  uint8_t OutHumidUpperControl;
  bool controllSettings[9]{false};
  bool isInSensor=true;
};

#endif // DEVICEINFOSTORAGE_H

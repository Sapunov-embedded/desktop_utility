#ifndef DEVICEINFOSTORAGE_H
#define DEVICEINFOSTORAGE_H

#include <QByteArray>
#include <QDateTime>

#include <QObject>
#include<QDebug>

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
  bool controllSettings[9]{false};
};

#endif // DEVICEINFOSTORAGE_H

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
  /**
   * @brief Getters for retrieving stored variables.
   *
   */
  bool isInnerSensor();
  float getTemperature();
  float getHumid();
  uint8_t getVolumeLevel();
  uint8_t getRangeValue();
  uint32_t getBlockSizeValue();
  QDate getVerificationDate();
  QString getModelDevice();
  QString getDataBasePath();
  QString getCsvPath();
  QString getPdfPath();
  QString getFwVersion();
  QString getSnDevice();
  QDateTime getDateTime();
  QDateTime getFromDateDB();
  QDateTime getToDateDB();
  QByteArray getDeviceInfo();
  QByteArray getDataBlock();
  bool* getControlSettings();

  /**
   * @brief getRangeFor211
   * @return InTempLowerControl,InTempUpperControl,InHumidLowerControl,InHumidUpperControl,
   *         OutTempLowerControl,OutTempUpperControl,OutHumidLowerControl,OutHumidUpperControl
   */
  std::tuple<int8_t, int8_t, uint8_t, uint8_t,int8_t, int8_t, uint8_t, uint8_t> getRangeFor211();

  /**
   * @brief Setters for assigning values to variables.
   *
   */
  void setTemperature(float t);
  void setHumid(float h);
  void setVolumeLevel(uint8_t level);
  void setBlockSizeValue(uint32_t size);
  void setVerificationDate(QDate d);
  void setModelDevice(QString deviceModel);
  void setDataBasePath(QString path);
  void setCsvPath(QString path);
  void setPdfPath(QString path);
  void setFwVersion(QString fw);
  void setSnDevice(QString sn);
  void setDateTime(QDateTime dt);
  void setFromDateDB(QDateTime from);
  void setToDateDB(QDateTime to);
  void setDeviceInfo(QByteArray Info);
  void setDataBlock(QByteArray db);
  void setRangeValue(uint8_t range);
  void setControlSettings(bool* array);

  /**
   * @brief Sets the range for sensor 211.
   *
   * Configures the acceptable ranges for temperature and humidity for inner and outer sensors.
   *
   * @param InTempLower Inner sensor's lower bound for temperature.
   * @param InTempUpper Inner sensor's upper bound for temperature.
   * @param InHumidLower Inner sensor's lower bound for humidity.
   * @param InHumidUpper Inner sensor's upper bound for humidity.
   * @param OutTempLower Outer sensor's lower bound for temperature.
   * @param OutTempUpper Outer sensor's upper bound for temperature.
   * @param OutHumidLower Outer sensor's lower bound for humidity.
   * @param OutHumidUpper Outer sensor's upper bound for humidity.
   */
  void setRangeFor211(int8_t InTempLower, int8_t InTempUpper, uint8_t InHumidLower, uint8_t InHumidUpper,
                      int8_t OutTempLower, int8_t OutTempUpper, uint8_t OutHumidLower, uint8_t OutHumidUpper);
  void setSensorType211(bool isInner);
  ~ DeviceInfoStorage();

private:
  DeviceInfoStorage();
  DeviceInfoStorage(const DeviceInfoStorage&)=delete;
  DeviceInfoStorage& operator=(const DeviceInfoStorage)=delete;
  //========================================================================
  bool controllSettings[9]{false};
  bool isInSensor=true;
  float temperature=0.0;
  float humids=0.0;
  int8_t InTempLowerControl;
  int8_t InTempUpperControl;
  int8_t OutTempLowerControl;
  int8_t OutTempUpperControl;
  uint8_t InHumidLowerControl;
  uint8_t InHumidUpperControl;
  uint8_t OutHumidLowerControl;
  uint8_t OutHumidUpperControl;
  uint8_t VolumeLevel=0;
  uint8_t RangeValue=0;
  uint32_t BlockSizeValue=0;
  QDate VerificationDate;
  QString ModelDevice;
  QString fwVersion;
  QString snDevice;
  QString AdobePath;
  QString DBasePath;
  QString CsvPath;
  QString PdfPath;
  QDateTime dataTime;
  QDateTime FromDB;
  QDateTime ToDB;
  QByteArray DataBlock;
  QByteArray deviceInfo;
};

#endif // DEVICEINFOSTORAGE_H

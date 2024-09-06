#ifndef SERIALPORTMANAGER_H
#define SERIALPORTMANAGER_H

#include <QByteArray>
#include <QDateTime>
#include <QObject>
#include <QSerialPort>
#include <QSerialPortInfo>
#include <QEventLoop>
#include <QTimer>
#include <QtMath>

#include <QFile>

#include "deviceinfostorage.h"

class SerialPortManager:public QObject
{
  Q_OBJECT

public:
  SerialPortManager();

  QString getPortName();
  uint8_t getPortNumber();
  bool getConnectStatus();

  uint32_t processBarValue=0;

  //mask getted value for set ui settings(0-240(65-80%),1-112(50-65%), 2-48(30-50%), 3-16(20-30%); 4-8(+25,+30),5-4(+15,+25),6-4(+8,+15), 7-2(+2,+8));
  //HRange[4]={240,112,48,16};
  //TRange[4]={8,4,2,1};
  //or 0b00000000
  bool controllSettings[9]{false};

  void setSpeed(QString speed);//debug ver

  ~SerialPortManager();

public slots:
  //connect
  void auto_search_com_ports();
  void manualPortConnect();
  void closeSerialPort();
  //setters
  void setHighBaudRate();
  void setVerficationDate();
  void setVolumeLevel(uint8_t level);
  void setControlRange();
  //getters
  void getTempHumid();
  void getVolumeLevel();
  void getVerificationDate();
  void getControlRange();
  void getAllData();
  void getBlockSize();//public for debug

  //----separate read and write and sync with signal/slots
  void handleReadyRead();

  void writeControllSettings();
  void writeDateTimeFromDevice();
  void readDateTimeFromDevice();
  void readFwVersion();
  void saveSettings();
  void portNumUpdate(const uint8_t &Nport);

private slots:
  void getBlock();
  void createButtonsMaskByte();
  void readSnDevice();
  void writeData(const QByteArray &data);
  void updateData();
  void statusUpdate(const bool &status);
  void readFlashAddr(const uint8_t cmd,const uint8_t lng, const uint8_t addr, const int byteCount);
  void writeToFlashByte(const uint8_t address, const uint8_t byte);
  void controlSettings();

signals:
  void dataReady();
  void blockDataReady();
//  void deviceInfoReady();
//  void rangeReady();
//  void dataTimeReady();
//  void fwNumberReady();

private:
  uint8_t BcdToByte(uint8_t Value);
  uint8_t ByteToBcd(uint8_t Value);
  uint16_t exportBits(QByteArray &data,uint8_t &startBit,uint8_t bitlengh,uint32_t iter);
  void resetSpeed();
  void delay(int milliseconds);
  bool askSerialPort();
  bool isConnected;
  QSerialPort *serial;
  QByteArray receivedData;
  QByteArray createCommand(uint8_t cmd, uint8_t data_1, uint8_t data_2);
  DeviceInfoStorage &storage;
};

#endif // SERIALPORTMANAGER_H

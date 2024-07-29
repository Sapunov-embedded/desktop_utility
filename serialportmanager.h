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

#include <QFile>//temp

#include "deviceinfostorage.h"

class SerialPortManager:public QObject
{
  Q_OBJECT

public:
  SerialPortManager();

  QByteArray createCommand(uint8_t cmd, uint8_t data_1, uint8_t data_2);

  bool getConnectStatus();

  void delay(int milliseconds);

  QString getPortName();
  uint8_t getPortNumber();

  uint32_t processBarValue=0;

  //mask getted value for set ui settings(0-240(65-80%),1-112(50-65%), 2-48(30-50%), 3-16(20-30%); 4-8(+25,+30),5-4(+15,+25),6-4(+8,+15), 7-2(+2,+8));
  //HRange[4]={240,112,48,16};
  //TRange[4]={8,4,2,1};
  //or 0b00000000
  bool controllSettings[9]{false};

  uint8_t BcdToByte(uint8_t Value);
  uint8_t ByteToBcd(uint8_t Value);

  void setSpeed(QString speed);

  void readFwVersion();

  void readSnDevice();


  ~SerialPortManager();

public slots:
  void on_Button_connect_clicked();

  void auto_search_com_ports();

  void closeSerialPort();

  void writeData(const QByteArray &data);

  void on_Disconnect_device_clicked();

  void on_pushButton_11_clicked();

  void autoSelectBaudRate();

  void updateData();

  void on_readDateTimeFromDevice_clicked();

  void on_writeDateTimeFromDevice_clicked();

  void StatusUpdate(const bool &status);

  void getTempHumid();

  void PortNumUpdate(const uint8_t &Nport);

  //----separate read and write and sync with signal/slots
  void handleReadyRead();
  //*

  void ReadFlashAddr(const uint8_t cmd,const uint8_t lng, const uint8_t addr, const int countBytes);

  void WriteToFlashByte(const uint8_t address, const uint8_t byte);

  void ControlSettings();

  void WriteControllSettings();

  void GetVolumeLevel();

  void SetVolumeLevel(uint8_t level);

  void GetVerificationDate();

  void SetVerficationDate();

  void GetControlRange();

  void SetControlRange();

  void GetAllData();

  void saveSettings();

  void readloop();

  void HexToFile(QString FileName, QByteArray &arr);//temp

  void ReadAllMem();

  void getBlockSize();//public for debug


private slots:
  void createButtonsMaskByte();

signals:
  void dataReady();
  void blockDataReady();
  void deviceInfoReady();
  void rangeReady();
  void dataTimeReady();
  void fwNumberReady();

private:
  uint16_t exportBits(QByteArray &data,uint8_t &startBit,uint8_t bitlengh,uint32_t iter);
  int getBlock();
  void resetSpeed();
  bool askSerialPort();
  QSerialPort *serial;
  DeviceInfoStorage &storage;
  bool isConnected;
  QByteArray receivedData;
};

#endif // SERIALPORTMANAGER_H

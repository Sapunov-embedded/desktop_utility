// This class manages communication with a device via a serial port.
// It handles tasks such as port discovery, connecting to the device,
// sending and receiving data, and managing various settings.
// The class provides methods to control and retrieve information
// from the device, including temperature, humidity, volume level, and more.

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

#include "logging.h"
#include "globaldefines.h"
#include "deviceinfostorage.h"

class SerialPortManager : public QObject
{
  Q_OBJECT

public:
  SerialPortManager();


  ~SerialPortManager();

  // Getters:
  QString getPortName() const;
  uint8_t getPortNumber() const;
  bool getConnectStatus() const;

  // Public member to hold the process bar value.
  uint32_t processBarValue = 0;

  // Array to manage control settings for the device.
  bool controllSettings[9] { false };

public slots:
  // Connection management:
  void auto_search_com_ports(); // Automatically searches for available COM ports.
  void manualPortConnect(); // Manually connects to a specified port.
  void closeSerialPort(); // Closes the current serial port connection.

  // Setters:
  void setHighBaudRate(); // Sets the baud rate to a high value(230000baud) on both sides.
  void setVerificationDate();
  void setVolumeLevel(uint8_t level);
  void setControlRange();

  // Getters:
  void getTempHumid(); // Retrieves temperature and humidity data from device.
  void getVolumeLevel();
  void getVerificationDate();
  void getControlRange();
  void getAllData(); // Retrieves all available data from the device.
  void getBlockSize();

  // Other public slots:
  void handleReadyRead(); // Handles incoming data when the serial port is ready to read.
  void writeControlSettings(); // Writes control settings to the device.
  void writeDateTimeFromDevice(); // Writes date and time to the device.
  void readDateTimeFromDevice(); // Reads date and time from the device.
  void readFwVersion();
  void saveSettings(); // Saves current settings on device(use for apply new control settings on device).
  void portNumUpdate(const uint8_t &Nport);

private slots:
  void getBlock(); // Retrieves a block of data.
  void createButtonsMaskByte(); // Creates a byte mask for button control.
  void readSnDevice(); // Reads the serial number of the device.
  void writeData(const QByteArray &data); // Writes data to the device.
  void updateData();
  void statusUpdate(const bool &status); // Updates the connection status.
  void readFlashAddr(const uint8_t cmd, const uint8_t lng, const uint8_t addr, const int byteCount); // Reads data from eeprom memory.
  void writeToFlashByte(const uint8_t address, const int8_t byte); // Writes a byte to eeprom memory.
  void controlSettings();
  void controlSettings211();

signals:
  void dataReady();
  void blockDataReady();
  void verificationDateRecorded();

private:
  uint8_t BcdToByte(uint8_t Value); // Converts BCD to byte format.
  uint8_t ByteToBcd(uint8_t Value); // Converts byte to BCD format.
  uint8_t repeatTimes=0;
  uint16_t exportBits(QByteArray &data, uint8_t &startBit, uint8_t bitLength, uint32_t iter); // Exports bits from data.
  void resetSpeed(); // Resets the speed settings.
  void delay(int milliseconds);
  bool askSerialPort(); // Device confirm.
  bool isConnected; // Indicates the connection status of the serial port.
  QSerialPort *serial;
  QByteArray receivedData; // Holds received data.
  QByteArray createCommand(uint8_t cmd, uint8_t data_1, uint8_t data_2); // Creates a correct command for device.
  DeviceInfoStorage &storage;
};

#endif // SERIALPORTMANAGER_H

// This class provides the main user interface of the application.
// It manages the interaction with various components, including:
// - Serial port communication through SerialPortManager.
// - Data export to CSV and PDF formats via ExportCSV and ExportPDF.
// - Graphical representation of data using the grapthics module.
// - User configuration settings using ApplicationConfiguration.
// Additionally, it handles device interaction, logging, and time/date management.
// The class is designed to provide a cohesive interface for controlling
// and monitoring connected devices, making data export and visualization seamless.

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QDateTime>
#include <QVBoxLayout>

#include "users.h"
#include "journal.h"
#include "logging.h"
#include "exportcsv.h"
#include "grapthics.h"
#include "exportpdf.h"
#include "globaldefines.h"
#include "serialportmanager.h"
#include "exportdatafrombytes.h"
#include "applicationconfiguration.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
  Q_OBJECT

public:
  explicit  MainWindow(Users *user, SerialPortManager *SerialPM,ExportDataFromBytes *parsedData,ApplicationConfiguration *config,QWidget *parent = nullptr);


  ~MainWindow();

private slots:
  // This slot shows or hides the buttons based on the current state or conditions.
  void on_SelectPort_2_stateChanged(int arg1);
  void on_dateEdit_userDateChanged(const QDate &date);
  void on_timeEdit_userTimeChanged(const QTime &time);
  void on_getTempButton_clicked();
  void on_readFlash_clicked();
  void on_refresh_clicked();
  void on_WriteToFlash_clicked();
  void on_WriteVolumeLevel_clicked();
  void on_VerificationDate_userDateChanged(const QDate &date);
  void on_AutoSpeed_clicked();
  void on_ReadDataFromDeviceButton_clicked();
  //  void on_progressBar_valueChanged(int value);
  void on_JournalButton_clicked();
  void on_SaveToDataBaseButton_clicked();
  void on_InTempLower_valueChanged(int arg1);
  void on_InHumidLower_valueChanged(int arg1);
  void on_InTempUpper_valueChanged(int arg1);
  void on_OutTempLower_valueChanged(int arg1);
  void on_OutHumidLower_valueChanged(int arg1);
  void on_OutTempUpper_valueChanged(int arg1);
  void on_OutHumidUpper_valueChanged(int arg1);
  void on_InHumidUpper_valueChanged(int arg1);

  void on_setVerDate_clicked();
  void on_graphButton_clicked();
  void on_tableButton_clicked();

  void JournalCreated();
  void checkControlRange();
  void updateRangeValues();
  void closeSerialPort();
  void readFwVersion();
  void readSnDevice();
  void updateData();
  void ProcessConnect();
  void ShowHideConnectWindow();
  void endVerificationDate();
  void set_lcd_datatime();
  void setProgressBar();
  void saveNewMask(uint8_t cell_number, bool checked);
  void initVerificationDate();


  void on_externalSensorButton_toggled(bool checked);

private:
  bool validationTimeDate();
  void disableAllIndicates();
  Ui::MainWindow *ui;
  SerialPortManager *SerialPM;
  DeviceInfoStorage &storage;
  ExportDataFromBytes *parsed;
  ApplicationConfiguration *appConfig;
  QTimer *timer;
  QVBoxLayout *layout;
  grapthics g;
  ExportPDF window;
  Users *us;
};
#endif // MAINWINDOW_H

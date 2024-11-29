
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
#include "globaldefines.h"
#include "serialportmanager.h"
#include "exportdatafrombytes.h"
#include "applicationconfiguration.h"

extern bool enableMainFunctionality;
extern bool enableSaveToDb;

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

/**
 * @class MainWindow
 * @brief The primary user interface class for the application.
 *
 * The MainWindow class provides the main graphical user interface (GUI)
 * and manages interaction with various components of the application.
 * It serves as the central hub for controlling and monitoring connected devices,
 * as well as visualizing and exporting data.
 *
 * Key responsibilities:
 * - **Serial Port Communication:** Manages device communication through the SerialPortManager.
 * - **Data Export:** Supports exporting data to CSV and PDF formats via ExportCSV and ExportPDF modules.
 * - **Data Visualization:** Provides graphical representation of data using the graphics module.
 * - **User Configuration:** Manages application settings through ApplicationConfiguration.
 * - **Logging:** Facilitates logging of activities and events.
 * - **Time and Date Management:** Handles date and time-related operations.
 *
 * This class is designed to offer a seamless and cohesive interface for users to
 * interact with connected devices, ensuring efficient data monitoring, export, and visualization.
 */
class MainWindow : public QMainWindow
{
  Q_OBJECT

public:
  /**
      * @brief Constructs the MainWindow.
      * @param user User data handler.
      * @param SerialPM Serial port manager.
      * @param parsedData Parsed data handler.
      * @param config Application configuration manager.
      * @param parent Parent widget (optional).
      */
  explicit  MainWindow(Users *user, SerialPortManager *SerialPM,ExportDataFromBytes *parsedData,ApplicationConfiguration *config,QWidget *parent = nullptr);


  ~MainWindow();

private slots:
  /**
     * @brief Handles checkbox state changes.
     * @param arg1 Checkbox state.
     */
  void on_SelectPort_2_stateChanged(int arg1);

  /**
     * @brief Updates the date value.
     * @param date New date.
     */
  void on_dateEdit_userDateChanged(const QDate &date);

  /**
   * @brief Updates the time value.
   * @param time New time.
   */
  void on_timeEdit_userTimeChanged(const QTime &time);

  /**
   * @brief Retrieves temperature data.
   */
  void on_getTempButton_clicked();

  /**
   * @brief Reads volume level from program storage.
   */
  void on_readFlash_clicked();

  /**
   * @brief Button to update emulate device screen
   */
  void on_refresh_clicked();

  /**
   * @brief Write to flash control settings
   */
  void on_WriteToFlash_clicked();

  /**
   * @brief Write Volume Level to device
   */
  void on_WriteVolumeLevel_clicked();

  /**
   * @brief Update Verification Date
   * @param date
   */
  void on_VerificationDate_userDateChanged(const QDate &date);

  /**
   * @brief This function sets the maximum data transfer speed 230000(maximum stable speed)
   */
  void on_AutoSpeed_clicked();

  /**
   * @brief Read Data From Device.
   * Processes a byte array containing all recorded data from the device's flash memory.
   */
  void on_ReadDataFromDeviceButton_clicked();

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

  /**
   * @brief Write new verification date to device
   */
  void on_setVerDate_clicked();

  void on_graphButton_clicked();
  void on_tableButton_clicked();
  void on_externalSensorButton_toggled(bool checked);
  void on_mainTargetRadioButton_clicked();

  /**
   * @brief Creates a journal without triggering other functions.
   */
  void on_journalTargetRadioButton_clicked();

  void on_externalSensorButtonDisWin_clicked();

  /**
   * @brief Show sensor type on emulate screen
   */
  void on_innerlSensorButtonDisWin_clicked();

  void JournalCreated();
  /**
   * @brief checkControlRange and updateRangeValues
   * For 102 version device
   */
  void checkControlRange();
  void updateRangeValues();

  void closeSerialPort();
  void readFwVersion();
  void readSnDevice();

  /**
   * @brief updateData
   * Calls readFwVersion() and readSnDevice()
   * And give temperature and humid from device(current value)
   */
  void updateData();

  /**
   * @brief ProcessConnect
   * Just show in main console "Connect to device"
   */
  void ProcessConnect();

  /**
   * @brief Toggles the UI between disconnected and connected device states.
   *
   * Updates the interface from the first screen (disconnected) to the second screen (connected),
   * applying all current device settings.
   */

  void ShowHideConnectWindow();
  void endVerificationDate();
  void set_lcd_datatime();
  void setProgressBar();

  /**
   * @brief For save mask setted ranges for 102 device
   * @param cell_number
   * @param checked
   */
  void saveNewMask(uint8_t cell_number, bool checked);

  /**
   * @brief Saves a new verification date and aligns the end verification date.
   */
  void initVerificationDate();



  void on_VolumeLevel_currentIndexChanged(const QString &arg1);

private:
  void updateBoxRange102();
  void setNewMaskRange102(int tempL, int tempH, int humidL, int humidH);

  /**
   * @brief Finds the corresponding range in the range list for a given value.
   */
  void matchValueInRange();
  void setActiveSensorDisplay(bool isInner);
  bool validationTimeDate();

  /**
   * @brief Write values from current selected row in range list
   */
  void rangeProcess();

  void refreash102();
  void disableAllIndicates();
  void onlyJournalFunc();
  void setBackgroundButton(QPushButton *button, bool needAction);

  std::pair<int8_t,int8_t> tempProcess(uint8_t index);
  std::pair<uint8_t,uint8_t> humidProcess(uint8_t index);

  Ui::MainWindow *ui;
  SerialPortManager *SerialPM;
  DeviceInfoStorage &storage;///< Main program storage, It exist one example
  ExportDataFromBytes *parsed;
  ApplicationConfiguration *appConfig;
  QTimer *timer;///< Timer for periodic tasks.
  QVBoxLayout *layout;///< For set segments layout
  grapthics g;
  Users *us;
  QStringList tempList={"ниже -18°C", "от -5 до -18°C","от +2 до +8°C", "от +2 до +15°C", "от +2 до +25°C",
                        "от +2 до +30°C", "от +8 до +15°C", "от +8 до +25°C", "от +15 до +25°C", "не ниже +2°C"};
  QStringList humidList={"не более 30%", "не более 50%", "не более 65%", "не более 80%" };
};

/**
 * @class CustomComboBox
 * @brief A custom combo box with redefined click behavior to open a list of ranges.
 */
class CustomComboBox : public QComboBox {
  Q_OBJECT

public:
  using QComboBox::QComboBox;

signals:
  void popupOpened();

protected:
  void showPopup() override {
    emit popupOpened();
    QComboBox::showPopup();
  }
};

class MySpinBox : public QSpinBox {
    Q_OBJECT

public:
    explicit MySpinBox(QWidget *parent = nullptr) : QSpinBox(parent) {
    QLineEdit *editor = this->findChild<QLineEdit *>();
    if (editor) {
        editor->installEventFilter(this);
    }
  }

protected:
    void mousePressEvent(QMouseEvent *event) override {
        QSpinBox::mousePressEvent(event);
        emit valueChanged(value());
    }
    bool eventFilter(QObject *obj, QEvent *event) override {
           if (QLineEdit *editor = this->findChild<QLineEdit *>()) {
               if (obj == editor && event->type() == QEvent::MouseButtonPress) {
                   emit valueChanged(this->value());
                   return true;
               }
           }
           return QSpinBox::eventFilter(obj, event);
       }
};

/**
 * @brief Find Best Range Index in list ranges
 * @param TempL
 * @param TempH
 * @param ranges
 * @return index
 */
int findBestRangeIndex(int TempL, int TempH, const QVector<QPair<int, int>>& ranges);
std::pair<int, int> get_combined_range(int start, int end, bool isTemp);


#endif // MAINWINDOW_H

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "grapthics.h"
#include <QDateTime>
#include <QVBoxLayout>


#include "serialportmanager.h"
#include "exportcsv.h"
#include "exportdatafrombytes.h"
#include "exportpdf.h"
#include "journal.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
  explicit  MainWindow(SerialPortManager *SerialPM,ExportCSV *CSV,ExportDataFromBytes *parsedData,QWidget *parent = nullptr);


    ~MainWindow();

private slots:

    void saveNewMask(uint8_t cell_number, bool checked);

    void on_toolButton_13_clicked();

    void on_toolButton_12_clicked();

    void on_Button_connect_clicked();

    void closeSerialPort();

    void on_Disconnect_device_clicked();

    void on_SelectPort_2_stateChanged(int arg1);

    void on_pushButton_11_clicked();

    void readFwVersion();

    void readSnDevice();

    void updateData();

    void on_pushButton_10_clicked();

    void on_readDateTimeFromDevice_clicked();

    void on_writeDateTimeFromDevice_clicked();

    void on_dateEdit_userDateChanged(const QDate &date);

    void on_pushButton_16_clicked();

    void on_pushButton_18_clicked();

    void ProcessConnect();

    void ShowHideConnectWindow();

    void on_timeEdit_userTimeChanged(const QTime &time);

    void on_getTempButton_clicked();

    void set_lcd_datatime();

    void on_readFlash_clicked();

    void on_indicate_240_clicked(bool checked);

    void on_refresh_clicked();

    void on_WriteToFlash_clicked();

    void on_indicate_112_clicked(bool checked);

    void on_indicate_48_clicked(bool checked);

    void on_indicate_16_clicked(bool checked);

    void on_indicate_8_clicked(bool checked);

    void on_indicate_4_clicked(bool checked);

    void on_indicate_2_clicked(bool checked);

    void on_indicate_1_clicked(bool checked);

    void on_WriteVolumeLevel_clicked();

    void on_VerificationDate_userDateChanged(const QDate &date);

    void on_pushButton_clicked();

    void on_AutoSpeed_clicked();

    void on_speed_currentIndexChanged(const QString &arg1);

    void on_ReadDataFromDeviceButton_clicked();

    void endVerificationDate();

    void on_ReportButton_clicked();

    void on_progressBar_valueChanged(int value);

    void setProgressBar();

    void on_JournalButton_clicked();

    void on_SaveToDataBaseButton_clicked();

    void on_test_clicked();

private:
    Ui::MainWindow *ui;
    SerialPortManager *SerialPM;
    DeviceInfoStorage &storage;
    ExportCSV *CSV;
    QTimer *timer;
    QVBoxLayout *layout;
    ExportDataFromBytes *parsed;
    void initVerificationDate();
    bool validationTimeDate();
    grapthics g;
    ExportPDF window;
};
#endif // MAINWINDOW_H

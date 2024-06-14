#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "grapthics.h"
#include <QSerialPort>
#include <QSerialPortInfo>
#include<QDateTime>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);


    ~MainWindow();
QString FolderNameDB;
QString FolderNamePdf;
QString FolderNameXlsx;
QString PathToAdobe;
QByteArray deviceInfo;
QDateTime dataTime;
QString fwVersion;
grapthics g;

private slots:

    void on_FolderDataBase_clicked();

    void on_FolderTemporaryFilesPdf_clicked();

    void on_FolderTemporaryFilesXlsx_clicked();

    void on_PathToAdobe_clicked();

    void on_lineEdit_7_textChanged(const QString &arg1);

    void on_lineEdit_8_textChanged(const QString &arg1);

    void on_lineEdit_9_textChanged(const QString &arg1);

    void on_lineEdit_10_textChanged(const QString &arg1);

    void on_toolButton_13_clicked();

    void on_toolButton_12_clicked();

    void on_UserButton_clicked();

    void on_Button_connect_clicked();

    bool askSerialPort();

    void auto_search_com_ports();

    void closeSerialPort();

    void writeData(const QByteArray &data);

    void on_Disconnect_device_clicked();

    void on_SelectPort_2_stateChanged(int arg1);

    void on_pushButton_11_clicked();

    void readFwVersion();

    void readSnDevice();

    QByteArray createCommand(uint8_t cmd, uint8_t data_1, uint8_t data_2);

    void updateData();


    void on_pushButton_10_clicked();

    void on_readDateTimeFromDevice_clicked();

    void on_writeDateTimeFromDevice_clicked();

    void on_dateEdit_userDateChanged(const QDate &date);
    void on_pushButton_16_clicked();

    void on_pushButton_18_clicked();

private:
    Ui::MainWindow *ui;
    QSerialPort *serial;
};
#endif // MAINWINDOW_H

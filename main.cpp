#include "mainwindow.h"
#include "dialog.h"
#include "exportcsv.h"
#include "exportdatafrombytes.h"
#include "applicationconfiguration.h"
#include "users.h"
#include <QApplication>
#include <QDir>

int main(int argc, char *argv[])
{
    QDir dir("logs");
    dir.mkpath(".");
    QApplication a(argc, argv);
    ExportCSV CSV;
    ExportDataFromBytes parsedData;
    SerialPortManager SerialPM;
    ApplicationConfiguration appConfig;
    Users us(appConfig);
    MainWindow w(&us,&SerialPM,&CSV,&parsedData,&appConfig);
    w.show();
    Dialog d(&CSV);
    d.hide();
    return a.exec();
}

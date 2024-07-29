#include "mainwindow.h"
#include "dialog.h"
#include "exportcsv.h"
#include "exportdatafrombytes.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    ExportCSV CSV;
    ExportDataFromBytes parsedData;
    SerialPortManager SerialPM;
    MainWindow w(&SerialPM,&CSV,&parsedData);
    w.show();
    Dialog d(&CSV);
    d.hide();
    return a.exec();
}

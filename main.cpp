#include "mainwindow.h"
#include "dialog.h"
#include "exportcsv.h"
#include "exportdatafrombytes.h"
#include "applicationconfiguration.h"
#include "users.h"
#include <QApplication>
#include <QDir>


bool enableMainFunctionality = true;
bool enableSaveToDb=false;

int main(int argc, char *argv[])
{
  QDir dir("logs");
  dir.mkpath(".");

  QApplication a(argc, argv);

  QCommandLineParser parser;
  parser.setApplicationDescription("The application for download and parse data from 102/212 TMFC devices.");
  parser.addHelpOption();

  QCommandLineOption versionOption("version", "Set the version for on/off save to data base Sql lite", "V02.28.8R5");
  parser.addOption(versionOption);

  QCommandLineOption setupOption("setup", "Run program with all functional");
  parser.addOption(setupOption);

  QCommandLineOption reportOption("report", "Run program with only report function");
  parser.addOption(reportOption);

  parser.process(a);

  if (parser.isSet(versionOption)) {
      QString versionValue = parser.value(versionOption);
      qDebug() << "Version specified:" << versionValue;
      //here we check version
      if(versionValue=="V02.28.8R5"){
          enableSaveToDb=false;
        }else{
          enableSaveToDb=true;
        }
    } else {
      qDebug() << "Version parameter not specified.";
      Logging::logInfo("Version parameter not specified.");
    }

  if (parser.isSet(setupOption)) {
      enableMainFunctionality=true;
    }else if(parser.isSet(reportOption)){
      enableMainFunctionality=false;
    }


  ExportDataFromBytes parsedData;
  SerialPortManager SerialPM;
  ApplicationConfiguration appConfig;
  Users us(appConfig);
  MainWindow w(&us,&SerialPM,&parsedData,&appConfig);
  w.show();
  return a.exec();
}

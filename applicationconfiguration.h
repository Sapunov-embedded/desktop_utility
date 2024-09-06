#ifndef APPLICATIONCONFIGURATION_H
#define APPLICATIONCONFIGURATION_H

#include <QSettings>
#include <QDebug>
#include <QFile>
#include <QTextStream>
#include <QString>
#include "ini.h"
#include <fstream>

#include "deviceinfostorage.h"

class ApplicationConfiguration
{
public:
  ApplicationConfiguration();
 ~ApplicationConfiguration();

  void saveSettings();
  void setCurrentUser(QString rPers,QString company,QString city);

  QString getResPerson();
  QString getCompanyN();
  QString getCityN();

private:
  QSettings *settings= new QSettings("FS_Service.ini", QSettings::IniFormat);
  DeviceInfoStorage &storage;
  QString AdobePath;
  QString DBasePath;
  QString CsvPath;
  QString PdfPath;
  QString ResPerson;
  QString CompanyN;
  QString CityN;
};

#endif // APPLICATIONCONFIGURATION_H

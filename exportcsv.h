#ifndef EXPORTCSV_H
#define EXPORTCSV_H

#include <QFile>
#include <QTextStream>
#include <QDesktopServices>
#include <QUrl>
#include <QDate>
#include <QLocale>
#include <vector>
#include <algorithm>
#include "globaldefines.h"
#include "serialportmanager.h"


class ExportCSV: public QObject
{
  Q_OBJECT



public:
  ExportCSV();
  //getters
  bool getIncludeTableOfContents();
  bool getIncludeHeaderOnEveryPage();
  bool getShowEveryMinute();
  bool getShowEveryNMinutesEachHour();
  uint8_t getNMinutesInterval();
  bool getCalculateAverageEveryNMinutes();
  uint8_t getAverageInterval();
  bool getShowEveryMinuteMultipleOfN();
  uint8_t getNMultiple();
  QDateTime getFromDb();
  QDateTime getToDb();

public slots:
  void startExportProcess();
  //setters
  void setIncludeTableOfContents(bool status);
  void setIncludeHeaderOnEveryPage(bool status);
  void setShowEveryMinute(bool status);
  void setShowEveryNMinutesEachHour(bool status);
  void setNMinutesInterval(uint8_t interval);
  void setCalculateAverageEveryNMinutes(bool status);
  void setAverageInterval(uint8_t interval);
  void setShowEveryMinuteMultipleOfN(bool status);
  void setNMultiple(uint8_t multiple);
  void setDbDate();

signals:
  void exportDone();

private:
  QString defaultFileAddr="DB.csv";
  uint16_t exportBits(QByteArray &data,uint8_t &startBit,uint8_t bitlengh,uint32_t iter);
  DeviceInfoStorage &storage;
  QDateTime FromDb;
  QDateTime ToDb;
  bool includeTableOfContents =false;
  bool includeHeaderOnEveryPage =false;
  bool showEveryMinute =true;
  bool showEveryNMinutesEachHour  = false;
  uint8_t nMinutesInterval =1;
  bool calculateAverageEveryNMinutes  = false;
  uint8_t averageInterval =2;
  bool showEveryMinuteMultipleOfN  = false;
  uint8_t nMultiple=1;

};

#endif // EXPORTCSV_H

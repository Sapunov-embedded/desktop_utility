#ifndef EXPORTCSV_H
#define EXPORTCSV_H

#include <QUrl>
#include <QFile>
#include <QDate>
#include <QDebug>
#include <QLocale>
#include <QTextStream>
#include <QDesktopServices>
#include <vector>
#include <algorithm>
#include "logging.h"
#include "globaldefines.h"
#include "serialportmanager.h"


class ExportCSV: public QObject
{
  Q_OBJECT



public:
  ExportCSV();

  /**
   * @brief Getters for retrieving stored variables.
   *
   */
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

  /**
   * @brief Setters for assigning values to variables.
   *
   */
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


private:
  QString defaultFileAddr="DB.csv";
  DeviceInfoStorage &storage;
  QDateTime FromDb;
  QDateTime ToDb;
  bool includeTableOfContents =false;
  bool includeHeaderOnEveryPage =false;
  bool showEveryMinute =true;
  bool showEveryNMinutesEachHour  = false;
  bool calculateAverageEveryNMinutes  = false;
  bool showEveryMinuteMultipleOfN  = false;
  uint8_t nMinutesInterval =1;
  uint8_t averageInterval =2;
  uint8_t nMultiple=1;
  uint16_t exportBits(QByteArray &data,uint8_t &startBit,uint8_t bitlengh,uint32_t iter);

};

#endif // EXPORTCSV_H

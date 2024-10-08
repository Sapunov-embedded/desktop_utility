#ifndef EXPORTDATAFROMBYTES_H
#define EXPORTDATAFROMBYTES_H

#include <stdint.h>
#include <QByteArray>
#include <QObject>
#include <QVector>

#include "deviceinfostorage.h"

class ExportDataFromBytes: public QObject
{
  Q_OBJECT

public:
  ExportDataFromBytes();

  struct Data{
    QDateTime date;
    //eTemp=array[0], eHumid=array[1], iTemp=array[2], iHumid=array[3]
    std::array <float,4>values;

    Data();
    Data(const QDateTime& date, const std::array<float, 4>& values);
  };

  const QVector<Data>& getArrayValues()const;
  std::pair<float,float> getTempMaxMin();
  std::pair<float,float> getHumidMaxMin();
  std::pair<int8_t,int8_t> getTempRange();
  std::pair<uint8_t,uint8_t> getHumidRange();
  std::pair<QDateTime,QDateTime> getStartEndDate();
  bool getRangeControlStatus();
public slots:
  void ExportServiceAndDataPoints();
signals:
  void exportReady();

private:
  uint16_t exportBits(QByteArray &data,uint8_t &startBit,uint8_t bitlengh,uint32_t iter);
 // uint16_t getBytes();
  QVector<Data> storageParsedData;
  DeviceInfoStorage &storage;
  float maxT=70;
  float minT=-50;
  float maxH=100;
  float minH=0;
  QDateTime startDateTime;
  QDateTime endDateTime;
  uint8_t startTempAverage=0;
  uint8_t endTempAverage=0;
  uint8_t startHumidAverage=0;
  uint8_t endHumidAverage=0;
  int8_t startExTempAverage=0;
  int8_t endExTempAverage=0;
  uint8_t startExHumidAverage=0;
  uint8_t endExHumidAverage=0;
  bool cntrlIsOn=false;
};

#endif // EXPORTDATAFROMBYTES_H

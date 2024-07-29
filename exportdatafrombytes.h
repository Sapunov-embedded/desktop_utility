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
    std::array <float,4>values;
    Data();
    Data(const QDateTime& date, const std::array<float, 4>& values);

  };
  //uint16_t getValue();
  const QVector<Data>& getArrayValues()const;
  std::pair<float,float> getTempMaxMin();
  std::pair<float,float> getHumidMaxMin();
  std::pair<QDateTime,QDateTime> getStartEndDate();
  std::pair<int8_t,int8_t> getTempRange();
  std::pair<uint8_t,uint8_t> getHumidRange();

public slots:
  void ExportServiceAndDataPoints();
signals:
  void exportReady();

private:
  uint16_t exportBits(QByteArray &data,uint8_t &startBit,uint8_t bitlengh,uint32_t iter);
  uint16_t getBytes();
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
};

#endif // EXPORTDATAFROMBYTES_H

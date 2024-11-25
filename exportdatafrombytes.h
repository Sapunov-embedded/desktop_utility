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
  /**
   * @brief A structure to store timestamped sensor data for temperature and humidity readings.
   *
   * The `Data` struct contains a timestamp (`date`) along with temperature and humidity values
   * for two sensors. The `values` array stores these readings, with indices assigned as follows:
   * - `values[0]`: External temperature (`eTemp`)
   * - `values[1]`: External humidity (`eHumid`)
   * - `values[2]`: Internal temperature (`iTemp`)
   * - `values[3]`: Internal humidity (`iHumid`)
   *
   * @note This struct includes two constructors: a default constructor and a parameterized constructor
   *       to initialize both `date` and `values` fields.
   */
  struct Data{
    QDateTime date;
    //eTemp=array[0], eHumid=array[1], iTemp=array[2], iHumid=array[3]
    std::array <float,4>values;

    Data();
    Data(const QDateTime& date, const std::array<float, 4>& values);
  };

  /**
   * @brief Retrieves a constant reference to a vector of sensor data entries.
   *
   * This function returns a `const QVector<Data>&`, providing read-only access
   * to each data entry. Each entry in the vector includes:
   * - `date`: The timestamp of the reading.
   * - `values`: An array holding temperature and humidity readings from both sensors.
   *
   * @return const QVector<Data>& A reference to the vector containing all recorded sensor data.
   *                              Each element includes Date, Temperature, and Humidity values.
   */
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
  QVector<Data> storageParsedData;///< Vector of parsed data
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

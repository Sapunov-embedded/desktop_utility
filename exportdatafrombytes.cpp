#include "exportdatafrombytes.h"

ExportDataFromBytes::ExportDataFromBytes():storage(DeviceInfoStorage::getInstanse()){}


uint16_t ExportDataFromBytes::exportBits(QByteArray &data,uint8_t &startBit,uint8_t bitlengh,uint32_t iter){
  uint16_t value=0;
  for(int i=0;i<bitlengh;++i){
      uint32_t byteIndex = (startBit+i)/8+iter;
      uint8_t bitIndex = 7-((startBit+i)%8);
      value<<=1;
      value|=data[byteIndex]>>bitIndex & 0x01;
    }
  startBit+=bitlengh;
  return value;
};


//uint16_t ExportDataFromBytes::getBytes(){
//  uint16_t result=0;
// // result=exportBits(array,stbit,bitlangth,iter);
//  return result;
//}

void ExportDataFromBytes::ExportServiceAndDataPoints(){
  storageParsedData.clear();
  //copy data from storage class
  QByteArray data = storage.getDataBlock();

  //parse date from first Service data block(count days sinse 01.01.2015)
  uint8_t from=21;
  uint16_t DaysAfter2015=exportBits(data,from,16,0);

  QDate start(2015,1,1);
  QDate target= start.addDays(DaysAfter2015);

  //for add minutes every loop
  QTime time(0,0,0);
  QTime n_time= time;

  startDateTime=storage.getFromDateDB();
  endDateTime=storage.getToDateDB();

  uint32_t count_db=0;

  bool* arr=storage.getControlSettings();

  //parse setted range control temp and humid
  for(uint8_t it=1;it<=9;++it){
      if(it<=4){
          if(arr[it]&&!startTempAverage){
              startTempAverage=it;
            }
          if(arr[it]){
              endTempAverage=it+1;
            }
        }
      if(it<=9&&it>4){
          if(arr[it]&&!startHumidAverage){
              startHumidAverage=it-4;
            }
          if(arr[it]){
              endHumidAverage=it-3;
            }
        }
    }

  uint8_t tempArray[]{0,2,8,15,25,30};
  uint8_t humidArray[]{0,20,30,50,65,80};
  startTempAverage=tempArray[startTempAverage];
  endTempAverage=tempArray[endTempAverage];
  startHumidAverage=humidArray[startHumidAverage];
  endHumidAverage=humidArray[endHumidAverage];

  //main loop data parse
  for(int i=0;i<data.size();i+=6){
      uint8_t flags=data[i]>>4;
      bool flag1 = flags & 0x01;
      bool flag2 = flags & 0x02;
      bool flag3 = flags & 0x04;
      bool flag4 = flags & 0x08;

      if(flag1&&!flag2){
          uint8_t startBit=4;
          cntrlIsOn=exportBits(data,startBit,1,i);
          startBit++;
          int16_t extTemp=exportBits(data,startBit,11,i);
          //for negative numbers
          if(extTemp>1023){
              extTemp|=1<<15;
              extTemp|=1<<14;
              extTemp|=1<<13;
              extTemp|=1<<12;
              extTemp|=1<<11;
            }
          uint16_t extHumid=exportBits(data,startBit,10,i);
          int16_t intTemp=exportBits(data,startBit,11,i);
          //for negative numbers
          if(intTemp>1023){
              intTemp|=1<<15;
              intTemp|=1<<14;
              intTemp|=1<<13;
              intTemp|=1<<12;
              intTemp|=1<<11;
            }
          uint16_t intHumid=exportBits(data,startBit,10,i);

          float eTemp= (float)extTemp/10;
          float eHumid = (float)extHumid/10;
          float iTemp= (float)intTemp/10;
          float iHumid = (float) intHumid/10;

          if(minT>iTemp||minT==(-50)) minT=iTemp;
          if(maxT<iTemp||maxT==70) maxT=iTemp;
          if(minH>iHumid||minH==0)minH=iHumid;
          if(maxH<iHumid||maxH==100)maxH=iHumid;

          count_db++;
          time=n_time;
          n_time=time.addSecs(60);
          QDateTime date;
          date.setDate(target);
          date.setTime(n_time);
          storageParsedData.push_back({date,{eTemp,eHumid,iTemp,iHumid}});
        }

      //read service data for parse date and time
      if(flag2&&!flag1){
          uint8_t startBit=5;
          uint16_t ranges=exportBits(data,startBit,16,i);
          uint16_t DaysAfter2015=exportBits(data,startBit,16,i);
          uint16_t Minutes=exportBits(data,startBit,11,i);
          target=start.addDays(DaysAfter2015);
          n_time.setHMS(0,0,0);
          time.setHMS(0,0,0);
          n_time=time.addSecs(Minutes*60);
        }
    }

};

const QVector<ExportDataFromBytes::Data>& ExportDataFromBytes::getArrayValues()const{
  return storageParsedData;
};

ExportDataFromBytes::Data::Data():date(),values({}) {};

ExportDataFromBytes::Data::Data(const QDateTime& date, const std::array<float, 4>& values)
  : date(date), values(values){};

std::pair<float,float> ExportDataFromBytes::getTempMaxMin(){
  return std::make_pair(minT,maxT);
};
std::pair<float,float> ExportDataFromBytes::getHumidMaxMin(){
  return std::make_pair(minH,maxH);
};
std::pair<QDateTime,QDateTime> ExportDataFromBytes::getStartEndDate(){
  return std::make_pair(startDateTime,endDateTime);
};

std::pair<int8_t,int8_t> ExportDataFromBytes::getTempRange(){
  return std::make_pair(startTempAverage,endTempAverage);
};
std::pair<uint8_t,uint8_t> ExportDataFromBytes::getHumidRange(){
  return std::make_pair(startHumidAverage,endHumidAverage);
};

bool ExportDataFromBytes::getRangeControlStatus(){
  return cntrlIsOn;
};

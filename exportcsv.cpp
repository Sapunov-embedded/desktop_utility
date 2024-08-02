#include "exportcsv.h"
#include <QDebug>

ExportCSV::ExportCSV():storage(DeviceInfoStorage::getInstanse()){
  FromDb=storage.getFromDateDB();
  ToDb=storage.getToDateDB();
};


uint16_t ExportCSV::exportBits(QByteArray &data,uint8_t &startBit,uint8_t bitlengh,uint32_t iter ){
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

void ExportCSV::startExportProcess(){
  QFile f(defaultFileAddr);
  if(f.open( QIODevice::WriteOnly)){
      QTextStream out(&f);
      QString TMFC_101="ДЕТАЛЬНАЯ ТАБЛИЦА ТЕМПЕРАТУРЫ И ВЛАЖНОСТИ ГИГРОМЕТРА ТМФЦ 101\n";
      QString TMFC_211="ДЕТАЛЬНАЯ ТАБЛИЦА ТЕМПЕРАТУРЫ И ВЛАЖНОСТИ ГИГРОМЕТРА ТМФЦ 211\n";
      QString pageHeader_101="Дата/Время;Т, °С;Rh, %;Нарушение;Контроль\n";
      QString pageHeader_211="Дата/Время;Т внутр, °С;Rh внутр, %;Т внеш, °С;Rh внеш, %;Нарушение;Контроль\n";
      QString controlRange="Интервал контроля;";
      QString yes="Да";
      QString no="Нет";

      std::vector<float> middleOfTemp;
      std::vector<float> middleOfHumid;

      if(includeTableOfContents){
          out<<TMFC_101;
        }

      out<<"Серийный №;"+storage.getSnDevice()+";";

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

      QString fromDateTime=storage.getFromDateDB().toString("dd.MM.yyyy hh:mm");
      QString toDate=" по "+storage.getToDateDB().toString("dd.MM.yyyy hh:mm");
      out<<"c "<<fromDateTime<<toDate<<"\n";

      out<<pageHeader_101;
      QLocale locale(QLocale::German);

      uint32_t count_db=0;

      out<<controlRange;

      uint8_t tempArray[]{0,2,8,15,25,30};
      uint8_t humidArray[]{0,20,30,50,65,80};
      uint8_t startTempAverage=0;
      uint8_t endTempAverage=0;
      uint8_t startHumidAverage=0;
      uint8_t endHumidAverage=0;

      bool PrintMeasure= false;
      bool IsFirstPage=true;

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
      //print range
      QString celsias="°С";
      out<<"("<<tempArray[startTempAverage]<<"-"<<tempArray[endTempAverage]<<")"<<celsias<<";("<<humidArray[startHumidAverage]<<"-"<<humidArray[endHumidAverage]<<")%\n";

      uint8_t shift = 0;
      //main loop data parse
      for(int i=0;i<data.size();i+=6){
          uint8_t flags=data[i]>>4;
          bool flag1 = flags & 0x01;
          bool flag2 = flags & 0x02;
          bool flag3 = flags & 0x04;
          bool flag4 = flags & 0x08;

          if(flag1&&!flag2){

              //if enable header every page, print logic here
              if (IsFirstPage&&count_db!=0) {
                  IsFirstPage = false;
                  if (includeTableOfContents) {
                      shift = 4;
                    } else {
                      shift = 3;
                    }
                }

              if (includeHeaderOnEveryPage && (count_db % (50 - shift) == 0)&&count_db!=0&&count_db!=49&&PrintMeasure) {
                  //    qDebug()<<count_db % (50 - shift)<<" shift: "<<shift<<"count: "<<count_db;
                  out << pageHeader_101;

                  shift=1;
                }
              //end of print


              uint8_t startBit=4;

              bool rangeControl=exportBits(data,startBit,1,i);
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

              //for 211 version device
              //out<<locale.toString(eTemp,'f',1);
              //out<<";";
              //out<< locale.toString(eHumid,'f',1)+";";

              //output settings
              if(showEveryMinute){
                  showEveryNMinutesEachHour=false;
                  showEveryMinuteMultipleOfN=false;
                  //print
                  PrintMeasure=true;
                }
              else if(showEveryNMinutesEachHour){
                  showEveryMinute=false;
                  showEveryMinuteMultipleOfN=false;
                  if(nMinutesInterval==n_time.minute()){
                      //print
                      PrintMeasure=true;
                    }
                  else{
                      PrintMeasure=false;
                    }
                }
              else if(showEveryMinuteMultipleOfN){

                  showEveryNMinutesEachHour=false;
                  showEveryMinute=false;
                  if(n_time.minute()!=0){
                      if(n_time.minute()%nMultiple==0){
                          //print
                          PrintMeasure=true;
                        }
                      else{
                          PrintMeasure=false;
                        }
                    }
                }
              //start of print
              if(PrintMeasure){
                //  qDebug()<<"out data";
                  out<<fromDateTime+";";
                  out<<locale.toString(iTemp,'f',1);
                  out<<";";
                  out<< locale.toString(iHumid,'f',1)+";";
                   if((rangeControl&&iTemp<tempArray[startTempAverage])||(rangeControl&&iTemp>tempArray[endTempAverage])||(rangeControl&&iHumid<humidArray[startHumidAverage])||(rangeControl&&iHumid>humidArray[endHumidAverage])){
                       out<<yes<<";";
                     }
                   else{
                       out<<no<<";";
                     }
                  if(rangeControl){
                      out<<yes<<"\n";
                    }
                  else{
                      out<<no<<"\n";
                    }
                  count_db++;
                }
              //end of print

              if(calculateAverageEveryNMinutes && middleOfTemp.size()!=(averageInterval-1)&&middleOfHumid.size()!=(averageInterval-1)&&PrintMeasure){
                  middleOfTemp.push_back(iTemp);
                  middleOfHumid.push_back(iHumid);
                }
              else if(calculateAverageEveryNMinutes&&PrintMeasure) {
                  QString result="Среднее значение тепературы: ";
                  result+=QString::number((std::accumulate(middleOfTemp.begin(),middleOfTemp.end(),0))/middleOfTemp.size());
                  result+="°С, среднее значение влажности:  ";
                  result+=QString::number((std::accumulate(middleOfHumid.begin(),middleOfHumid.end(),0))/middleOfHumid.size());
                  result+="\n\n";
                  out<<result;
                  middleOfTemp.clear();
                  middleOfHumid.clear();
                }
              time=n_time;
              n_time=time.addSecs(60);
              fromDateTime=target.toString("dd.MM.yyyy")+" "+n_time.toString("HH:mm");
            }

          //read service data for parse date and time
          if(flag2&&!flag1){
              uint8_t startBit=5;
              uint16_t ranges=exportBits(data,startBit,16,i);
              uint16_t DaysAfter2015=exportBits(data,startBit,16,i);
              uint16_t Minutes=exportBits(data,startBit,11,i);
              target=start.addDays(DaysAfter2015);
              fromDateTime=target.toString("dd.MM.yyyy");
              n_time.setHMS(0,0,0);
              time.setHMS(0,0,0);
              n_time=time.addSecs(Minutes*60);
              fromDateTime+=" "+n_time.toString("HH:mm");
            }
        }
    }
  f.close();
};

//setters
void ExportCSV::setIncludeTableOfContents(bool state) {
  includeTableOfContents = state;
}

void ExportCSV::setIncludeHeaderOnEveryPage(bool state) {
  includeHeaderOnEveryPage = state;
}

void ExportCSV::setShowEveryMinute(bool state) {
  showEveryMinute = state;
}

void ExportCSV::setShowEveryNMinutesEachHour(bool state) {
  showEveryNMinutesEachHour = state;
}

void ExportCSV::setNMinutesInterval(uint8_t interval) {
  nMinutesInterval = interval;
}

void ExportCSV::setCalculateAverageEveryNMinutes(bool state) {
  calculateAverageEveryNMinutes = state;
}

void ExportCSV::setAverageInterval(uint8_t interval) {
  averageInterval = interval;
}

void ExportCSV::setShowEveryMinuteMultipleOfN(bool state) {
  showEveryMinuteMultipleOfN = state;
}

void ExportCSV::setNMultiple(uint8_t multiple) {
  nMultiple = multiple;
}
void ExportCSV::setDbDate(){
  FromDb=storage.getFromDateDB();
  ToDb=storage.getToDateDB();
};

bool ExportCSV::getIncludeTableOfContents(){
  return includeTableOfContents;
};
bool ExportCSV::getIncludeHeaderOnEveryPage(){
  return includeHeaderOnEveryPage;
};
bool ExportCSV::getShowEveryMinute(){
  return showEveryMinute;
};
bool ExportCSV::getShowEveryNMinutesEachHour(){
  return showEveryNMinutesEachHour;
};
uint8_t ExportCSV::getNMinutesInterval(){
  return nMinutesInterval;
};
bool ExportCSV::getCalculateAverageEveryNMinutes(){
  return  calculateAverageEveryNMinutes;
};
uint8_t ExportCSV::getAverageInterval(){
  return averageInterval;
};
bool ExportCSV::getShowEveryMinuteMultipleOfN(){
  return showEveryMinuteMultipleOfN;
};
uint8_t ExportCSV::getNMultiple(){
  return nMultiple;
};

QDateTime ExportCSV::getFromDb(){
  return FromDb;
};

QDateTime ExportCSV::getToDb(){
  return ToDb;
};


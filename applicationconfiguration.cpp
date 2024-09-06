#include "applicationconfiguration.h"


ApplicationConfiguration::ApplicationConfiguration():storage(DeviceInfoStorage::getInstanse())
{   
  QFile file("FS_Service.ini");
  if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
      qDebug() << "Unable to open the file!";
    }

  QString fileContent;
  QTextStream in(&file);
  while (!in.atEnd()) {
      QString line = in.readLine();
      fileContent += line + "\n";
    }
  file.close();

  // replace all backward slashes on forward
  fileContent.replace("\\", "/");

  // save all in temp file
  QFile outFile("temp.ini");
  if (!outFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
      qDebug() << "Unable to open the file for writing!";
    }
  QTextStream out(&outFile);
  out << fileContent;
  outFile.close();

  QSettings *settings2= new QSettings("temp.ini", QSettings::IniFormat);
  AdobePath=settings2->value("eARPath.text").toString();
  // qDebug()<<settings->defaultFormat();
  qDebug()<<"AdobePath"<<AdobePath;

  DBasePath=settings2->value("eDataDir.text").toString();
  qDebug()<<"DBasePath"<<DBasePath;
  storage.setDataBasePath(DBasePath);
  CsvPath=settings2->value("eXlsxDir.text").toString();
  qDebug()<<"CsvPath"<<CsvPath;
  storage.setCsvPath(CsvPath);
  PdfPath=settings2->value("ePdfDir.text").toString();
  qDebug()<<"PdfPath"<<PdfPath;
  storage.setPdfPath(PdfPath);
  ResPerson=settings->value("eRespPersonN.text").toString();
  qDebug()<<ResPerson;
  CompanyN=settings->value("eCompanyN.text").toString();
  qDebug()<<CompanyN;
  QStringList list=settings->value("eTHLocationN.text").toStringList();
  CityN=list.join(", ");
  qDebug()<<CityN;
  delete settings2;

}

ApplicationConfiguration::~ApplicationConfiguration()
{
  delete settings;
    if( QFile::remove("temp.ini")){
      qDebug()<<"temp.ini was removed";
     };
}

void ApplicationConfiguration::saveSettings() {  
  // first, create a file instance
  mINI::INIFile file("FS_Service.ini");
  // next, create a structure that will hold data
  mINI::INIStructure ini;
  // now we can read the file
  file.read(ini);
  // update a value
  ini["General"]["eRespPersonN.text"] = ResPerson.toStdString();
  ini["General"]["eCompanyN.text"] = CompanyN.toStdString();
  ini["General"]["eTHLocationN.text"] = CityN.toStdString();
  // write updates to file
  file.write(ini);
}

void ApplicationConfiguration::setCurrentUser(QString rPers,QString company,QString city){
      ResPerson=rPers;
      CompanyN=company;
      CityN=city;
};

QString ApplicationConfiguration::getResPerson(){
  return ResPerson;
};
QString ApplicationConfiguration::getCompanyN(){
  return CompanyN;
};
QString ApplicationConfiguration::getCityN(){
  return CityN;
};



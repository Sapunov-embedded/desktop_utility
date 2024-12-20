#include "applicationconfiguration.h"


ApplicationConfiguration::ApplicationConfiguration():storage(DeviceInfoStorage::getInstanse())
{   
  QFile file("FS_Service.ini");
  if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
      qDebug() << "Unable to open the file!";
      Logging::logInfo("Unable to open the file!");
    }

  QString fileContent;
  QTextStream in(&file);
  while (!in.atEnd()) {
      QString line = in.readLine();
      fileContent += line + "\n";
    }
  file.close();

  // Replace all backward slashes on forward
  fileContent.replace("\\", "/");

  // Write the modified content to a temporary file
  QFile outFile("temp.ini");
  if (!outFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
      qDebug() << "Unable to open the file for writing!";
      Logging::logInfo("Unable to open the file for writing!");
    }

  QTextStream out(&outFile);
  out << fileContent;
  outFile.close();
  mINI::INIFile fil("temp.ini");
  mINI::INIStructure ini;
  fil.read(ini);

  auto settings2= std::make_unique<QSettings>("temp.ini", QSettings::IniFormat);

  //AdobePath=settings2->value("eARPath.text").toString();
  AdobePath=QString::fromStdString(ini["General"]["eARPath.text"]);
  Logging::logInfo("AdobePath " +AdobePath.toStdString());
  qDebug()<<"AdobePath"<<AdobePath;

  //DBasePath=settings2->value("eDataDir.text").toString();
  DBasePath=QString::fromStdString(ini["General"]["eDataDir.text"]);
  Logging::logInfo("DBasePath "+DBasePath.toStdString());
  storage.setDataBasePath(DBasePath);
  qDebug()<<"DBasePath"<<DBasePath;

 // CsvPath=settings2->value("eXlsxDir.text").toString();
  CsvPath=QString::fromStdString(ini["General"]["eXlsxDir.text"]);
  Logging::logInfo("CsvPath "+CsvPath.toStdString());
  storage.setCsvPath(CsvPath);
  qDebug()<<"CsvPath"<<CsvPath;

 // PdfPath=settings2->value("ePdfDir.text").toString();
  PdfPath=QString::fromStdString(ini["General"]["ePdfDir.text"]);
  Logging::logInfo("PdfPath "+PdfPath.toStdString());
  storage.setPdfPath(PdfPath);
  qDebug()<<"PdfPath"<<PdfPath;


  ResPerson= QString::fromStdString(ini["General"]["eRespPersonN.text"]);
  Logging::logInfo("RespPerson "+ResPerson.toStdString());
  qDebug()<<ResPerson;

  CompanyN= QString::fromStdString(ini["General"]["eCompanyN.text"]);
  Logging::logInfo("CompanyN "+CompanyN.toStdString());
  qDebug()<<CompanyN;

  CityN=QString::fromStdString(ini["General"]["eTHLocationN.text"]);
  Logging::logInfo("CityN "+CityN.toStdString());
  qDebug()<<CityN;
}

ApplicationConfiguration::~ApplicationConfiguration()
{
  if( QFile::remove("temp.ini")){
      qDebug()<<"temp.ini was removed";
    };
}

void ApplicationConfiguration::saveSettings() {  
  mINI::INIFile file("FS_Service.ini");
  mINI::INIStructure ini;
  file.read(ini);

  // Update values and write them to the INI file
  ini["General"]["eRespPersonN.text"] = ResPerson.toStdString();
  ini["General"]["eCompanyN.text"] = CompanyN.toStdString();
  ini["General"]["eTHLocationN.text"] = CityN.toStdString();
  file.write(ini);
}

void ApplicationConfiguration::setCurrentUser(QString rPers,QString company,QString city){
  ResPerson=std::move(rPers);
  CompanyN=std::move(company);
  CityN=std::move(city);
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



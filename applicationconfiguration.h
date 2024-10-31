#ifndef APPLICATIONCONFIGURATION_H
#define APPLICATIONCONFIGURATION_H

#include <QFile>
#include <QDebug>
#include <memory>
#include <fstream>
#include <QString>
#include <QSettings>
#include <QTextStream>

#include "ini.h"
#include "logging.h"
#include "deviceinfostorage.h"

/**
 * @class ApplicationConfiguration
 * @brief Handles reading configuration data from a shared .ini file and normalizes Windows paths.
 *
 * The ApplicationConfiguration class reads settings from `FS_Service.ini`, including Windows paths,
 * and converts backslashes to forward slashes for cross-platform compatibility, preventing misinterpretation
 * of escape sequences. It also stores user-specific information such as the resource person, company, and city.
 */

class ApplicationConfiguration
{
public:
  /**
   * @brief Constructs an ApplicationConfiguration object.
   *
   * This constructor initializes the application configuration by:
   * - Loading settings from the `FS_Service.ini` file.
   * - Creating a temporary configuration file named `temp.ini`.
   * - Normalizing file path separators from backward slashes to forward slashes.
   *
   * It also parses the paths for PDF files, the Adobe executable,
   * CSV files, and database connections, storing the relevant
   * information in the program's storage for later use.
   */

  ApplicationConfiguration();

  /**
   * @brief Destructor delete tempory file temp.ini
   */

  ~ApplicationConfiguration();

  /**
   * @brief Saves the configuration settings to the FS_Service.ini file.
   *
   * This function checks if the values for `RespPerson`, `Company`,
   * and `Location` have changed. If any of these values have been
   * modified, it updates the corresponding entries in the database.
   * Additionally, it saves the new values to the `FS_Service.ini`
   * file without corrupting any existing data.
   */

  void saveSettings();

  void setCurrentUser(QString rPers, QString company, QString city);

  QString getResPerson();
  QString getCompanyN();
  QString getCityN();

private:
  std::unique_ptr<QSettings> settings = std::make_unique<QSettings>("FS_Service.ini", QSettings::IniFormat);
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

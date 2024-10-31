#ifndef DIALOG_H
#define DIALOG_H

#include <memory>
#include <QDialog>
#include "exportcsv.h"

namespace Ui {
  class Dialog;
}
/**
 * @class Dialog
 * @brief User interface for configuring CSV export parameters.
 *
 * Provides an interface for setting parameters required to create a CSV table.
 * This class interacts with the ExportCSV class, which handles the business logic
 * for generating the CSV file based on user-defined settings.
 */

class Dialog : public QDialog
{
  Q_OBJECT

public:
  explicit Dialog(QWidget *parent = nullptr);
  ~Dialog();


private slots:
  void on_buttonBox_accepted();

  void on_includeTableOfContents_stateChanged(int arg1);

  void on_includeHeaderOnEveryPage_stateChanged(int arg1);

  void on_showEveryMinute_stateChanged(int arg1);

  void on_showEveryNMinutesEachHour_stateChanged(int arg1);

  void on_nMinutesInterval_valueChanged(int arg1);

  void on_calculateAverageEveryNMinutes_stateChanged(int arg1);

  void on_averageInterval_valueChanged(int arg1);

  void on_showEveryMinuteMultipleOfN_stateChanged(int arg1);

  void on_nMultiple_valueChanged(int arg1);

private:
  DeviceInfoStorage &storage;       ///< Reference to device information storage.
  std::unique_ptr<Ui::Dialog> ui_2; ///< Pointer to the UI elements for the dialog.
  std::unique_ptr<ExportCSV> CSV;   ///< Contains business logic for creating the CSV table.
};

#endif // DIALOG_H

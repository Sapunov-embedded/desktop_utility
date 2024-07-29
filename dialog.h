#ifndef DIALOG_H
#define DIALOG_H

#include <QDialog>
#include "exportcsv.h"

namespace Ui {
class Dialog;
}

class Dialog : public QDialog
{
    Q_OBJECT

public:
    explicit Dialog(ExportCSV *CSV,QWidget *parent = nullptr);
    ~Dialog();

public slots:

signals:
  void runCSV();


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
    Ui::Dialog *ui_2;
    ExportCSV *CSV;
};

#endif // DIALOG_H

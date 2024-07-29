#include "dialog.h"
#include "ui_dialog.h"
#include <QDebug>

Dialog::Dialog(ExportCSV *CSV,QWidget *parent) :
    QDialog(parent),
    ui_2(new Ui::Dialog),CSV(CSV)
{
    ui_2->setupUi(this);
    CSV->setDbDate();
    QString str=CSV->getFromDb().toString("dd.MM.yyyy hh:mm");
    ui_2->fromDate->setText(str);
    ui_2->toDate->setText(CSV->getToDb().toString("dd.MM.yyyy hh:mm"));



    if(CSV->getIncludeTableOfContents()){
        ui_2->includeTableOfContents->setChecked( true);
      }
    else{
         ui_2->includeTableOfContents->setChecked(false);
      }

    if(CSV->getIncludeHeaderOnEveryPage()){
        ui_2->includeHeaderOnEveryPage->setChecked(true);
      }
    else{
        ui_2->includeHeaderOnEveryPage->setChecked(false);
      }

    if(CSV->getShowEveryMinute()){
        ui_2->showEveryMinute->setChecked(true);
      }
    else{
      ui_2->showEveryMinute->setChecked(false);
    }

    if(CSV->getShowEveryNMinutesEachHour()){
        ui_2->showEveryNMinutesEachHour->setChecked(true);
      }
    else{
        ui_2->showEveryNMinutesEachHour->setChecked(false);
      }
    ui_2->nMinutesInterval->setValue( CSV->getNMinutesInterval());

    if(CSV->getCalculateAverageEveryNMinutes()){
        ui_2->calculateAverageEveryNMinutes->setChecked(true);
      }
    else{
         ui_2->calculateAverageEveryNMinutes->setChecked(false);
      }

    ui_2->averageInterval->setValue(CSV->getAverageInterval());

    if(CSV->getShowEveryMinuteMultipleOfN()){
        ui_2->showEveryMinuteMultipleOfN->setChecked(true);
      }
    else{
        ui_2->showEveryMinuteMultipleOfN->setChecked(false);
      }

    ui_2->nMultiple->setValue(CSV->getNMultiple());

}

Dialog::~Dialog()
{
    delete ui_2;
}

void Dialog::on_buttonBox_accepted()
{
    CSV->startExportProcess();
}

void Dialog::on_includeTableOfContents_stateChanged(int arg1)
{
    if(arg1==2){
        CSV->setIncludeTableOfContents(true);
      }
    else if(arg1==0){
        CSV->setIncludeTableOfContents(false);
      }
}


void Dialog::on_includeHeaderOnEveryPage_stateChanged(int arg1)
{
  if(arg1==2){
      CSV->setIncludeHeaderOnEveryPage(true);
    }
  else if(arg1==0){
      CSV->setIncludeHeaderOnEveryPage(false);
    }
}


void Dialog::on_showEveryMinute_stateChanged(int arg1)
{
  if(arg1==2){
      CSV->setShowEveryMinute(true);

      CSV->setShowEveryNMinutesEachHour(false);
      ui_2->showEveryNMinutesEachHour->setChecked(false);

      CSV->setShowEveryMinuteMultipleOfN(false);
      ui_2->showEveryMinuteMultipleOfN->setChecked(false);
    }
  else if(arg1==0){
      CSV->setShowEveryMinute(false);
    }
}


void Dialog::on_showEveryNMinutesEachHour_stateChanged(int arg1)
{
  if(arg1==2){
      CSV->setShowEveryNMinutesEachHour(true);

      CSV->setShowEveryMinute(false);
      ui_2->showEveryMinute->setChecked(false);

      CSV->setShowEveryMinuteMultipleOfN(false);
      ui_2->showEveryMinuteMultipleOfN->setChecked(false);
    }
  else if(arg1==0){
      CSV->setShowEveryNMinutesEachHour(false);
    }
}

void Dialog::on_nMinutesInterval_valueChanged(int arg1)
{
    CSV->setNMinutesInterval(arg1);
}


void Dialog::on_calculateAverageEveryNMinutes_stateChanged(int arg1)
{
  if(arg1==2){
      CSV->setCalculateAverageEveryNMinutes(true);
    }
  else if(arg1==0){
      CSV->setCalculateAverageEveryNMinutes(false);
    }
}


void Dialog::on_averageInterval_valueChanged(int arg1)
{
    CSV->setAverageInterval(arg1);
}


void Dialog::on_showEveryMinuteMultipleOfN_stateChanged(int arg1)
{
  if(arg1==2){
      CSV->setShowEveryMinuteMultipleOfN(true);

      CSV->setShowEveryMinute(false);
      ui_2->showEveryMinute->setChecked(false);

      CSV->setShowEveryNMinutesEachHour(false);
      ui_2->showEveryNMinutesEachHour->setChecked(false);
    }
  else if(arg1==0){
      CSV->setShowEveryMinuteMultipleOfN(false);
    }
}


void Dialog::on_nMultiple_valueChanged(int arg1)
{
    CSV->setNMultiple(arg1);
}


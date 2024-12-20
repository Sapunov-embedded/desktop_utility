#ifndef GRAPTHICS_H
#define GRAPTHICS_H

#include <QWidget>
#include <memory>
#include "qcustomplot.h"
#include "globaldefines.h"
#include "deviceinfostorage.h"
#include "exportdatafrombytes.h"

namespace Ui {
class grapthics;
}

class grapthics : public QWidget
{
    Q_OBJECT

public:
    explicit grapthics(ExportDataFromBytes *exp,QWidget *parent = nullptr);


    ~grapthics();
public slots:
  void drawGraph();

private slots:

  void on_inTemp_stateChanged(int arg1);

  void on_outTemp_stateChanged(int arg1);

  void on_iHumid_stateChanged(int arg1);

  void on_outHumid_stateChanged(int arg1);

  void on_partialPressure_stateChanged(int arg1);

  void on_dewPoint_stateChanged(int arg1);

  void on_pushButton_2_clicked();

  void on_comboBox_currentIndexChanged(int index);

  void on_controlTemp_clicked(bool checked);

  void on_controlHumid_clicked(bool checked);

  void on_Print_clicked();

  void print();

  void on_printToPdf_clicked();

  void setupPrinter(QPrinter &printer, const QString &filePath);

  void setupPainter(QPainter &painter, QPrinter &printer);

  void renderContent(QPainter &painter, QPrinter &printer);

  void on_controlTempOut_clicked(bool checked);

  void on_controlHumidOut_clicked(bool checked);

private:
    std::unique_ptr<Ui::grapthics> ui;
    DeviceInfoStorage& storage;
    QPen iTempPen,eTempPen,iHumidPen,eHumidPen,dewPen,pPressurePen;
    ExportDataFromBytes *expData;///<instance in main function
    uint64_t RangeInSeconds=60;
    double calculatePartialPressure(float Temp, float Humid);
    double calculateDewPoint(float temperature, float relativeHumidity);
    QCPItemLine *UpperTempControlLine;///<dont care about delete this pointer
    QCPItemLine *LowerTempControlLine;///<dont care about delete this pointer
    QCPItemLine *UpperHumidControlLine;///<dont care about delete this pointer
    QCPItemLine *LowerHumidControlLine;///<dont care about delete this pointer
    QCPItemLine *LowerExTempControlLine;///<dont care about delete this pointer
    QCPItemLine *UpperExHumidControlLine;///<dont care about delete this pointer
    QCPItemLine *LowerExHumidControlLine;///<dont care about delete this pointer
    QCPItemLine *UpperExTempControlLine;///<dont care about delete this pointer



    QString graphHeader="График регистрации температуры за период:";
};

#endif // GRAPTHICS_H

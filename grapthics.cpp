#include "grapthics.h"
#include "ui_grapthics.h"

grapthics::grapthics(ExportDataFromBytes *exp,QWidget *parent) :
  QWidget(parent),
  ui(new Ui::grapthics),storage(DeviceInfoStorage::getInstanse()),expData(exp)
{
  ui->setupUi(this);
  UpperTempControlLine=new QCPItemLine (ui->graphicsView);
  UpperTempControlLine->setVisible(false);
  LowerTempControlLine=new QCPItemLine (ui->graphicsView);
  LowerTempControlLine->setVisible(false);
  UpperHumidControlLine=new QCPItemLine (ui->graphicsView);
  UpperHumidControlLine->setVisible(false);
  LowerHumidControlLine=new QCPItemLine (ui->graphicsView);
  LowerHumidControlLine->setVisible(false);

  iTempPen.setColor(QColor(255, 0, 0));
  eTempPen.setColor(QColor(255, 0, 255));
  iHumidPen.setColor(QColor(0, 0, 255));
  eHumidPen.setColor(QColor(85, 255, 255));
  pPressurePen.setColor(QColor(255, 170, 0));
  dewPen.setColor(QColor(191, 220, 190));

  ui->graphicsView->yAxis->setRange((-30),100);
  ui->graphicsView->xAxis->setRange(0,30);
  ui->graphicsView->yAxis->setLabel("Температрура/Влажность/Давление/Точка россы");
  ui->graphicsView->xAxis->setLabel("Время в мин");



  ui->snDevice->setText(storage.getSnDevice());

  for(uint8_t it=0;it<6;++it){
      ui->graphicsView->addGraph();
    }

  ui->graphicsView->graph(0)->setPen(iTempPen);
  ui->graphicsView->graph(0)->setVisible(false);
  ui->graphicsView->graph(1)->setPen(eTempPen);
  ui->graphicsView->graph(1)->setVisible(false);
  ui->graphicsView->graph(2)->setPen(iHumidPen);
  ui->graphicsView->graph(2)->setVisible(false);
  ui->graphicsView->graph(3)->setPen(eHumidPen);
  ui->graphicsView->graph(3)->setVisible(false);
  ui->graphicsView->graph(4)->setPen(pPressurePen);
  ui->graphicsView->graph(4)->setVisible(false);
  ui->graphicsView->graph(5)->setPen(dewPen);
  ui->graphicsView->graph(5)->setVisible(false);
}

grapthics::~grapthics()
{
  delete ui;
}


void grapthics::drawGraph(){
  QVector<double> iTemp;
  QVector<double> iHumid;
  QVector<double> eTemp;
  QVector<double> eHumid;
  QVector<double> pPressure;
  QVector<double> dewPoint;
  QVector<double> time;
  expData->ExportServiceAndDataPoints();

  ui->fromDateTime->setText("с "+storage.getFromDateDB().toString("dd.MM.yyyy hh:mm"));
  ui->ToDateTime->setText("по "+storage.getToDateDB().toString("dd.MM.yyyy hh:mm"));

  const QVector<ExportDataFromBytes::Data>& data=expData->getArrayValues();
  qDebug() << "Data size:" << data.size();
  if (data.isEmpty()) {
      qWarning("Data is empty!");
      return;
    }
  double step=0;
  qint64 distance=data[0].date.secsTo(data[data.size()-1].date)/60;
  for(int it=0;it<data.size();++it){
      if(it%RangeInMinutes==0){
          eTemp.push_back(static_cast<double>(data[it].values[0]));
          eHumid.push_back(static_cast<double>(data[it].values[1]));
          iTemp.push_back(static_cast<double>(data[it].values[2]));
          iHumid.push_back(static_cast<double>(data[it].values[3]));
          pPressure.push_back(calculatePartialPressure(data[it].values[2],data[it].values[3]));
          dewPoint.push_back(calculateDewPoint(data[it].values[2],data[it].values[3]));
          time.push_back(step+=RangeInMinutes);
        }
    }

  ui->graphicsView->graph(0)->setData(time,iTemp);
  ui->graphicsView->graph(1)->setData(time,eTemp);
  ui->graphicsView->graph(2)->setData(time,iHumid);
  ui->graphicsView->graph(3)->setData(time,eHumid);
  ui->graphicsView->graph(4)->setData(time,pPressure);
  ui->graphicsView->graph(5)->setData(time,dewPoint);

  ui->graphicsView->yAxis->setRange((expData->getTempMaxMin().first)-20,100);
  ui->graphicsView->xAxis->setRange(0,distance);

  ui->graphicsView->replot();

}

void grapthics::on_inTemp_stateChanged(int arg1)
{
  if(arg1==2){
      ui->graphicsView->graph(0)->setVisible(true);
      ui->graphicsView->replot();
    }
  else{
      ui->graphicsView->graph(0)->setVisible(false);
      ui->graphicsView->replot();
    }
}


void grapthics::on_outTemp_stateChanged(int arg1)
{
  if(arg1==2){
      ui->graphicsView->graph(1)->setVisible(true);
      ui->graphicsView->replot();
    }
  else{
      ui->graphicsView->graph(1)->setVisible(false);
      ui->graphicsView->replot();
    }
}


void grapthics::on_iHumid_stateChanged(int arg1)
{
  if(arg1==2){
      ui->graphicsView->graph(2)->setVisible(true);
      ui->graphicsView->replot();
    }
  else{
      ui->graphicsView->graph(2)->setVisible(false);
      ui->graphicsView->replot();
    }
}


void grapthics::on_outHumid_stateChanged(int arg1)
{
  if(arg1==2){
      ui->graphicsView->graph(3)->setVisible(true);
      ui->graphicsView->replot();
    }
  else{
      ui->graphicsView->graph(3)->setVisible(false);
      ui->graphicsView->replot();
    }
}


void grapthics::on_partialPressure_stateChanged(int arg1)
{
  if(arg1==2){
      ui->graphicsView->graph(4)->setVisible(true);
      ui->graphicsView->replot();
    }
  else{
      ui->graphicsView->graph(4)->setVisible(false);
      ui->graphicsView->replot();
    }
}


void grapthics::on_dewPoint_stateChanged(int arg1)
{
  if(arg1==2){
      ui->graphicsView->graph(5)->setVisible(true);
      ui->graphicsView->replot();
    }
  else{
      ui->graphicsView->graph(5)->setVisible(false);
      ui->graphicsView->replot();
    }

}

void grapthics::on_pushButton_2_clicked()
{
  drawGraph();
}


void grapthics::on_comboBox_currentIndexChanged(int index)
{
  RangeInMinutes= ui->comboBox->itemText(index).toInt();
}

double grapthics::calculatePartialPressure(float Temp, float Humid){
  double saturationPressure= 6.1078 * std::pow(10, (7.5 * Temp) / (Temp + 237.3));
  return (Humid / 100.0) * saturationPressure;
};

double grapthics::calculateDewPoint(float temperature, float relativeHumidity) {
  const double a = 17.27;
  const double b = 237.7;
  double alpha = (a * temperature) / (b + temperature) + std::log(relativeHumidity / 100.0);
  float dewPoint = (b * alpha) / (a - alpha);
  return dewPoint;
}

void grapthics::on_controlTemp_clicked(bool checked)
{
  if(checked){
      auto TempRange=expData->getTempRange();
      UpperTempControlLine->setVisible(true);
      UpperTempControlLine->start->setCoords(ui->graphicsView->xAxis->range().lower, TempRange.first);
      UpperTempControlLine->end->setCoords(ui->graphicsView->xAxis->range().upper, TempRange.first);
      LowerTempControlLine->setVisible(true);
      LowerTempControlLine->start->setCoords(ui->graphicsView->xAxis->range().lower, TempRange.second);
      LowerTempControlLine->end->setCoords(ui->graphicsView->xAxis->range().upper, TempRange.second);
      UpperTempControlLine->setPen(QPen(Qt::red));
      LowerTempControlLine->setPen(QPen(Qt::red));
      ui->graphicsView->replot();
    }
  else{
      UpperTempControlLine->setVisible(false);
      LowerTempControlLine->setVisible(false);
      ui->graphicsView->replot();
    }
}


void grapthics::on_controlHumid_clicked(bool checked)
{
  if(checked){
      auto HumidRange=expData->getHumidRange();
      UpperHumidControlLine->setVisible(true);
      UpperHumidControlLine->start->setCoords(ui->graphicsView->xAxis->range().lower, HumidRange.first);
      UpperHumidControlLine->end->setCoords(ui->graphicsView->xAxis->range().upper, HumidRange.first);
      LowerHumidControlLine->setVisible(true);
      LowerHumidControlLine->start->setCoords(ui->graphicsView->xAxis->range().lower, HumidRange.second);
      LowerHumidControlLine->end->setCoords(ui->graphicsView->xAxis->range().upper, HumidRange.second);
      UpperHumidControlLine->setPen(QPen(Qt::blue));
      LowerHumidControlLine->setPen(QPen(Qt::blue));
      ui->graphicsView->replot();
    }
  else{
      UpperHumidControlLine->setVisible(false);
      LowerHumidControlLine->setVisible(false);
      ui->graphicsView->replot();
    }
}


void grapthics::on_Print_clicked()
{
  print();
}

void grapthics::setupPrinter(QPrinter &printer, const QString &filePath)
{
  printer.setOutputFileName(filePath);
  printer.setResolution(600);
  printer.setPaperSize(QPrinter::A4);
  printer.setFullPage(true);
}

void grapthics::setupPainter(QPainter &painter, QPrinter &printer)
{
  if (!painter.begin(&printer)) {
      qWarning() << "Failed to begin painting.";
      return;
    }
}

void grapthics::renderContent(QPainter &painter, QPrinter &printer)
{
    QSize contentSize = this->ui->graphicsView->size();
    QRect pageRect = printer.pageRect();
    int pageWidth = pageRect.width();
    int pageHeight = pageRect.height();

    qreal scaleFactor = 5;
    qreal xOffset = (pageWidth - (contentSize.width() * scaleFactor)) / 2.0;
    qreal yOffset = 50;

    // Draw text
    QFont font = painter.font();
    font.setPointSize(12);
    painter.setFont(font);

    qreal textYPos = yOffset+200;
    QString text = graphHeader + " с " + storage.getFromDateDB().toString("dd.MM.yyyy hh:mm") + " по " + storage.getToDateDB().toString("dd.MM.yyyy hh:mm");
    painter.drawText(xOffset, textYPos, text);

    // Calculate the height of the text to determine the offset
    QFontMetrics metrics(font);
    int textHeight = metrics.height();

    // Adjust yOffset to ensure the content is drawn below the text
    yOffset += textHeight + 300;  // Adding a 20-pixel margin for clarity

    // Render the graphics view content
    painter.save();
    painter.translate(xOffset, yOffset);
    painter.scale(scaleFactor, scaleFactor);
    this->ui->graphicsView->render(&painter);
    painter.restore();
}


void grapthics::on_printToPdf_clicked()
{
  QString filePath = QFileDialog::getSaveFileName(this, "Save PDF", "", "*.pdf");
  if (filePath.isEmpty())
    return;

  QPrinter printer(QPrinter::HighResolution);
  printer.setOutputFormat(QPrinter::PdfFormat);

  printer.setOutputFileName(filePath);
  setupPrinter(printer, filePath);


  if (!printer.isValid()) {
      qWarning() << "Printer is not valid.";
      return;
    }

  QPainter painter;
  if (!painter.begin(&printer)) {
      qWarning() << "Failed to begin painting.";
      return;
    }


  //  qDebug() << "Printing to PDF:";
  //  qDebug() << "File Path:" << filePath;
  //  qDebug() << "Printer Resolution:" << printer.resolution();

  renderContent(painter, printer);
  painter.end();
}

void grapthics::print()
{
  QPrinter printer(QPrinter::HighResolution);
  printer.setOutputFormat(QPrinter::NativeFormat);

  setupPrinter(printer, QString());

  QPrintDialog printDialog(&printer, this);
  if (printDialog.exec() != QDialog::Accepted) {
      qWarning() << "Print dialog was cancelled.";
      return;
    }

  QPainter painter;
  if (!painter.begin(&printer)) {
      qWarning() << "Failed to begin painting.";
      return;
    }


  //  qDebug() << "Printing to physical printer:";
  //  qDebug() << "Printer Resolution:" << printer.resolution();

  renderContent(painter, printer);

  painter.end();
}

void grapthics::debugRenderContent(const QSize &contentSize, const QRect &pageRect, qreal scaleFactor, qreal xOffset, qreal yOffset)
{
  qDebug() << "Content Size:" << contentSize;
  qDebug() << "Page Rect:" << pageRect;
  qDebug() << "Scale Factor:" << scaleFactor;
  qDebug() << "X Offset:" << xOffset;
  qDebug() << "Y Offset:" << yOffset;
}




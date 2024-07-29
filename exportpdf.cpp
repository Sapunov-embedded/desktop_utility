#include "exportpdf.h"


ExportPDF::ExportPDF(QWidget *parent):QWidget(parent)
{

          QVBoxLayout *layout = new QVBoxLayout(this);
          QPushButton *exportButton = new QPushButton("Export to PDF", this);
          QPushButton *printButton = new QPushButton("Print", this);
          layout->addWidget(exportButton);
          layout->addWidget(printButton);
          setLayout(layout);

//          connect(exportButton, &QPushButton::clicked, this, &ExportPDF::exportToPdf);
//          connect(printButton, &QPushButton::clicked, this, &ExportPDF::print);
}

void ExportPDF::startExportToPdf()
{}

//void ExportPDF::exportToPdf(typeOfPdf type){
////  if(type==graphics){
////  QString filePath = QFileDialog::getSaveFileName(this, "Save PDF", "", "*.pdf");
////          if (filePath.isEmpty())
////              return;

////          QPrinter printer(QPrinter::HighResolution);
////          printer.setOutputFormat(QPrinter::PdfFormat);
////          printer.setOutputFileName(filePath);

////          QPainter painter(&printer);
////          this->render(&painter);
////          painter.end();
////    }
////  else if(type==journal){

////    }
//}

//void ExportPDF::print(){
////          QPrinter printer;
////          QPrintDialog printDialog(&printer, this);

////          if (printDialog.exec() == QDialog::Accepted) {
////              QPainter painter(&printer);
////              this->render(&painter);
////              painter.end();
////          }
//}




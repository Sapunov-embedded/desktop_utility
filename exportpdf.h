#ifndef EXPORTPDF_H
#define EXPORTPDF_H
#include <QObject>
#include <QApplication>
#include <QWidget>
#include <QPushButton>
#include <QVBoxLayout>
#include <QPrinter>
#include <QPainter>
#include <QPrintDialog>
#include <QFileDialog>

enum typeOfPdf{
  graphics,
  journal
};

class ExportPDF: public QWidget
{
  Q_OBJECT

public:
  ExportPDF(QWidget *parent = nullptr);
  void startExportToPdf();

//   void exportToPdf(typeOfPdf type);
//   void print();

private:

};

#endif // EXPORTPDF_H

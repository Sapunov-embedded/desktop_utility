#include "journal.h"

Journal::Journal(const QString &location, const QString &organization, const QString &responsiblePerson,QVector<ExportDataFromBytes::Data> &exp)
  : location(location), organization(organization), responsiblePerson(responsiblePerson),entries(exp)
{

}

void Journal::createJournal(const QString &fileName) {
  QTextDocument document;

  // Create a QTextCursor to navigate and modify the document
  QTextCursor cursor(&document);
  //temp definition
  product = "101";
  sensorType="Inside sensor";
  serialNumber="110111";
  nextCheckDate.setDate(2021,10,10);
  //end area

  drawHeader(document);

  // Create a QTextTableFormat and set up the table format
  QTextTableFormat tableFormat;
  tableFormat.setBorder(1);
  tableFormat.setBorderBrush(QBrush(Qt::black)); // Set border color
  tableFormat.setCellPadding(10);
  tableFormat.setCellSpacing(0);

  // Insert a table with 2 rows and 3 columns (1 header row spanning 3 columns + 1 data row)
  int rows = 2;
  int columns = 3;
  QTextTable *table = cursor.insertTable(rows, columns, tableFormat);

  // Format the header row
  QTextCharFormat headerFormat;
  headerFormat.setFontWeight(QFont::Bold); // Make header bold
  headerFormat.setFontPointSize(12); // Set header font size
  headerFormat.setBackground(QBrush(Qt::lightGray)); // Set header background color

  // Merge cells in the first row to span all columns
  QTextTableCell headerCell = table->cellAt(0, 0);
  QTextCursor headerCursor = headerCell.firstCursorPosition();
  headerCursor.insertText("Main Header or Title", headerFormat);
  table->mergeCells(0, 0, 1, columns); // Merge first row cells across all columns

  // Format the data row
  QTextCharFormat dataFormat;
  dataFormat.setFontPointSize(10); // Set data font size

  // Set the data values
  QStringList data = {"Value 1", "Value 2", "Value 3"};
  for (int column = 0; column < columns; ++column) {
      QTextTableCell dataCell = table->cellAt(1, column); // Second row
      QTextCursor dataCursor = dataCell.firstCursorPosition();
      dataCursor.insertText(data[column], dataFormat);
    }

  // Create a QPdfWriter
  QPdfWriter writer(fileName);

  // Optional: Set some parameters for the PDF (e.g., resolution, page size)
  writer.setResolution(300);
  writer.setPageSize(QPageSize(QPageSize::A4));

  // Print the QTextDocument to the PDF
  document.print(&writer);
}

void Journal::drawHeader(QTextDocument &document) const {
  QString dirDevice="Местоположение Изделия\n";
  QString org="Организация\n";
  QString dev="Изделие: ";
  QString discDev="гигрометр ТМФЦ ";
  QString sens="Датчик: ";
  QString s_numb="Серийный № ";
  QString ver="Следующая поверка ";
  QString resPer="Ответсвенное лицо";
  QString signature="Подпись";

  QTextCharFormat Bold;
  Bold.setFontWeight(QFont::Bold);

  QTextCharFormat underlineFormat;
  underlineFormat.setFontUnderline(true); // Enable underline

  QTextCharFormat normal;
  normal.setFontWeight(QFont::Normal);

  QTextBlockFormat blockFormatRight;
  blockFormatRight.setAlignment(Qt::AlignLeft);

  blockFormatRight.setLeftMargin(1500); // Adjust the margin as needed

  QTextBlockFormat blockFormatLeft;

  QTextCursor cursor(&document);


  cursor.setBlockFormat(blockFormatLeft);
  cursor.insertText(dirDevice, Bold);
  cursor.insertText(location, underlineFormat);
  cursor.insertText("\n");
  cursor.setBlockFormat(blockFormatLeft);
  cursor.insertText(dev, Bold);
  cursor.insertText(product,normal);
  cursor.insertText("\n");
  cursor.insertText(sens,Bold);
  cursor.insertText(sensorType+"\n",normal);
  cursor.insertText(s_numb,Bold);
  cursor.insertText(serialNumber+"\n",normal);
  cursor.insertText(ver,Bold);
  cursor.insertText(nextCheckDate.toString("dd.MM.yyyy")+"\n",normal);




  cursor.setBlockFormat(blockFormatRight);
  cursor.insertText(org,Bold);
  cursor.insertText(organization,underlineFormat);
  cursor.insertText("\n");
  cursor.insertText(resPer,Bold);
  cursor.insertText("\n");
  cursor.insertText(responsiblePerson,underlineFormat);
  cursor.insertText("\n");
  cursor.insertText(signature,Bold);
  cursor.insertText("\n");
  cursor.insertText("                              ",underlineFormat);
  cursor.insertText("\n");
}
void Journal::drawEntries(QPainter &painter) const {
  painter.setFont(QFont("Arial", 12));
  QFont ft=QFont("Arial", 12);
  QFontMetrics fm(ft);
  int hSpace=fm.horizontalAdvance(' ');
  int vSpace=fm.lineSpacing();
  int y = 250;
  int x=0;
  for (auto &entry : entries) {
      painter.drawText(x+=hSpace, y, entry.date.date().toString("dd.MM.yyyy"));
      painter.drawText(hSpace, y, entry.date.time().toString("hh:mm"));
      painter.drawText(hSpace, y, QString::number(entry.values[2], 'f', 1) + "°C");
      painter.drawText(hSpace, y, QString::number(entry.values[3], 'f', 1) + "%");
      y += vSpace;
    }
}

#include "journal.h"

Journal::Journal(const QString &location, const QString &organization, const QString &responsiblePerson,QVector<ExportDataFromBytes::Data> &exp)
  : location(location), organization(organization), responsiblePerson(responsiblePerson),entries(exp)
{
  calculateRangeOfEachDay();
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
  cursor.insertText("\n\n");
  // Create a QTextTableFormat and set up the table format
  QTextTableFormat tableFormat;
  tableFormat.setBorder(1);
  tableFormat.setBorderBrush(QBrush(Qt::black)); // Set border color
  tableFormat.setCellPadding(5);
  tableFormat.setCellSpacing(0);
  tableFormat.setLeftMargin(150);
  QVector<QTextLength> constraints;
  constraints << QTextLength(QTextLength::FixedLength, 100)   // First column width
              << QTextLength(QTextLength::FixedLength, 100)   // Second column width
              << QTextLength(QTextLength::FixedLength, 100)  // Third column width
              << QTextLength(QTextLength::FixedLength, 100)  // fourth column width
              << QTextLength(QTextLength::FixedLength, 100);  // fourth column width
  tableFormat.setColumnWidthConstraints(constraints);
  int rows=4;//default for test
  // Insert a table with 2 rows and 3 columns (1 header row spanning 3 columns + 1 data row)
  if(entries.size()!=0){
      rows = (reportData.size()*2);
    }

  int columns = 5;
  QTextTable *table = cursor.insertTable(rows, columns, tableFormat);

  // Format the header row
  QTextCharFormat headerFormat;
  headerFormat.setFontWeight(QFont::Normal); // Make header bold
  headerFormat.setFontPointSize(8); // Set header font size
  QTextBlockFormat format;
  format.setAlignment(Qt::AlignCenter);
  cursor.setBlockFormat(format);

  // Merge cells in the first row to span all columns
  QTextTableCell headerCell = table->cellAt(0, 1);
  QTextCursor headerCursor = headerCell.firstCursorPosition();
  headerCursor.setBlockFormat(format);
  headerCursor.insertText("Показания", headerFormat);
  table->mergeCells(0,0,2,1);
  table->mergeCells(0, 1, 1, columns-1); // Merge first row cells across all columns

  // Format the data row
  QTextCharFormat dataFormat;
  dataFormat.setFontPointSize(8); // Set data font size

  // Set the data values
  QStringList data = {"Дата","Время", "T,°С", "Rh, %", "Примечание"};
  for (int column = 0; column < columns; ++column) {
      QTextTableCell dataCell = table->cellAt(1, column); // Second row
      QTextCursor dataCursor = dataCell.firstCursorPosition();
      dataCursor.setBlockFormat(format);
      dataCursor.insertText(data[column], dataFormat);
    }

  drawEntries(*table);

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

  QTextBlockFormat blockFormatLeft;

  QTextCursor cursor(&document);

  // Create a QTextTableFormat and set up the table format
  QTextTableFormat tableFormat;
  tableFormat.setBorder(1);
  tableFormat.setBorderBrush(QBrush(Qt::transparent)); // Set border color
  tableFormat.setCellPadding(10);
  tableFormat.setCellSpacing(0);
  QVector<QTextLength> constraints;
  constraints << QTextLength(QTextLength::FixedLength, 200)   // First column width
              << QTextLength(QTextLength::FixedLength, 200)   // Second column width
              << QTextLength(QTextLength::FixedLength, 200);  // Third column width

  tableFormat.setColumnWidthConstraints(constraints);
  // Insert a table with 2 rows and 3 columns (1 header row spanning 3 columns + 1 data row)
  int rows = 1;
  int columns = 3;
  QTextTable *table = cursor.insertTable(rows, columns, tableFormat);

  // Set the data values
  QTextTableCell dataCell = table->cellAt(0, 0); // First row, first column
  QTextCursor dataCursor = dataCell.firstCursorPosition();
  dataCursor.insertText(dirDevice, Bold);
  dataCursor.insertText(location+"\n", underlineFormat);
  dataCursor.insertText(dev, Bold);
  dataCursor.insertText(discDev+product+"\n",normal);
  dataCursor.insertText(sens,Bold);
  dataCursor.insertText(sensorType+"\n",normal);
  dataCursor.insertText(s_numb,Bold);
  dataCursor.insertText(serialNumber+"\n",normal);
  dataCursor.insertText(ver,Bold);
  dataCursor.insertText(nextCheckDate.toString("dd.MM.yyyy")+"\n",normal);

  // Move to the second column
  dataCell = table->cellAt(0, 2); // First row, second column
  dataCursor = dataCell.firstCursorPosition(); // Update cursor to the new cell
  dataCursor.insertText(org,Bold);
  dataCursor.insertText(organization+"\n",underlineFormat);
  dataCursor.insertText(resPer+"\n",Bold);
  dataCursor.insertText(responsiblePerson+"\n",underlineFormat);
  dataCursor.insertText(signature+"\n",Bold);
  dataCursor.insertText("                                     ",underlineFormat);


  dataCursor.movePosition(QTextCursor::NextBlock);
  QTextBlockFormat format;
  QTextCharFormat charFormat;

  format.setAlignment(Qt::AlignHCenter );
  charFormat.setFontPointSize(12);
  charFormat.setFontWeight(QFont::Bold);

  dataCursor.setBlockFormat(format);

  dataCursor.insertText("\n\nЖурнал регистрации температуры и влажности\n",charFormat);

  charFormat.setFontPointSize(10);
  charFormat.setFontWeight(QFont::Normal);
  DeviceInfoStorage &storage=DeviceInfoStorage::getInstanse();

  dataCursor.insertText("c "+storage.getFromDateDB().toString("dd.MM.yyyy")+" по "+storage.getToDateDB().toString("dd.MM.yyyy"),charFormat);

}
void Journal::drawEntries(QTextTable& table) const {
  QTextTableCell dataCell = table.cellAt(2, 0);
  QTextCursor dataCursor = dataCell.firstCursorPosition();
  int row=2;
  for(int it=0;it<reportData.size()-1;++it){
      dataCell = table.cellAt(row, 0);
      dataCursor = dataCell.firstCursorPosition();
      dataCursor.insertText(reportData[it].date.toString("dd.MM.yyyy"));

      dataCell = table.cellAt(row, 1);
      dataCursor = dataCell.firstCursorPosition();
      dataCursor.insertText("8:00");

      dataCell = table.cellAt(row, 2);
      dataCursor = dataCell.firstCursorPosition();
      dataCursor.insertText(QString::number(reportData[it].minTemp));

      dataCell = table.cellAt(row, 3);
      dataCursor = dataCell.firstCursorPosition();
      dataCursor.insertText(QString::number(reportData[it].minHumid));

      dataCell = table.cellAt(row+1, 1);
      dataCursor = dataCell.firstCursorPosition();
      dataCursor.insertText("20:00");

      dataCell = table.cellAt(row+1, 2);
      dataCursor = dataCell.firstCursorPosition();
      dataCursor.insertText(QString::number(reportData[it].maxTemp));

      dataCell = table.cellAt(row+1, 3);
      dataCursor = dataCell.firstCursorPosition();
      dataCursor.insertText(QString::number(reportData[it].maxHumid));

      table.mergeCells(row,0,2,1);
      row+=2;
    }


}

void Journal::calculateRangeOfEachDay(){
  QTime startTime(8,00,00);
  QTime endTime(20,00,00);
  QDate currentDay;
  std::vector<float> temp;
  std::vector<float> humids;
  for (auto &entry : entries) {
      if(!currentDay.isValid()){
          currentDay=entry.date.date();
        }
      if(entry.date.date()!=currentDay){
          auto tempMin=std::min_element(temp.begin(), temp.end());
          auto tempMax=std::max_element(temp.begin(), temp.end());
          auto humidMin=std::min_element(humids.begin(),humids.end());
          auto humidMax=std::max_element(humids.begin(),humids.end());
          MidleOfDay midle{currentDay,*tempMin,*tempMax,*humidMin,*humidMax};
          reportData.push_back(midle);
          temp.clear();
          humids.clear();
          currentDay=entry.date.date();
        }
      else{
          if(entry.date.time()>startTime&&entry.date.time()<endTime){
              temp.push_back(entry.values[2]);
              humids.push_back(entry.values[3]);
            }
        }
    }
};

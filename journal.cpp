#include "journal.h"

Journal::Journal(const QString &location, const QString &organization, const QString &responsiblePerson,QVector<ExportDataFromBytes::Data> &exp)
  : location(location), organization(organization), responsiblePerson(responsiblePerson),storage(DeviceInfoStorage::getInstanse()),entries(exp)
{
  calculateRangeOfEachDay();
  product=storage.getModelDevice();
  if(product=="101"){
      sensorType="Внутренний датчик";
    }
  else{
      sensorType="Внутренний или внешний датчик";//temp, realization than 211
    }
  serialNumber=storage.getSnDevice();
  QDate last_Verification=storage.getVerificationDate();
  QDate last_Verification2= last_Verification.addYears(2);
  nextCheckDate=last_Verification2.toString("dd.MM.yyyy");
}

void Journal::createJournal(const QString &fileName) {
  QTextDocument document;
  // Create a QTextCursor to navigate and modify the document
  QTextCursor cursor(&document);

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
      rows = (reportData.size()*2)+2;
      qDebug()<<"rows"<<rows;
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
  emit JournalCreateDone();
  QUrl fileUrl = QUrl::fromLocalFile(fileName);
  QDesktopServices::openUrl(fileUrl);
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
  dataCursor.insertText(nextCheckDate+"\n",normal);

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

  dataCursor.insertText("c "+storage.getFromDateDB().toString("dd.MM.yyyy")+" по "+storage.getToDateDB().toString("dd.MM.yyyy"),charFormat);

}
//void Journal::drawEntries(QTextTable& table) const {
//  QTextTableCell dataCell = table.cellAt(2, 0);
//  QTextCursor dataCursor = dataCell.firstCursorPosition();
//  int row=2;

//  for(int it=0;it<reportData.size()-1;it++){
//      dataCell = table.cellAt(row, 0);
//      dataCursor = dataCell.firstCursorPosition();
//      dataCursor.insertText(reportData[it].date.toString("dd.MM.yyyy"));

//      dataCell = table.cellAt(row, 1);
//      dataCursor = dataCell.firstCursorPosition();
//      dataCursor.insertText("8:00");

//      dataCell = table.cellAt(row, 2);
//      dataCursor = dataCell.firstCursorPosition();

//      if(reportData[it].middleTempFirstHalf!=1000){
//          dataCursor.insertText(QString::number(reportData[it].middleTempFirstHalf, 'f', 1));
//        }else{
//          dataCursor.insertText("-");
//        }

//      dataCell = table.cellAt(row, 3);
//      dataCursor = dataCell.firstCursorPosition();

//      if(reportData[it].middleHumidFirstHalf!=1000){
//          dataCursor.insertText(QString::number(reportData[it].middleHumidFirstHalf, 'f', 1));
//        }else{
//          dataCursor.insertText("-");
//        }

//      dataCell = table.cellAt(row+1, 1);
//      dataCursor = dataCell.firstCursorPosition();
//      dataCursor.insertText("20:00");

//      dataCell = table.cellAt(row+1, 2);
//      dataCursor = dataCell.firstCursorPosition();

//      if(reportData[it].middleTempSecondHalf!=1000){
//          dataCursor.insertText(QString::number(reportData[it].middleTempSecondHalf, 'f', 1));
//        }else{
//          dataCursor.insertText("-");
//        }

//      dataCell = table.cellAt(row+1, 3);
//      dataCursor = dataCell.firstCursorPosition();

//      if(reportData[it].middleTempSecondHalf!=1000){
//          dataCursor.insertText(QString::number(reportData[it].middleHumidSecondHalf, 'f', 1));
//        }else{
//          dataCursor.insertText("-");
//        }

//      table.mergeCells(row,0,2,1);
//      row+=2;
//    }
//}
void Journal::drawEntries(QTextTable& table) const {
    int row = 2; // Starting from the third row, assuming the first two rows are headers

    for (int it = 0; it < reportData.size(); ++it) {
        // First half of the day
        QTextTableCell dataCell = table.cellAt(row, 0);
        QTextCursor dataCursor = dataCell.firstCursorPosition();
        dataCursor.insertText(reportData[it].date.toString("dd.MM.yyyy"));

        dataCell = table.cellAt(row, 1);
        dataCursor = dataCell.firstCursorPosition();
        dataCursor.insertText("8:00");

        dataCell = table.cellAt(row, 2);
        dataCursor = dataCell.firstCursorPosition();

        if (reportData[it].middleTempFirstHalf != 1000) {
            dataCursor.insertText(QString::number(reportData[it].middleTempFirstHalf, 'f', 1));
        } else {
            dataCursor.insertText("-");
        }

        dataCell = table.cellAt(row, 3);
        dataCursor = dataCell.firstCursorPosition();

        if (reportData[it].middleHumidFirstHalf != 1000) {
            dataCursor.insertText(QString::number(reportData[it].middleHumidFirstHalf, 'f', 1));
        } else {
            dataCursor.insertText("-");
        }

        // Second half of the day
        dataCell = table.cellAt(row + 1, 1);
        dataCursor = dataCell.firstCursorPosition();
        dataCursor.insertText("20:00");

        dataCell = table.cellAt(row + 1, 2);
        dataCursor = dataCell.firstCursorPosition();

        if (reportData[it].middleTempSecondHalf != 1000) {
            dataCursor.insertText(QString::number(reportData[it].middleTempSecondHalf, 'f', 1));
        } else {
            dataCursor.insertText("-");
        }

        dataCell = table.cellAt(row + 1, 3);
        dataCursor = dataCell.firstCursorPosition();

        if (reportData[it].middleHumidSecondHalf != 1000) {
            dataCursor.insertText(QString::number(reportData[it].middleHumidSecondHalf, 'f', 1));
        } else {
            dataCursor.insertText("-");
        }

        // Merge the date cells for the two rows
        table.mergeCells(row, 0, 2, 1);

        // Move to the next set of rows
        row += 2;
    }
}




//if prev date, current date and next date all diferent, current date save 0;
//This problem should not occur if the data is continuous
//void Journal::calculateRangeOfEachDay(){
//  QTime startTimeF(00,00,00);
//  QTime endTimeF(11,59,00);
//  QTime startTimeS(12,00,00);
//  QTime endTimeS(23,59,00);
//  QDateTime currentDay;
//  std::vector<float> tempFirstHalf;
//  std::vector<float> tempSecondHalf;
//  std::vector<float> humidFirstHalf;
//  std::vector<float> humidSecondHalf;

//  for (auto &entry : entries) {
//      if(!currentDay.isValid()){
//          currentDay=entry.date;
//        }

//      if(entry.date.date()!=currentDay.date()){
//          float tempFirst=0.0;
//          float tempSecond=0.0;
//          float humidFirst=0.0;
//          float humidSecond=0.0;

//          if(!tempFirstHalf.empty()){
//              tempFirst=std::accumulate(tempFirstHalf.begin(), tempFirstHalf.end(),0)/static_cast<float>(tempFirstHalf.size());
//            }else{
//              tempFirst=1000; //just for mark zero data
//            }

//          if(!tempSecondHalf.empty()){
//              tempSecond =std::accumulate(tempSecondHalf.begin(), tempSecondHalf.end(),0)/static_cast<float>(tempSecondHalf.size());
//            }else{
//              tempSecond=1000; //just for mark zero data
//            }
//          if(!humidFirstHalf.empty()){
//              humidFirst =std::accumulate(humidFirstHalf.begin(), humidFirstHalf.end(),0)/static_cast<float>(humidFirstHalf.size());
//            }else{
//              humidFirst=1000; //just for mark zero data
//            }
//          if(!humidSecondHalf.empty()){
//              humidSecond=std::accumulate(humidSecondHalf.begin(), humidSecondHalf.end(),0)/static_cast<float>(humidSecondHalf.size());
//            }else{
//              humidSecond=1000; //just for mark zero data
//            }

//          MiddleOfDay middle{currentDay,tempFirst,tempSecond,humidFirst,humidSecond};
//          reportData.push_back(middle);
//          tempFirstHalf.clear();
//          tempSecondHalf.clear();
//          humidFirstHalf.clear();
//          humidSecondHalf.clear();
//          currentDay=entry.date;
//        }
//      else{
//          if(entry.date.time()>startTimeF&&entry.date.time()<endTimeF){
//              tempFirstHalf.push_back(entry.values[2]);
//              humidFirstHalf.push_back(entry.values[3]);
//            } else if(entry.date.time()>startTimeS&&entry.date.time()<endTimeS){
//              tempSecondHalf.push_back(entry.values[2]);
//              humidSecondHalf.push_back(entry.values[3]);
//            }
//        }
//    }
//};
//void Journal::calculateRangeOfEachDay() {
//    QTime startTimeF(00, 00, 00);
//    QTime endTimeF(11, 59, 00);
//    QTime startTimeS(12, 00, 00);
//    QTime endTimeS(23, 59, 00);
//    QDateTime currentDay;
//    std::vector<float> tempFirstHalf;
//    std::vector<float> tempSecondHalf;
//    std::vector<float> humidFirstHalf;
//    std::vector<float> humidSecondHalf;

//    for (auto &entry : entries) {
//        if (!currentDay.isValid()) {
//            currentDay = entry.date;
//        }

//        if (entry.date.date() != currentDay.date()) {
//            float tempFirst = tempFirstHalf.empty() ? 1000 : std::accumulate(tempFirstHalf.begin(), tempFirstHalf.end(), 0.0f) / tempFirstHalf.size();
//            float tempSecond = tempSecondHalf.empty() ? 1000 : std::accumulate(tempSecondHalf.begin(), tempSecondHalf.end(), 0.0f) / tempSecondHalf.size();
//            float humidFirst = humidFirstHalf.empty() ? 1000 : std::accumulate(humidFirstHalf.begin(), humidFirstHalf.end(), 0.0f) / humidFirstHalf.size();
//            float humidSecond = humidSecondHalf.empty() ? 1000 : std::accumulate(humidSecondHalf.begin(), humidSecondHalf.end(), 0.0f) / humidSecondHalf.size();

//            MiddleOfDay middle{currentDay, tempFirst, tempSecond, humidFirst, humidSecond};
//            reportData.push_back(middle);

//            // Clear the temporary data for the new day
//            tempFirstHalf.clear();
//            tempSecondHalf.clear();
//            humidFirstHalf.clear();
//            humidSecondHalf.clear();

//            currentDay = entry.date;
//        }

//        // Add entry values to the correct half-day based on the time
//        if (entry.date.time() >= startTimeF && entry.date.time() <= endTimeF) {
//            tempFirstHalf.push_back(entry.values[2]);
//            humidFirstHalf.push_back(entry.values[3]);
//        } else if (entry.date.time() >= startTimeS && entry.date.time() <= endTimeS) {
//            tempSecondHalf.push_back(entry.values[2]);
//            humidSecondHalf.push_back(entry.values[3]);
//        }
//    }

////    // Handle the last day
//    if (currentDay.isValid()) {
//        float tempFirst = tempFirstHalf.empty() ? 1000 : std::accumulate(tempFirstHalf.begin(), tempFirstHalf.end(), 0.0f) / tempFirstHalf.size();
//        float tempSecond = tempSecondHalf.empty() ? 1000 : std::accumulate(tempSecondHalf.begin(), tempSecondHalf.end(), 0.0f) / tempSecondHalf.size();
//        float humidFirst = humidFirstHalf.empty() ? 1000 : std::accumulate(humidFirstHalf.begin(), humidFirstHalf.end(), 0.0f) / humidFirstHalf.size();
//        float humidSecond = humidSecondHalf.empty() ? 1000 : std::accumulate(humidSecondHalf.begin(), humidSecondHalf.end(), 0.0f) / humidSecondHalf.size();

//        MiddleOfDay middle{currentDay, tempFirst, tempSecond, humidFirst, humidSecond};
//        reportData.push_back(middle);
//    }
//}
void Journal::calculateRangeOfEachDay() {
    QTime startTimeF(00, 00, 00);
    QTime endTimeF(11, 59, 00);
    QTime startTimeS(12, 00, 00);
    QTime endTimeS(23, 59, 00);
    QDateTime currentDay;
    std::vector<float> tempFirstHalf;
    std::vector<float> tempSecondHalf;
    std::vector<float> humidFirstHalf;
    std::vector<float> humidSecondHalf;

    for (auto &entry : entries) {
        if (!currentDay.isValid()) {
            currentDay = entry.date;
      qDebug()<<"no valid date";
        }

        if (entry.date.date() != currentDay.date()) {
            float tempFirst = tempFirstHalf.empty() ? 1000 : std::accumulate(tempFirstHalf.begin(), tempFirstHalf.end(), 0.0f) / tempFirstHalf.size();
            float tempSecond = tempSecondHalf.empty() ? 1000 : std::accumulate(tempSecondHalf.begin(), tempSecondHalf.end(), 0.0f) / tempSecondHalf.size();
            float humidFirst = humidFirstHalf.empty() ? 1000 : std::accumulate(humidFirstHalf.begin(), humidFirstHalf.end(), 0.0f) / humidFirstHalf.size();
            float humidSecond = humidSecondHalf.empty() ? 1000 : std::accumulate(humidSecondHalf.begin(), humidSecondHalf.end(), 0.0f) / humidSecondHalf.size();

            MiddleOfDay middle{currentDay, tempFirst, tempSecond, humidFirst, humidSecond};
            reportData.push_back(middle);
qDebug()<<currentDay.date().toString("dd.MM.yyyy");
qDebug()<<entry.date.date().toString("dd.MM.yyyy");
            // Clear the temporary data for the new day
            tempFirstHalf.clear();
            tempSecondHalf.clear();
            humidFirstHalf.clear();
            humidSecondHalf.clear();

            currentDay = entry.date;
        }

        // Add entry values to the correct half-day based on the time
        if (entry.date.time() >= startTimeF && entry.date.time() <= endTimeF) {
            tempFirstHalf.push_back(entry.values[2]);
            humidFirstHalf.push_back(entry.values[3]);
        } else if (entry.date.time() >= startTimeS && entry.date.time() <= endTimeS) {
            tempSecondHalf.push_back(entry.values[2]);
            humidSecondHalf.push_back(entry.values[3]);
        }
    }

    // Handle the last day (if it hasn't been processed inside the loop)
    if (!tempFirstHalf.empty() || !tempSecondHalf.empty() || !humidFirstHalf.empty() || !humidSecondHalf.empty()) {
        float tempFirst = tempFirstHalf.empty() ? 1000 : std::accumulate(tempFirstHalf.begin(), tempFirstHalf.end(), 0.0f) / tempFirstHalf.size();
        float tempSecond = tempSecondHalf.empty() ? 1000 : std::accumulate(tempSecondHalf.begin(), tempSecondHalf.end(), 0.0f) / tempSecondHalf.size();
        float humidFirst = humidFirstHalf.empty() ? 1000 : std::accumulate(humidFirstHalf.begin(), humidFirstHalf.end(), 0.0f) / humidFirstHalf.size();
        float humidSecond = humidSecondHalf.empty() ? 1000 : std::accumulate(humidSecondHalf.begin(), humidSecondHalf.end(), 0.0f) / humidSecondHalf.size();

        MiddleOfDay middle{currentDay, tempFirst, tempSecond, humidFirst, humidSecond};
        reportData.push_back(middle);
    }
}

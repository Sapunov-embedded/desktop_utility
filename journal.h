#ifndef JOURNAL_H
#define JOURNAL_H

#include <QObject>
#include <QPdfWriter>
#include <QTextDocument>
#include <QTextCursor>
#include <QTextTableCell>
#include <QPrinter>
#include <QPainter>
#include <QDateTime>
#include <QDesktopServices>
#include <QUrl>
#include <QString>
#include "exportdatafrombytes.h"
#include "deviceinfostorage.h"

class Journal : public QObject
{
  Q_OBJECT

public:
  Journal(const QString &location, const QString &organization, const QString &responsiblePerson,QVector<ExportDataFromBytes::Data> &exp);

  struct MiddleOfDay{
    QDateTime date;
    float middleTempFirstHalf;
    float middleTempSecondHalf;
    float middleHumidFirstHalf;
    float middleHumidSecondHalf;
  };

public slots:
 void createJournal(const QString &fileName);

signals:
 void JournalCreateDone();

private:
     QString location;
     QString organization;
     QString responsiblePerson;
     QString product;
     QString sensorType;
     QString serialNumber;
     QString nextCheckDate;
     DeviceInfoStorage &storage;
     QVector<ExportDataFromBytes::Data> &entries;
     QVector<MiddleOfDay> reportData;
     void drawHeader(QTextDocument &document) const;
     void drawEntries(QTextTable &table) const;
     void calculateRangeOfEachDay();
};

#endif // JOURNAL_H

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
#include <QString>
#include "exportdatafrombytes.h"

class Journal : public QObject
{
  Q_OBJECT

public:
  Journal(const QString &location, const QString &organization, const QString &responsiblePerson,QVector<ExportDataFromBytes::Data> &exp);

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
     QDate nextCheckDate;

     QVector<ExportDataFromBytes::Data> &entries;

     void drawHeader(QTextDocument &document) const;
     void drawEntries(QPainter &painter) const;

};

#endif // JOURNAL_H

#ifndef USERS_H
#define USERS_H

#include <QDialog>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlTableModel>
#include <QSqlError>
#include "globaldefines.h"
#include "deviceinfostorage.h"
#include "exportdatafrombytes.h"
#include "applicationconfiguration.h"
//#include <QAbstractButton>
#include <QDebug>

namespace Ui {
class Users;
}

class Users : public QDialog
{
    Q_OBJECT

public:
    explicit Users(ApplicationConfiguration &config,QWidget *parent = nullptr);
    ~Users();

private slots:
  void on_plus_clicked();
  void on_minus_clicked();
  void on_upButton_clicked();
  void on_addUser_clicked();
  void on_deleteUser_clicked();

  void on_update_clicked();



  void on_buttonBox_accepted();

private:
    Ui::Users *ui;
    QSqlDatabase db;
    QSqlTableModel *model;
    DeviceInfoStorage &storage;
    ApplicationConfiguration &appConfig;
    QString filePath="FS_Service.db";
    QString s_n;
    QModelIndex getSelectedIndex();
    int UserId=-1;
    bool initializeDatabase();
    void setFilter(QString filter);
    void deleteTable(QString tableName);
    int getSelectedValueFromColum(int columNumber);
    QString getSelectedStringValue(int columN);
    void setRusNameColums();
    //  void restoreData();
    // bool saveData(const QString &name, int age);
};

#endif // USERS_H

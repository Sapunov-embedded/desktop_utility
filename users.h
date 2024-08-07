#ifndef USERS_H
#define USERS_H

#include <QDialog>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlTableModel>
#include <QSqlError>
#include <QDebug>

namespace Ui {
class Users;
}

class Users : public QDialog
{
    Q_OBJECT

public:
    explicit Users(QWidget *parent = nullptr);
    ~Users();

private slots:
  void on_plus_clicked();

  void on_minus_clicked();

private:
    Ui::Users *ui;
    bool initializeDatabase();
    bool saveData(const QString &name, int age);
    void restoreData();
};

#endif // USERS_H

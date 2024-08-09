#include "users.h"
#include "ui_users.h"

Users::Users(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Users)
{
    ui->setupUi(this);

    initializeDatabase();
}

Users::~Users()
{
  db.close();
  QSqlDatabase::removeDatabase(filePath);
    delete ui;
}

bool Users::initializeDatabase() {
  // Set up the database connection
   db = QSqlDatabase::addDatabase("QSQLITE");
// db.setDatabaseName("example.db");
  db.setDatabaseName(filePath);


  if (!db.open()) {
      qDebug() << "Error: connection with database failed";
    } else {
      qDebug() << "Database: connection ok";
    }
      qDebug()<<db.tables();
  // Create and configure the model
  model = new QSqlTableModel(this, db);
  model->setTable("User");
  model->select(); // This will load the data from the table into the model


  model->setHeaderData(1,Qt::Horizontal, tr("Организация"));
  model->setHeaderData(2,Qt::Horizontal, tr("Адрес"));
  model->setHeaderData(3,Qt::Horizontal, tr("Ответсвенное лицо"));

  // Set the model to the table view
  ui->tableView->setModel(model);

  // Additional table view setup
  ui->tableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
  ui->tableView->verticalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
  ui->tableView->setSelectionBehavior(QAbstractItemView::SelectRows);
  ui->tableView->setEditTriggers(QAbstractItemView::DoubleClicked | QAbstractItemView::SelectedClicked);
  ui->tableView->setContextMenuPolicy(Qt::CustomContextMenu);
  // connect(ui->tableView, &QTableView::customContextMenuRequested, this, &MainWindow::showContextMenu);
  ui->tableView->setWindowTitle("QTableView Example");
  ui->tableView->setColumnHidden(0, true);
  return true;
}

bool Users::saveData(const QString &name, int age) {
  QSqlQuery query;

  query.prepare("INSERT INTO User (UserID, Organization, Location, ResponsiblePerson) VALUES (:UserID, :Organization, :Location, :ResponsiblePerson)");

  query.bindValue(":Organization", name);
  query.bindValue(":UserID", age);
  query.bindValue(":Location", name);
  query.bindValue(":ResponsiblePerson", name);

    // Debugging: Print the query and bound values
    qDebug() << "Prepared query:" << query.lastQuery();
    qDebug() << "Bound values:";
    for (const auto &key : query.boundValues().keys()) {
        qDebug() << key << "=" << query.boundValue(key);
    }
  if (!query.exec()) {
      qDebug() << "Error: Could not insert data -" << query.lastError();
      return false;
    }

  return true;
}

void Users::restoreData() {
    QSqlQuery query("SELECT id, name, age FROM people");

    while (query.next()) {
        int id = query.value(0).toInt();
        QString name = query.value(1).toString();
        int age = query.value(2).toInt();

        qDebug() << "ID:" << id << "Name:" << name << "Age:" << age;
    }
}


void Users::on_plus_clicked()
{
  int row=model->rowCount();
  model->insertRow(row);
}




void Users::on_minus_clicked()
{
  // Get the last row index (zero-based)
  int row = model->rowCount() - 1;

  if (row >= 0) {  // Check if there's a row to remove
      model->removeRow(row);
      model->submitAll();
      model->select();
  }
}


void Users::on_addUser_clicked()
{
    // Assuming adding a user involves more operations; if not, this code might not be necessary.
    if (!model->submitAll()) {
        qDebug() << "Error adding user:" << model->lastError().text();
        model->database().rollback();
    } else {
        model->database().commit();
   }
//    QModelIndexList selectedIndexes = ui->tableView->selectionModel()->selectedRows();

//    if (selectedIndexes.isEmpty()) {
//        qDebug() << "No row selected";
//        return;
//      }
//    else{
//    QModelIndex index = selectedIndexes.first();

//    int row = index.row();

//    int id = model->data(model->index(row, 0)).toInt();
//    UserId=id;

//     model->setTable("Device");
//     QString filter = QString("%1 = '%2'").arg("UserID").arg(id);

//     model->setFilter(filter);
     model->setTable("Device");
     model->select();
   // }
}


void Users::on_deleteUser_clicked()
{
  QModelIndexList selectedIndexes = ui->tableView->selectionModel()->selectedRows();

  if (selectedIndexes.isEmpty()) {
      qDebug() << "No row selected";
      return;
    }

  QModelIndex index = selectedIndexes.first();

  int row = index.row();

  int id = model->data(model->index(row, 0)).toInt();

  QSqlQuery query;
   query.prepare("DELETE FROM User WHERE UserID = :UserID");
   query.bindValue(":UserID", id);

   if (!query.exec()) {
       qDebug() << "Error deleting row:" << query.lastError().text();
   } else {
        model->submitAll();
       model->select();
   }

  model->submitAll();

}









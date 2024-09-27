#include "users.h"
#include "ui_users.h"

Users::Users(ApplicationConfiguration &config,QWidget *parent) :
  QDialog(parent),
  ui(new Ui::Users),
  storage(DeviceInfoStorage::getInstanse()),
  appConfig(config)
{
  ui->setupUi(this);
  initializeDatabase();
  ui->downloadProgress->hide();
  ui->SavedLabel->hide();
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
  filePath=storage.getDataBasePath()+"/"+"FS_Service.db";
  db.setDatabaseName(filePath);

  if (!db.open()) {
      qDebug() << "Error: connection with database failed";
    } else {
      qDebug() << "Database: connection ok";
      QSqlQuery query;
      query.exec("PRAGMA foreign_keys = ON;");
    }
  qDebug()<<db.tables();

  // Create and configure the model
  model = new QSqlTableModel(this, db);
  model->setTable("User");
  model->select(); // This is load the data from the table into the model
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
  // ui->tableView->setWindowTitle("QTableView Example");
  ui->tableView->setColumnHidden(0, true);

  // connect(ui->tableView, &QTableView::customContextMenuRequested, this, &MainWindow::showContextMenu);
  return true;
}

//bool Users::saveData(const QString &name, int age) {
//  QSqlQuery query;

//  query.prepare("INSERT INTO User (UserID, Organization, Location, ResponsiblePerson) VALUES (:UserID, :Organization, :Location, :ResponsiblePerson)");

//  query.bindValue(":Organization", name);
//  query.bindValue(":UserID", age);
//  query.bindValue(":Location", name);
//  query.bindValue(":ResponsiblePerson", name);

//  // Debugging: Print the query and bound values
//  qDebug() << "Prepared query:" << query.lastQuery();
//  qDebug() << "Bound values:";
//  for (const auto &key : query.boundValues().keys()) {
//      qDebug() << key << "=" << query.boundValue(key);
//    }
//  if (!query.exec()) {
//      qDebug() << "Error: Could not insert data -" << query.lastError();
//      return false;
//    }

//  return true;
//}

//void Users::restoreData() {
//  QSqlQuery query("SELECT id, name, age FROM people");

//  while (query.next()) {
//      int id = query.value(0).toInt();
//      QString name = query.value(1).toString();
//      int age = query.value(2).toInt();

//      qDebug() << "ID:" << id << "Name:" << name << "Age:" << age;
//    }
//}


void Users::on_plus_clicked()
{
  int row=model->rowCount();
  model->insertRow(row);
}

//delete only last row
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
  //crutch, because Device table has foreign key to user table, whitch i can't or want to delete
  if(model->tableName()=="User"){
      QSqlQuery query;
      query.exec("PRAGMA foreign_keys = OFF;");
    }
  else{
      QSqlQuery query;
      query.exec("PRAGMA foreign_keys = ON;");
    }
  s_n=storage.getSnDevice();
  qDebug()<<storage.getSnDevice();

  //create device data table
  QSqlQuery query;
  bool success = query.exec(QString("CREATE TABLE IF NOT EXISTS DeviceData%1 ("
                                    "DT DATETIME NOT NULL, "
                                    "DataBlockID INTEGER, "
                                    "Tint FLOAT, "
                                    "HInt FLOAT, "
                                    "TExt FLOAT, "
                                    "HExt FLOAT, "
                                    "Ctrl BIT, "
                                    "FOREIGN KEY (DataBlockID) REFERENCES DataBlock(DataBlockID)ON DELETE CASCADE)").arg(s_n));

  if (!success) {
      qDebug() << "Error creating table:" << query.lastError().text();
    } else {
      qDebug() << "Table created successfully";
    }
  //------------

  // Assuming adding a user involves more operations; if not, this code might not be necessary.
  if (!model->submitAll()) {
      qDebug() << "Error adding user:" << model->lastError().text();
      model->database().rollback();
    } else {
      model->database().commit();
      qDebug()<<"database commit successfully";
    }

  QString tableName=model->tableName();

  if(tableName=="User"){
      int id = getSelectedValueFromColum(0);
      appConfig.setCurrentUser(getSelectedStringValue(3),getSelectedStringValue(1),getSelectedStringValue(2));
      appConfig.saveSettings();
      model->setTable("Device");
      QSqlQuery query;
      query.exec("PRAGMA foreign_keys = ON;");
      QString filter = QString("%1 = '%2' AND %3 = '%4'")
          .arg("UserID").arg(id)
          .arg("SerialNumber").arg(s_n);
      model->setFilter(filter);
      model->select();

      // Device table show with filter
      QString queryStr = "SELECT SerialNumber FROM Device WHERE SerialNumber = :SerialNumber AND UserID = :User";
      query.prepare(queryStr);
      query.bindValue(":SerialNumber", s_n);
      query.bindValue(":User", id);

      if (query.exec()) {
          if (query.next()) {
              // The value exists in the table
              qDebug() << "SerialNumber exists. Displaying all matching rows:";
            } else {
              // The value does not exist in the table
              qDebug() << "SerialNumber does not exist. Adding to the table.";
              // Step 2: Add the new value with additional information
              QSqlQuery insertQuery;
              insertQuery.prepare("INSERT INTO Device (DeviceTypeID,UserID,SerialNumber,VerificationDate) VALUES (:DeviceTypeID,:UserID,:SerialNumber,:VerificationDate)");
              insertQuery.bindValue(":DeviceTypeID", storage.getModelDevice());
              insertQuery.bindValue(":UserID", id);
              insertQuery.bindValue(":SerialNumber", s_n);
              insertQuery.bindValue(":VerificationDate", storage.getVerificationDate().toString("yyyy-MM-dd"));

              if (insertQuery.exec()) {
                  qDebug() << "New Device added successfully.";
                  model->select();
                } else {
                  qDebug() << "Failed to add Device:" << insertQuery.lastError().text();
                }
            }
        } else {
          // Handle query execution error
          qDebug() << "Query execution failed:" << query.lastError().text();
        }
      ui->tableView->selectRow(0);
      on_addUser_clicked();
    }
  if(tableName=="Device"){
      //Data block show
      int id = getSelectedValueFromColum(0);
      model->setTable("DataBlock");
      setFilter(QString("%1 = '%2'").arg("DeviceID").arg(id));
      model->select();

      QString queryStr = "SELECT FirstTime FROM DataBlock WHERE FirstTime = :FirstTime AND LastTime = :LastTime AND DeviceID = :DeviceID";
      //  /*QSqlQuery*/ query;
      query.prepare(queryStr);
      query.bindValue(":FirstTime", storage.getFromDateDB().toString("yyyy-MM-dd hh:mm"));
      query.bindValue(":LastTime", storage.getToDateDB().toString("yyyy-MM-dd hh:mm"));
      query.bindValue(":DeviceID", id);

      if (query.exec()) {
          if (query.next()) {
              // The value exists in the table
              qDebug() << "FirstTime exists. Displaying all matching rows:";
            } else {
              // The value does not exist in the table
              qDebug() << "FirstTime does not exist. Adding to the table.";

              // Step 2: Add the new value with additional information
              QSqlQuery insertQuery;
              insertQuery.prepare("INSERT INTO DataBlock (DeviceID,FirstTime,LastTime,DataBlockType,IntervalInMin) VALUES (:DeviceID,:FirstTime,:LastTime, :DataBlockType, :IntervalInMin)");
              insertQuery.bindValue(":DeviceID", id);
              insertQuery.bindValue(":FirstTime", storage.getFromDateDB().toString("yyyy-MM-dd hh:mm"));
              insertQuery.bindValue(":LastTime", storage.getToDateDB().toString("yyyy-MM-dd hh:mm"));
              insertQuery.bindValue(":DataBlockType", storage.getModelDevice());
              insertQuery.bindValue(":IntervalInMin", 1);

              if (insertQuery.exec()) {
                  qDebug() << "New DataBlock added successfully.";
                  model->select();
                } else {
                  qDebug() << "Failed to add DataBlock:" << insertQuery.lastError().text();
                }
            }
        } else {
          // Handle query execution error
          qDebug() << "Query execution failed:" << query.lastError().text();
        }
      ui->tableView->selectRow(0);
      on_addUser_clicked();
    }
  if(tableName=="DataBlock"){
      //Data block show
      int id = getSelectedValueFromColum(0);
      int devId=getSelectedValueFromColum(1);
      ExportDataFromBytes exp;
      exp.ExportServiceAndDataPoints();
      bool ctrl=exp.getRangeControlStatus();
      auto range211=storage.getRangeFor211();
      QString devName=storage.getModelDevice();
      //add range control settings in to DeviceControl table
      QSqlQuery Query;
      Query.prepare("INSERT INTO DeviceControl (DT, DataBlockID, DeviceID, TIntCheck, HIntCheck, TExtCheck, HExtCheck, TIntMin, TIntMax, HIntMin, HIntMax, TExtMin, TExtMax, HExtMin, HExtMax) "
                    "VALUES (:DT, :DataBlockID, :DeviceID, :TIntCheck, :HIntCheck, :TExtCheck, :HExtCheck, :TIntMin, :TIntMax, :HIntMin, :HIntMax, :TExtMin, :TExtMax, :HExtMin, :HExtMax)");

      Query.bindValue(":DT",exp.getStartEndDate().first.toString("yyyy-MM-dd hh:mm"));
      Query.bindValue(":DataBlockID", id);
      Query.bindValue(":DeviceID",devId);
      Query.bindValue(":TIntCheck",ctrl);
      Query.bindValue(":HIntCheck",ctrl);
      Query.bindValue(":TExtCheck",ctrl);//temp set inner sensor data
      Query.bindValue(":HExtCheck",ctrl);//temp set inner sensor data
      if(devName=="101"){
          Query.bindValue(":TIntMin",static_cast<int>(exp.getTempRange().first));
          Query.bindValue(":TIntMax",static_cast<int>(exp.getTempRange().second));
          Query.bindValue(":HIntMin",static_cast<int>(exp.getHumidRange().first));
          Query.bindValue(":HIntMax",static_cast<int>(exp.getHumidRange().second));
          Query.bindValue(":TExtMin",0);
          Query.bindValue(":TExtMax",0);
          Query.bindValue(":HExtMin",0);
          Query.bindValue(":HExtMax",0);
        }else if(devName=="211"){
          Query.bindValue(":TIntMin",static_cast<int>(std::get<0>(range211)));
          Query.bindValue(":TIntMax",static_cast<int>(std::get<1>(range211)));
          Query.bindValue(":HIntMin",static_cast<int>(std::get<2>(range211)));
          Query.bindValue(":HIntMax",static_cast<int>(std::get<3>(range211)));
          Query.bindValue(":TExtMin",static_cast<int>(std::get<4>(range211)));
          Query.bindValue(":TExtMax",static_cast<int>(std::get<5>(range211)));
          Query.bindValue(":HExtMin",static_cast<int>(std::get<6>(range211)));
          Query.bindValue(":HExtMax",static_cast<int>(std::get<7>(range211)));
        }


      if(!Query.exec()){
          qDebug()<<"Device control data not write, "<<Query.lastError();
        }
      else{
          qDebug()<<"Device control data write!!!!!!";
        }
      //---------------

      model->setTable(QString("DeviceData%1").arg(s_n));
      setFilter(QString("%1 = '%2'").arg("DataBlockID").arg(id));
      model->select();

      QString queryStr = QString("SELECT DataBlockID FROM DeviceData%1 WHERE DataBlockID = :DataBlockID ").arg(s_n);
      QSqlQuery query;
      query.prepare(queryStr);
      query.bindValue(":DataBlockID", id);
      if (query.exec()) {
          if (query.next()) {
              // The value exists in the table
              qDebug() << "DataBlockID exists. Displaying all matching rows:";

            } else {
              // The value does not exist in the table
              qDebug() << "DataBlockID does not exist. Adding to the table.";

              // Step 2: Add the new value with additional information
              QVector<ExportDataFromBytes::Data> values=exp.getArrayValues();
              QSqlQuery insertQuery;
              insertQuery.prepare(QString("INSERT INTO DeviceData%1 (DT,DataBlockID,Tint,HInt,TExt,HExt,Ctrl) VALUES (:DT,:DataBlockID,:Tint, :HInt, :TExt,:HExt,:Ctrl)").arg(s_n));
              db.transaction();

              ui->downloadProgress->setMaximum(values.size()-1);
              ui->downloadProgress->show();
              int downloadValue=0;

              for(const auto& it:values){
                  ui->downloadProgress->setValue(downloadValue);
                  insertQuery.bindValue(":DT",it.date.toString("yyyy-MM-dd hh:mm"));
                  insertQuery.bindValue(":DataBlockID",id );
                  insertQuery.bindValue(":Tint", it.values[2]);
                  insertQuery.bindValue(":HInt", it.values[3]);
                  insertQuery.bindValue(":TExt", it.values[0]);
                  insertQuery.bindValue(":HExt", it.values[1]);
                  insertQuery.bindValue(":Ctrl", ctrl);

                  if (insertQuery.exec()) {
                      // qDebug() << "sentance added successfully.";

                    } else {
                      //  qDebug() << "Failed to add user:" << insertQuery.lastError().text();
                    }
                  model->select();
                  ++downloadValue;

                }
            }
        } else {
          // Handle query execution error
          qDebug() << "Query execution failed:" << query.lastError().text();
        }
    }
  if(tableName==QString("DeviceData%1").arg(s_n)){
       ui->SavedLabel->setText("Сохранено");
     ui->SavedLabel->show();
    }
}


void Users::on_deleteUser_clicked()
{
  int row = getSelectedIndex().row();

  // int id = model->data(model->index(row, 0)).toInt();
  QString firstColumnName = model->headerData(0, Qt::Horizontal).toString();
  // Get the data from the first column of the selected row
  QVariant idVariant = model->data(model->index(row, 0));

  // Convert to int, considering whether the data is text or already an int
  bool ok = false;
  int id = idVariant.toInt(&ok);

  if (!ok) {
      qDebug() << "Conversion to int failed for value:" << idVariant;

    }

  //crutch, because Device table has foreign key to user table, whitch i can't or want to delete
  if(model->tableName()=="User"){
      QSqlQuery query;
      query.exec("PRAGMA foreign_keys = OFF;");
    }
  else{
      QSqlQuery query;
      query.exec("PRAGMA foreign_keys = ON;");
    }

  // Prepare the deletion query
  QSqlQuery query;
  QString queryString = QString("DELETE FROM %1 WHERE %2 = :value")
      .arg(model->tableName(),firstColumnName);
  query.prepare(queryString);
  query.bindValue(":value", id);

  if (!query.exec()) {
      qDebug() << "Error deleting row:" << query.lastError().text();

    } else {
      model->submitAll();
      model->select();
    }

  model->submitAll();

}


void Users::deleteTable(QString tableName){
  QSqlQuery query;

  // Construct the SQL query to drop the table
  QString sqlQuery = QString("DROP TABLE IF EXISTS %1").arg(tableName);

  // Execute the query
  if (!query.exec(sqlQuery)) {
      qDebug() << "Error deleting table:" << query.lastError().text();

    }

  qDebug() << "Table" << tableName << "deleted successfully.";
}

void Users::setFilter(QString filter){
  model->setFilter(filter);
};

QModelIndex Users::getSelectedIndex(){
  QModelIndexList selectedIndexes = ui->tableView->selectionModel()->selectedRows();
  QModelIndex index;
  if (selectedIndexes.isEmpty()) {
      qDebug() << "No row selected";
    }
  else{
      index  = selectedIndexes.first();
    }

  return index;
};



void Users::on_upButton_clicked()
{
  ui->downloadProgress->hide();
  ui->SavedLabel->hide();
  model->setTable("User");
  model->select();
  ui->tableView->setColumnHidden(0, true);
}

int Users::getSelectedValueFromColum(int columNumber){
  QModelIndexList selectedIndexes = ui->tableView->selectionModel()->selectedRows();
  QModelIndex index;
  if (selectedIndexes.isEmpty()) {
      qDebug() << "No row selected";
    }
  else{
      index  = selectedIndexes.first();
      int row = index.row();
      int id = model->data(model->index(row, columNumber)).toInt();
      return id;
    }
  return -1;
};

QString Users::getSelectedStringValue(int columN){
  QModelIndexList selectedIndexes = ui->tableView->selectionModel()->selectedRows();
  QModelIndex index;
  if (selectedIndexes.isEmpty()) {
      qDebug() << "No row selected";
    }
  else{
      index  = selectedIndexes.first();
      int row = index.row();
      QString id = model->data(model->index(row, columN)).toString();
      return id;
    }
  return "";
};

void Users::on_update_clicked()
{
  model->submitAll();
  model->select();
}




void Users::on_buttonBox_accepted()
{
  model->submitAll();
  model->database().commit();
}


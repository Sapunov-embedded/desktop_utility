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
    delete ui;
}

bool Users::initializeDatabase() {
  // Set up the database connection
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
    //db.setDatabaseName("example.db");
  db.setDatabaseName("FS_Service.db");


    if (!db.open()) {
        qDebug() << "Error: connection with database failed";
    } else {
        qDebug() << "Database: connection ok";
    }

    // Create and configure the model
    QSqlTableModel *model = new QSqlTableModel(this, db);
    model->setTable("User");
    model->select(); // This will load the data from the table into the model

    // Debugging output
        qDebug() << "Rows:" << model->rowCount();
        qDebug() << "Columns:" << model->columnCount();
        qDebug()<<db.tables();
      //  qDebug()<

    // Optionally, set headers for the columns
//    model->setHeaderData(0, Qt::Horizontal, QObject::tr("Column 1"));
//    model->setHeaderData(1, Qt::Horizontal, QObject::tr("Column 2"));
//    model->setHeaderData(2, Qt::Horizontal, QObject::tr("Column 3"));
    model->removeColumn(0);
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
    return true;
}

bool Users::saveData(const QString &name, int age) {
    QSqlQuery query;
    query.prepare("INSERT INTO people (name, age) VALUES (:name, :age)");
    query.bindValue(":name", name);
    query.bindValue(":age", age);

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

//int main(int argc, char *argv[]) {
//    QCoreApplication a(argc, argv);

//    if (!initializeDatabase()) {
//        return -1;
//    }

//    // Save some data
//    saveData("Alice", 30);
//    saveData("Bob", 25);

//    // Restore and print the data
//    restoreData();

//    return a.exec();
//}

void Users::on_plus_clicked()
{
    saveData("lol",29);
}




void Users::on_minus_clicked()
{
    restoreData();
}


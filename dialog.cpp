#include "dialog.h"
#include "ui_dialog.h"

Dialog::Dialog(QWidget *parent) :
    QDialog(parent),
    ui_2(new Ui::Dialog)
{
    ui_2->setupUi(this);
}

Dialog::~Dialog()
{
    delete ui_2;
}

#include "grapthics.h"
#include "ui_grapthics.h"

grapthics::grapthics(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::grapthics)
{
    ui->setupUi(this);
}

grapthics::~grapthics()
{
    delete ui;
}




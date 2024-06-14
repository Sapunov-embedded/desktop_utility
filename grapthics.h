#ifndef GRAPTHICS_H
#define GRAPTHICS_H

#include <QWidget>

namespace Ui {
class grapthics;
}

class grapthics : public QWidget
{
    Q_OBJECT

public:
    explicit grapthics(QWidget *parent = nullptr);
    ~grapthics();

private slots:


private:
    Ui::grapthics *ui;
};

#endif // GRAPTHICS_H

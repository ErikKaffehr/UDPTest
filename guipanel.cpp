#include "guipanel.h"
#include "datastorage.h"
#include <QLCDNumber>
#include <QTimer>

GUIPanel::GUIPanel(QWidget *parent)
    : QMainWindow{parent}
{
    _timer = new QTimer(this);
    connect(_timer, SIGNAL(timeout()), this, SLOT(scanData()));
    _timer->setInterval(1000);
    _timer->start();
    lcd1 = new QLCDNumber(this);
    lcd1->resize(300, 100);
    lcd1->show();

    lcd2 = new QLCDNumber(this);
    lcd2->resize(300, 100);
    lcd2->move(0, 300);
    lcd2->show();

    resize(800, 800);
}

void GUIPanel::scanData()
{
    float value = DataStorage::getInstance()->getValueByName("TDer");
    lcd1->display(value);
    value = DataStorage::getInstance()->getValueByName("T4");
    lcd2->display(value);
}

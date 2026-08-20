#include "guipanel.h"
#include "datastorage.h"
#include <QLCDNumber>
#include <QPushButton>
#include <QTimer>

GUIPanel::GUIPanel(QWidget *parent)
    : QWidget(parent)
{
    _timer = new QTimer(this);
    connect(_timer, SIGNAL(timeout()), this, SLOT(scanData()));
    _timer->setInterval(1000);
    _timer->start();
    _layout = new QVBoxLayout(this);
    QWidget *dummy = new QWidget(this);
    dummy->setFixedSize(2,2);
    dummy->show();
    _layout->addWidget(dummy);
    _layout->addStretch(10);
#if 0
    lcd1 = new QLCDNumber(this);
    lcd1->resize(300, 100);
    lcd1->show();

    lcd2 = new QLCDNumber(this);
    lcd2->resize(300, 100);
    lcd2->move(0, 300);
    lcd2->show();
#endif

    resize(800, 800);
}

void GUIPanel::scanData()
{
    const char* c_name;
    DisplayItem *pdi;
    VariableNames vnames = DataStorage::getInstance()->getVariableNames();
    for (ulong i = 0; i < vnames.size(); i++) {
        c_name = vnames[i].c_str();
        DisplayItems::iterator pit;
        if (_display_items.find(c_name) == _display_items.end()) {
            pdi = new DisplayItem(this,  c_name);
            _display_items[c_name] = pdi;
            _layout->insertWidget(_layout->count() - 1, pdi);
            //pdi->move(0, i*60);
            pdi->show();
        }
        _display_items[vnames[i].c_str()]->update();
    }


#if 0
    float value = DataStorage::getInstance()->getValueByName("TDer");
    lcd1->display(value);
    value = DataStorage::getInstance()->getValueByName("T4");
    lcd2->display(value);
#endif
}

DisplayItem::DisplayItem(QWidget *pw, const char* item):QWidget(pw)
{
    QFont my_font("Times", 30);
    _layout = new QHBoxLayout(this);
    _name = new QLabel(this);
    _name->setText(item);
    _name->setFont(my_font);
    _text = new QLabel(this);
    _text->setText("no value");
    _text->setFont(my_font);
    setFixedSize(300, 60);
    _layout->addWidget(_name);
    _layout->addWidget(_text);
}

void DisplayItem::update()
{
    float v = DataStorage::getInstance()->getValueByName(_name->text().toLatin1().data());
    _text->setNum(v);
}

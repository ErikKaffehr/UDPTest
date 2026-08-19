#include "Plotter.h"
#include "myudp.h"
#include <QSize>
#include <QPainter>
#include <QPen>
#include <QLayout>
#include <QGridLayout>
#include <QPoint>
#include <QVector>
#include <QDialogButtonBox>
#include <QFileDialog>


//#include <QVBoxLayout>
#include "datastorage.h"

Axis::Axis(Plotter *parent, float min_val, float max_val, const char* color,float divisor = 1):QWidget(parent)
{
    _min = min_val;
    _max = max_val;
    _steps = 10;
    _divisor = divisor;
    _axis_dialogue = new AxisDialogue(this);
    _parent = parent;

    // connect(this, SIGNAL(clicked()), _axis_dialogue, SLOT(activate()));
}

int Axis::remap(double value ) {
    int max_pos;
    bool is_vertical = 0;
    QSize my_size = size();
    if (my_size.height() > my_size.width()) {
        max_pos = my_size.height() - 1;
        is_vertical = true;
    } else
        max_pos = my_size.width();

    float Dy_Dx = max_pos / (_max - _min);
    float fpos = Dy_Dx * (value - _min);
    if (is_vertical)
        fpos = max_pos -fpos;
    return round(fpos);
}


VerticalAxis::VerticalAxis(Plotter* parent, float min, float max, const char* color, float divisor)
    :Axis(parent, min, max,color,  divisor)
{
    setFixedWidth(70);
    _my_color = QColor(color);
    _axis_dialogue = new AxisDialogue(this);
}

void Axis::acceptClicked()
{
    _axis_dialogue->exec();
}

void Axis::mousePressEvent(QMouseEvent *)
{
    _parent->pauseUpdates(true);
    acceptClicked();
    _parent->pauseUpdates(false);
}

void VerticalAxis::mousePressEvent(QMouseEvent*)
{
    _parent->pauseUpdates(true);
    invoke();
    acceptClicked();
    _parent->pauseUpdates(false);
}
const char* VerticalAxis::variableName()
{
    return _attached_variable.c_str();
}

void VerticalAxis::paintEvent(QPaintEvent*)
{
    QPainter p(this);
    QSize my_size = size();

    int xpos = my_size.width() - 1;

    //p.begin();
    p.setPen(_my_color);
    p.drawLine(xpos, 0, xpos, my_size.height());
    float v;
    float y;
    char label[20];
    for (int i = 0; i <= _steps; i++) {
        v = _min + i * (_max - _min) / _steps;
        y = remap(v);
        p.drawLine(xpos - 10, y, xpos, y);
        sprintf(label, "%0.2f", v);
        p.drawText(QPointF(30, y +5), label);
    }
    int y_pos = my_size.height() /2;

    p.save();
    p.translate(10, y_pos);
    p.rotate(-90);
    p.drawText(0,0, _attached_variable.c_str());
    p.restore();
    //_parent->repaint();
}

void VerticalAxis::setVariable(std::string var_name)
{
    _attached_variable = var_name;
}

void VerticalAxis::invoke()
{
    _axis_dialogue->invoke();
    //_axis_dialogue->exec();
}
void HorizontalAxis::paintEvent(QPaintEvent*)
{
    QPainter p(this);
    QSize my_size = size();

    _time_zero = DataStorage::getInstance()->baseTime();
    int ypos = my_size.height() - 100;

    //p.begin();
    p.setPen("blue");

    p.drawLine(0, ypos, my_size.width(), ypos);
    float v;
    float x;
    //float y = my_size.height() - 0;
    //char label[20];
    for (int i = 0; i <= _steps; i++) {
        v = _min + i * (_max - _min) / _steps;
        v += _time_zero;
        x = remap(v);
        p.drawLine(x, ypos, x, ypos +10);
        time_t t = v;
        struct tm tm = *localtime(&t);              // UTC broken-down time
        char buf[32];
        strftime(buf, sizeof buf, "%d/%m %H:%M", &tm);
        printf("%s\n", buf);
        //sprintf(label, "%0.2f", v / _divisor);

        p.drawText(QPointF(x, ypos + 20), buf);
    }
   // _parent->repaint();
}

HorizontalAxis::HorizontalAxis(Plotter* parent, float min, float max, const char *color, float divisor)
    :Axis(parent, min, max, "black", divisor)
{
    //setFixedWidth(600);
    setFixedHeight(100);
    _step = (max - min) / 10;
    _shifted= 0;
}

bool HorizontalAxis::shiftLimits(unsigned long t) {
    t -= _time_zero;
    if ((_shifted >= 0) && (t > _max)) {
        _min += _step;
        _max += _step;
        repaint();
        return true;
    }
    return false;
}

void HorizontalAxis::setBaseTime(time_t time_zero)
{
    _time_zero = time_zero;
}

int HorizontalAxis:: remap(double value)
{
    value -= _time_zero;

    QSize my_size = size();

    int max_pos = my_size.width();

    float Dy_Dx = max_pos / (_max - _min);
    float fpos = Dy_Dx * (value - _min);
    return round(fpos);
}
void HorizontalAxis::forwardWindow()
{
    _min += _step;
    _max += _step;
    if (_shifted++ > 0)
        _shifted = 0;
    repaint();
}

void HorizontalAxis::backwardWindow()
{
    _min -= _step;
    _max -= _step;
    _shifted--;
    repaint();
}

PlottingArea::PlottingArea(QWidget *parent):QWidget(parent)
{

}

void PlottingArea::paintEvent(QPaintEvent*) {
    QPainter painter(this);

    std::string colors[] = { "black", "red", "green", "blue", "orange", "violet", "brown"};
    for (int i = 0; i < 7; i++) {
        QPen pen(colors[i].c_str());
        pen.setWidth(3);
        painter.setPen(pen);
        painter.drawPolyline(_polygons[i]);
    }
}

ControlPanel::ControlPanel(Plotter* parent)
{
    _forwardButton = new QPushButton(">>", this);
    _backwardButton = new QPushButton("<<", this);
    _screendumpButton = new QPushButton("Screendump", this);
    _thermostatOn = new QPushButton("Thermostat Passive", this);
    _thermostatOn->setCheckable(true);
    _thermostatMin = new QLineEdit("20", this);
    _thermostatMax = new QLineEdit("28", this);
    _tl = new QGridLayout(this);

    _tl->addWidget(_backwardButton, 0, 0);
    _tl->addWidget(_forwardButton, 0, 1);
    _tl->addWidget(_screendumpButton, 1, 0);
    _tl->addWidget(_thermostatOn, 2, 0);
    _tl->addWidget(_thermostatMin, 2, 1);
    _tl->addWidget(_thermostatMax, 2, 2);
    //_tl->addStretch(10, 2,3);

    connect(_forwardButton, SIGNAL(clicked()), parent->horizontalAxis(), SLOT(forwardWindow()));
    connect(_backwardButton, SIGNAL(clicked()), parent->horizontalAxis(), SLOT(backwardWindow()));
    connect(_screendumpButton, SIGNAL(clicked()), parent, SLOT(dumpPixmap()));
    connect(_thermostatOn, SIGNAL(clicked()), this, SLOT(handleThermostat()));
    connect(_thermostatMin, SIGNAL(changed()), this, SLOT(handleThermostat()));
}

void ControlPanel::handleThermostat()
{
    QString cmd = "CMD THERMOSTAT ";

    if (_thermostatOn->isChecked()) {
        _thermostatOn->setText("Thermostat Active");
        cmd.append("ON ");
        cmd.append(_thermostatMin->text());
        cmd.append(" ");
        cmd.append(_thermostatMax->text());
    } else {
        _thermostatOn->setText("Thermostat Passive");
        cmd.append("OFF ");
    }
    qDebug() << cmd;
    MyUDP::getInstance()->broadcastCmd(cmd);
}

Plotter::Plotter(QWidget *parent)
    : QWidget{parent}
{
    const char *variables[] = { "TFloor", "TDer", "HS", "TimeL", "T3", "T5", "Voltage" };
    float min_scale[] = { 5, -10, 0, 0, 20, 20, 11};
    float max_scale[] = { 30, 10, 8, 1800, 30, 30, 15};

 ;
    const char *colors[]    = { "black","red", "green", "blue", "orange", "violet", "brown"};
    QGridLayout *tl = new QGridLayout(this);

    _panel = new PlottingArea(this);
    _panel->show();

    const int num_of_vars = 7;
    for (int i = 0; i < num_of_vars; i++) {
        VerticalAxis *pax = new VerticalAxis(this, min_scale[i], max_scale[i],colors[i], 1);
        pax->setVariable(variables[i]);
        _axes.push_back(pax);
        pax->show();
       // pax->repaint();
        tl->addWidget(pax, 1, i);
    }
    tl->addWidget(_panel, 1 , num_of_vars);
    _time_axis = new HorizontalAxis(this, 0, 8 * 3600, "black", 3600);

    tl->addWidget(_time_axis, 2, num_of_vars);
    _time_axis->show();

    _control_panel = new ControlPanel(this);
    tl->addWidget(_control_panel, 2, 0, 1, num_of_vars);
    _control_panel->show();

    connect(&_timer, SIGNAL(timeout()), this, SLOT(timeout()));

    _timer.setInterval(2000);
    _timer.start();
}

void Plotter::timeout()
{
    refresh();
    if (_refresh_counter++ > 60) {
        _control_panel->handleThermostat();
        _refresh_counter =0;
    }
}

//void Plotter::paintEvent(QPaintEvent *p)
void Plotter::refresh()
{
    _time_axis->setBaseTime(DataStorage::getInstance()->baseTime());
    int num_of_axes = _axes.size();
    for (int i = 0; i < num_of_axes; i++) {
        const char* pvarname = _axes[i]->variableName();
        DataPointVector *pdpv
            = DataStorage::getInstance()->getDataPointVectorByName(pvarname);

        if (pdpv) {
            int length = pdpv->size();
            //QVector< QPoint > polygon(length);
            Polygon* polygon = _panel->getPolygon(i);
            polygon->resize(length);
            DataPointVector::iterator it;
            DataPointVector::iterator last = pdpv->end() -1;
            DataPoint last_dp = (*last);
            int64_t last_time = last_dp.time();
            _time_axis->shiftLimits(last_time);
            int pos = 0;
            for (it = pdpv->begin(); it < pdpv->end(); it++) {
                float v;
                int64_t t;

                DataPoint pdp = (*it);

                v = pdp.value();

                t = pdp.time();
                //_time_axis->shiftLimits(t);
                QPoint p((int) _time_axis->remap(t), (int) _axes[i]->remap(v));
                (*polygon)[pos++] = p;
            }

        }
    }
    _panel->update();
}

void Plotter::pauseUpdates(bool flag)
{
    if (flag)
        _timer.stop();
    else
        _timer.start();
}

void Plotter::dumpPixmap()
{
    QPixmap pixmap(this->size());
    this->render(&pixmap);
    QString save_name = QFileDialog::getSaveFileName(this, "Save File");
    pixmap.save(save_name, "png");

}

AxisDialogue::AxisDialogue(Axis *parent):QDialog(parent)
{
    _min = new QLineEdit(this);
    _max = new QLineEdit(this);
    _steps = new QLineEdit(this);
    _parent = parent;

    float min, max;
    int steps;
    char buffer[64];
    parent->getMinMax(min, max, steps);
    sprintf(buffer, "%g", min);
    _min->setText(buffer);
    sprintf(buffer, "%g", max);
    _max->setText(buffer);
    sprintf(buffer, "%i", steps);
    _steps->setText(buffer);
    _tl = new QVBoxLayout(this);
    _tl->addWidget(_max);
    _tl->addWidget(_min);
    _tl->addWidget(_steps);

    _variables = new QComboBox(this);
    _tl->addWidget(_variables);



    QDialogButtonBox *buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok
                                     | QDialogButtonBox::Cancel);

    connect(buttonBox, SIGNAL(accepted()), this, SLOT(apply()));
    connect(buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);
    _tl->addWidget(buttonBox);

    // VariableNames variables = DataStorage::getInstance()->getVariableNames();
    // for (int i = 0; i <variables.size(); i++)
    //   _variables->insertItem(i, variables[i].c_str());
    setModal(true);
    hide();
}

void AxisDialogue::invoke()
{
    _variables->clear();
    VariableNames variables = DataStorage::getInstance()->getVariableNames();
    _variables->insertItem(0,"");
    for (uint i = 0; i <variables.size(); i++)
        _variables->insertItem(i+1, variables[i].c_str());
    //exec();
}

void AxisDialogue::apply()
{
    min = _min->text().toFloat();
    max = _max->text().toFloat();
    steps = _steps->text().toInt();
    _parent->setMinMax(min, max, steps);


    //VerticalAxis *p = (VerticalAxis*) _parent;
    if (_variables->currentText() != QString("")) {
       // p->setVariable(_variables->currentText().toStdString());
     _parent->setVariable(_variables->currentText().toStdString());
    }
    _parent->repaint();
    //hide();
    accept();
}

void Axis::getMinMax(float &min, float &max, int &steps)
{
    min = _min / _divisor;
    max = _max / _divisor;
    steps = _steps;
}

void Axis::setMinMax(float min, float max, int steps)
{
    _min = min * _divisor;
    _max = max * _divisor;
    _steps = steps;
    _step = (_max - _min) / 10;
}

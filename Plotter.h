#ifndef Plotter_H
#define Plotter_H

#include <QtTypes>
#include <QObject>
#include <QWidget>
#include <QMainWindow>
#include <QPaintEvent>
#include <QTimer>
#include <QVector>
#include <QPoint>
#include <QGridLayout>
#include <QVBoxLayout>
#include <QPushButton>
#include <QLineEdit>
#include <QDialog>
#include <QColor>
#include <QString>
#include <QComboBox>


#include <vector>

typedef QVector<QPoint> Polygon;
class ControlPanel;
class AxisDialogue;

class Plotter;

class Axis: public QWidget {
    Q_OBJECT
public:
    Axis(Plotter *parent, float min_val, float max_val, const char* color, float divisor);
    virtual int remap(double value);
    void setMinMax(float min, float max, int steps);
    void getMinMax(float & min, float& max, int &steps);
    void paintEvent(QPaintEvent*){}
    void connectDataSource(const char* name);
    void invoke();
    virtual void setVariable(std::string){};

private:

protected:
  AxisDialogue *_axis_dialogue;
  Plotter *_parent;

  float _min; // min value
  float _max; // max value
  float _divisor; //convert seconds to hours/mintues
  int  _steps;
  int _step;
  QColor _my_color;
  //QString _my_variable;

  void mousePressEvent(QMouseEvent *event);
  public slots:

  void acceptClicked();
};

class VerticalAxis: public Axis {
public:
  VerticalAxis(Plotter* parent, float min_val, float max_val, const char* color, float divisor = 1);
    void paintEvent(QPaintEvent*);
    void invoke();
    void refresh();
    //void setVariable(std::string varname);
    virtual void setVariable(std::string varname);
    void mousePressEvent(QMouseEvent *event);
    const char* variableName();
private:

    std::string _attached_variable;
};

class HorizontalAxis: public Axis {
    Q_OBJECT
public:
    HorizontalAxis(Plotter* parent, float min_val, float max_val, const char* color, float divisor = 3600);
    int remap(double value);
    bool shiftLimits(unsigned long t);
    void setBaseTime(time_t time_zero);
    void paintEvent(QPaintEvent*);
private:
    //float _step; // step shifting axis
    int _shifted;
    time_t _time_zero;
public slots:
    void forwardWindow();
    void backwardWindow();
};

// This area contains the graphs
class PlottingArea: public QWidget {
public:
    PlottingArea(QWidget *parent);
    Polygon *getPolygon(int num) { return &_polygons[ num ]; }
    void paintEvent(QPaintEvent*); // The paint event draws the curves

private:
    Polygon _polygons[7];

};


// A vector holding all vertical axes
typedef std::vector< VerticalAxis* > Axes;

// this holds things together
class Plotter : public QWidget
{
    Q_OBJECT
public:
    explicit Plotter(QWidget *parent = nullptr);
    void refresh(); // gets new variables and sends data to PlottingArea to plot
    void pauseUpdates(bool);
    HorizontalAxis *horizontalAxis(){return _time_axis; }; // the horizonatl axis
private:
    PlottingArea *_panel;
    HorizontalAxis *_time_axis;
    ControlPanel *_control_panel;
    Axes _axes;
    QTimer _timer;
    int _refresh_counter = 0;


private slots:
    void timeout();
    void dumpPixmap();
signals:
};

class ControlPanel: public QWidget
{
    Q_OBJECT
public:
    ControlPanel(Plotter *parent = 0);

private:

    QPushButton *_forwardButton;
    QPushButton *_backwardButton;
    QPushButton *_screendumpButton;
    QPushButton *_thermostatOn;
    QLineEdit *_thermostatMin;
    QLineEdit *_thermostatMax;
    QGridLayout *_tl;
public slots:
    void handleThermostat();

};

class AxisDialogue: public QDialog {
    Q_OBJECT
public:
    AxisDialogue(Axis *parent);
    QLineEdit *_min;
    QLineEdit *_max;
    QLineEdit *_steps;
    QComboBox *_variables;
    Axis *_parent;
    void invoke();
public slots:
    void apply();
    //
    //void activate();

private:
    float min, max, steps;
    QVBoxLayout *_tl;
};

#endif // Plotter_H

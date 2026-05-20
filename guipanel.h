#ifndef GUIPANEL_H
#define GUIPANEL_H

#include <QMainWindow>
#include <QObject>
#include <QWidget>
#include <QLCDNumber>
#include <QTimer>


class GUIPanel : public QMainWindow
{
    Q_OBJECT
public:
    explicit GUIPanel(QWidget *parent = nullptr);
public slots:
    void scanData();
private:
    QLCDNumber *lcd1;
    QLCDNumber *lcd2;
    QTimer *_timer;
signals:
};

#endif // GUIPANEL_H

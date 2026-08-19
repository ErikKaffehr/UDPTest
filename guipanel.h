#ifndef GUIPANEL_H
#define GUIPANEL_H

#include <QMainWindow>
#include <QObject>
#include <QWidget>
#include <QLCDNumber>
#include <QLabel>
#include <QTimer>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <map>

class DisplayItem;

typedef std::map<std::string, DisplayItem *> DisplayItems;

class DisplayItem : public QWidget
{
    Q_OBJECT
public:
    DisplayItem(QWidget *pw, const char *item);
    void update();
private:
    QHBoxLayout *_layout;
    QLabel *_name;
    QLabel *_text;

};

class GUIPanel : public QMainWindow
{
    Q_OBJECT
public:
    explicit GUIPanel(QWidget *parent = nullptr);
    void update();
public slots:
    void scanData();
private:
    QLCDNumber *lcd1;
    QLCDNumber *lcd2;
    QTimer *_timer;
    DisplayItems _display_items;
    QVBoxLayout *_layout;
signals:
};

#endif // GUIPANEL_H

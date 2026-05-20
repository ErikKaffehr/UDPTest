#ifndef LAUNCHER_H
#define LAUNCHER_H

#include <QMainWindow>
#include <QMenu>
#include <QMenuBar>
#include <QAction>

class Launcher : public QMainWindow
{
    Q_OBJECT
public:
    explicit Launcher(QWidget *parent = nullptr);

signals:
private:
    QMenuBar* _my_menu_bar;

public slots:
    void launchPlotter();

};

#endif // LAUNCHER_H

#include "launcher.h"
#include <Plotter.h>
#include "datastorage.h"

#include <QAction>

Launcher::Launcher(QWidget *parent)
    : QMainWindow{parent}
{
    _my_menu_bar = menuBar();
    QMenu *file_menu = new QMenu("File");

    _my_menu_bar->addMenu(file_menu);

    QAction *launchPlotterAction = new QAction("Launch Plotter", this);
    file_menu->addAction(launchPlotterAction);
    QAction *saveFileAction = new QAction("Save File", this);
    file_menu->addAction(saveFileAction);
    QAction *restoreFileAction = new QAction("Restore File", this);
    file_menu->addAction(restoreFileAction);
    connect(launchPlotterAction, SIGNAL(triggered()), this, SLOT(launchPlotter()));
    connect(saveFileAction, SIGNAL(triggered()), DataStorage::getInstance(), SLOT(store()));
    connect(restoreFileAction, SIGNAL(triggered()), DataStorage::getInstance(), SLOT(restore()));
    _my_menu_bar->show();
}

void Launcher::launchPlotter() {
    Plotter *p = new Plotter(0);
    p->show();
}

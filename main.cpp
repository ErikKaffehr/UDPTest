#include <QApplication>
//#include <QNetworkProxyFactory>
#include "myudp.h"
#include <stdio.h>
#include "guipanel.h"
#include "Plotter.h"
#include <QDialog>
#include <launcher.h>


#include "myudp.h"

int main(int argc, char *argv[])
{
    //QNetworkProxyFactory::setUseSystemConfiguration(false);
    //QNetworkProxy::setApplicationProxy(QNetworkProxy::NoProxy);
    QApplication a(argc, argv);

    GUIPanel gui_panel;
    Plotter plotter(0);
    Launcher launcher;

    launcher.show();

    gui_panel.show();
    plotter.show();
    plotter.resize(800, 800);

    MyUDP *client = MyUDP::getInstance();

    client->HelloUDP();



    return a.exec();
}

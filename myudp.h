// myudp.h

#ifndef MYUDP_H
#define MYUDP_H

#include <QObject>
#include <QUdpSocket>
#include <QString>
#include <QTimer>


class MyUDP : public QObject
{
    Q_OBJECT
public:
    static MyUDP *getInstance();
    void HelloUDP();
signals:

public slots:
    void readyRead();
    void checkStatus();
    void broadcastCmd(QString & cmd);

private:
    explicit MyUDP(QObject *parent = 0);
    QUdpSocket *udp_socket;
    QTimer _my_timer;
    unsigned long msg_count = 0;

    static MyUDP *_my_instance;

};

#endif // MYUDP_H


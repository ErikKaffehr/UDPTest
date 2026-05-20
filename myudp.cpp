// myudp.cpp

#include <unistd.h>
#include "myudp.h"
#include <QHostAddress>
#include <QDebug>
#include <QTimer>
#include <string>

#include "datastorage.h"



MyUDP::MyUDP(QObject *parent) :
    QObject(parent)
{
    // create a QUDP socket
    udp_socket = new QUdpSocket(0);

    // The most common way to use QUdpSocket class is
    // to bind to an address and port using bind()
    // bool QAbstractSocket::bind(const QHostAddress & address,
    //     quint16 port = 0, BindMode mode = DefaultForPlatform)

    //QHostAddress addr;
    //
    //addr.setAddress("localhost");

    //udp_socket->bind(QHostAddress("127.0.0.1"), 12345);
    //udp_socket->bind(QHostAddress::AnyIPv4, 45454);
    bool res = udp_socket->bind(QHostAddress::AnyIPv4, 49000);
    if (!res) {
      QString errstr = udp_socket->errorString();
      qDebug() << "Error" << errstr;
      exit(1);
    }

    connect(udp_socket, SIGNAL(readyRead()), this, SLOT(readyRead()));
    connect(&_my_timer, SIGNAL(timeout()), this, SLOT(checkStatus()));
    _my_timer.setInterval(50000);
    _my_timer.start();

}

void MyUDP::HelloUDP()
{
    QByteArray Data;
    Data.append("HELLO");

    // Sends the datagram datagram
    // to the host address and at port.
    // qint64 QUdpSocket::writeDatagram(const QByteArray & datagram,
    //                      const QHostAddress & host, quint16 port)
   // for (int i = 0; i < 10; i++)
   // _my_timer.stop();
    for (int i = 0; i < 2; i++)
        udp_socket->writeDatagram(Data, QHostAddress::Broadcast, 49000);
    sleep(1);
    //_my_timer.start();
}

void MyUDP::broadcastCmd(QString &cmd)
{
    std::string tmp = cmd.toStdString();
    for (int i = 0; i < 2; i++)
        udp_socket->writeDatagram(tmp.c_str(), QHostAddress::Broadcast, 49000);
}

void MyUDP::readyRead()
{
    // when data comes in
    QByteArray buffer;
    buffer.resize(udp_socket->pendingDatagramSize());

    QHostAddress sender;
    quint16 senderPort;

    // qint64 QUdpSocket::readDatagram(char * data, qint64 maxSize,
    //                 QHostAddress * address = 0, quint16 * port = 0)
    // Receives a datagram no larger than maxSize bytes and stores it in data.
    // The sender's host address and port is stored in *address and *port
    // (unless the pointers are 0).

    udp_socket->readDatagram(buffer.data(), buffer.size(),
                         &sender, &senderPort);

    msg_count++;

    qDebug() << "Message from: " << sender.toString();
    qDebug() << "Message port: " << senderPort;
    qDebug() << "Message: " << buffer;
    DataStorage::getInstance()->addDataFromBuffer(buffer, strlen(buffer));
}

void MyUDP::checkStatus()
{
    HelloUDP();
    // if (msg_count == 0) {
    //     HelloUDP();
    // } else {
    //     msg_count = 0;
    // }

}

MyUDP* MyUDP::_my_instance = nullptr;

MyUDP* MyUDP::getInstance()
{
    if (!_my_instance)
        _my_instance = new MyUDP;
    return(_my_instance);
}

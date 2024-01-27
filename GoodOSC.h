#ifndef GOODOSC_H
#define GOODOSC_H

#include <QObject>
#include <QUdpSocket>
#include <QNetworkDatagram>
#include <QDebug>


class GoodOSCMessage : public QObject
{
    Q_OBJECT

public:
    QByteArray address;
    QByteArray typetag;
    QByteArray values;

    explicit GoodOSCMessage(QByteArray address) {
        this->address = address;
        this->typetag = ",";
    }

    void addValue(int32_t val) {
        this->typetag.append("i");

        // Swap to network byte order
        char *oval = (char *) &val;
        char nval[4];
        nval[0] = oval[3];
        nval[1] = oval[2];
        nval[2] = oval[1];
        nval[3] = oval[0];

        this->values.append(nval, 4);
    }

    void addValue(float val) {
        this->typetag.append("f");

        // Swap to network byte order
        char *oval = (char *) &val;
        char nval[4];
        nval[0] = oval[3];
        nval[1] = oval[2];
        nval[2] = oval[1];
        nval[3] = oval[0];

        this->values.append(nval, 4);
    }

    void addValue(QByteArray val) {
        this->typetag.append("s");

        val.append(1, 0);
        int fill = (val.size() % 4) > 0 ? 4 - (val.size() % 4) : 0;
        val.append(fill, 0);

        this->values.append(val);
    }
};


class GoodOSC : public QObject
{
    Q_OBJECT

private:
    QUdpSocket *udpSocket;

    QByteArray packString(QByteArray val) {
        // Null terminate
        val.append(1, 0);

        // Fill to nearest 32bit segment
        int fill = (val.size() % 4) > 0 ? 4 - (val.size() % 4) : 0;
        val.append(fill, 0);

        return val;
    }

public:
    explicit GoodOSC(QHostAddress host, quint16 port, QObject *parent = nullptr)
        : QObject{parent}
    {
        udpSocket = new QUdpSocket(this);
        udpSocket->bind(host, port);

        connect(udpSocket, &QUdpSocket::readyRead, this, &GoodOSC::readPendingDatagrams);
    }

    void send(QHostAddress host, quint16 port, GoodOSCMessage *msg) {
        msg->address = packString(msg->address);
        msg->typetag = packString(msg->typetag);

        QByteArray packet;
        packet.append(msg->address.data(),  msg->address.size());
        packet.append(msg->typetag.data(),  msg->typetag.size());
        packet.append(msg->values.data(),   msg->values.size());

        udpSocket->writeDatagram(packet, host, port);
    }

signals:

public slots:
    void readPendingDatagrams()
    {
        while (udpSocket->hasPendingDatagrams()) {
            QNetworkDatagram datagram = udpSocket->receiveDatagram();

            qDebug() << "GoodOSC: Got datagram";
            //            qDebug() << datagram.data().toStdString();
        }
    }
};

#endif // GOODOSC_H

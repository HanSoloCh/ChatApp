#include "client.h"

#include <QDataStream>

Client::Client(quint16 curPort, QObject *parent)
    : QObject(parent)
    , port(curPort)
{
    socket = new QUdpSocket(this);
    socket->bind(QHostAddress::Any, port);
    connect(socket, &QUdpSocket::readyRead, this, &Client::slotReadyRead);
}

void Client::slotReadyRead()
{
    while (socket->hasPendingDatagrams()) {
        QByteArray buffer;
        buffer.resize(socket->pendingDatagramSize());
        QHostAddress sender;
        quint16 senderPort;

        socket->readDatagram(buffer.data(), buffer.size(), &sender, &senderPort);

        QDataStream in(&buffer, QIODevice::ReadOnly);
        QString str, nickname;
        in >> str >> nickname;
        emit showMessage(str, nickname); // TODO А можно ли писать код после emit?
    }
}

void Client::slotSendToServer(const QString &str, const QString &nickname)
{
    data.clear();
    QDataStream out(&data, QIODevice::WriteOnly);
    out << str << nickname;
    socket->writeDatagram(data, QHostAddress::Broadcast, port);
}

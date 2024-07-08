#include "server.h"
#include <QDataStream>
#include <QDebug>

Server::Server()
    : port(2323)
{
    socket = new QUdpSocket(this);
    if (socket->bind(QHostAddress::Any, port)) {
        qDebug() << "Server started on port" << port;
        connect(socket, &QUdpSocket::readyRead, this, &Server::slotReadyRead);
    } else {
        qDebug() << "Error starting server";
    }
}

void Server::slotReadyRead()
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
        qDebug() << "Received message:" << str << "from" << nickname;

        clients.insert(qMakePair(sender, senderPort));

        sendToClients(str, nickname);
    }
}

void Server::sendToClients(const QString &str, const QString &nickname)
{
    data.clear();
    QDataStream out(&data, QIODevice::WriteOnly);
    out << str << nickname;

    for (const auto &client : qAsConst(clients))
        socket->writeDatagram(data, client.first, client.second);
}

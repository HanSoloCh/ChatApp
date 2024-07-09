#include "server.h"

#include <QDataStream>
#include <QDebug>

#include "message.h"

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

void Server::processIncomingMessage(const Message &message, const QHostAddress &sender, quint16 senderPort)
{
    if (message.type == UserConnected)
    {
        clients.insert(qMakePair(sender, senderPort));
        qDebug() << "New user connected" << sender << senderPort;
    }
    else if (message.type == UserDisconnected)
    {
        clients.remove(qMakePair(sender, senderPort));
        qDebug() << "User disconnected" << sender << senderPort;

    }
    else
    {
        qDebug() << "Received message part:" << message.partIndex << "of" << message.totalPartsCount << "from" << message.nickname;
        sendToClients(message, sender, senderPort);
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

        Message message;
        in >> message;

        processIncomingMessage(message, sender, senderPort);
    }
}

void Server::sendToClients(const Message &message, const QHostAddress &sender, quint16 senderPort)
{
    data.clear();
    QDataStream out(&data, QIODevice::WriteOnly);
    out << message;
    for (const auto &client : qAsConst(clients))
        if (client.first != sender || client.second != senderPort)
            socket->writeDatagram(data, client.first, client.second);
}

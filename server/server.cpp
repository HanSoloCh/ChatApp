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
        connect(&messageManager, &MessageManager::allClientsReceivedMessage, this, &Server::slotAllClientsReceivedMessage);
        resendTiemer = new QTimer(this);
        connect(resendTiemer, &QTimer::timeout, this, &Server::slotResendPackages);
        resendTiemer->start(10 * 1000);
    } else {
        qDebug() << "Error starting server";
    }
}

void Server::processIncomingMessage(const Message &message, const UserAddres &sender)
{
    if (message.header.type == SystemUserConnected)
    {
        clients.insert(sender);
        qDebug() << "New user connected" << sender;
    }
    else if (message.header.type == SystemUserDisconnected)
    {
        clients.remove(sender);
        qDebug() << "User disconnected" << sender;

    }
    else if (message.header.type == SystemMessageReceived)
    {
        qDebug() << sender << "successfully received message with" << message.getMessageId() << "ID";
        messageManager.incommingAnswer(message.getMessageId(), message.header.partIndex, sender);
    }
    else
    {
        qDebug() << "Received message part:" << message.header.partIndex + 1 << "of" << message.header.totalPartsCount << "with ID" << message.header.messageId;
        notifyClientMessageReceived(message.getMessageId(), message.header.partIndex, sender);
        if (messageManager.registerMessagePart(message, sender, clients))
            sendToClients(message, messageManager.getUserAddreses(message.getMessageId()));
        else
            slotAllClientsReceivedMessage(message.getMessageId(), sender);
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

        processIncomingMessage(message, qMakePair(sender, senderPort));
    }
}

void Server::sendToClients(const Message &message, const UserAddreses &clients)
{
    QByteArray data = makeBytes(message);
    for (const auto &client : qAsConst(clients))
        socket->writeDatagram(data, client.first, client.second);

}

void Server::sendToClients(const Message &message, const UserAddres &client)
{
    QByteArray data = makeBytes(message);
    socket->writeDatagram(data, client.first, client.second);
}

void Server::notifyClientMessageReceived(const QUuid &messageId, const qint32 &messagePart, const UserAddres &client)
{
    Message message(SystemMessageReceived, messageId, messagePart);
    sendToClients(message, client);
}

void Server::slotAllClientsReceivedMessage(const QUuid &messageId, const UserAddres &sender)
{
    Message message(SystemAllClientsReceivedMessage, messageId);
    sendToClients(message, sender);
}

void Server::slotResendPackages()
{
    auto messageParts = messageManager.getMessageParts();
    for (auto it = messageParts.begin(); it != messageParts.end(); ++it)
    {
        for (auto &parts : it.value())
        {
            sendToClients(parts, messageManager.getUserAddreses(it.key()));
        }
    }
}


QByteArray Server::makeBytes(const Message &message)
{
    QByteArray data;
    QDataStream out(&data, QIODevice::WriteOnly);
    out << message;
    return data;
}

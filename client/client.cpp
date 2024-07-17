#include "client.h"

#include <QDataStream>
#include <QDir>
#include <QFileInfo>

#include "message.h"

Client::Client(quint16 curPort, QObject *parent) : QObject(parent), port(curPort)
{
    socket = new QUdpSocket(this);
    socket->bind(QHostAddress::LocalHost, port);
    connect(socket, &QUdpSocket::readyRead, this, &Client::slotReadyRead);

    messageManager = new MessageManager(this);
    connect(messageManager, &MessageManager::textMessageComplete, this, &Client::slotTextMessageComplete);
    connect(messageManager, &MessageManager::fileMessageComplete, this, &Client::slotFileMessageComplete);

    connect(messageManager, &MessageManager::messageReceived, this, &Client::slotMessageReceived);
    connect(messageManager, &MessageManager::notifyClientMessageReceived, this, &Client::slotNotifyClientMessageReceived);

    // resendTimer = new QTimer;
    // connect(resendTimer, &QTimer::timeout, this, &Client::slotResendPackages);

    // resendTimer->start(100);
}

// void Client::notifyServerMessagePartReceived(const QUuid &messageId, const qint32 &partIndex)
// {
//     Message message(SystemMessageReceived, messageId, partIndex);
//     sendByteArray(message);
// }


void Client::slotReadyRead()
{
    while (socket->hasPendingDatagrams())
    {
        QByteArray buffer;
        buffer.resize(socket->pendingDatagramSize());
        QHostAddress sender;
        quint16 senderPort;

        socket->readDatagram(buffer.data(), buffer.size(), &sender, &senderPort);

        QDataStream in(&buffer, QIODevice::ReadOnly);
        Message message;
        in >> message;

        messageManager->processIncomingMessage(message, qMakePair(sender, senderPort));
    }
}


void Client::slotTextMessageComplete(const QUuid &messageId, QByteArray &data)
{
    QString nickname, text;
    QDataStream in(&data, QIODevice::ReadOnly);
    in >> nickname >> text;
    emit showMessage(nickname, text, messageId);
}

void Client::slotFileMessageComplete(const QUuid &messageId, QByteArray &data)
{
    QString nickname, fileName;
    QDataStream in(&data, QIODevice::ReadOnly);

    in >> nickname >> fileName;
    QFile file(QDir::temp().filePath(fileName));
    if (file.open(QIODevice::WriteOnly))
    {
        QByteArray fileData;
        in >> fileData;
        file.write(fileData);
        file.close();
        emit showFile(nickname, file.fileName(), messageId);
    }
}

void Client::slotMessageReceived(const QUuid &messageId)
{
    emit showMessageReceived(messageId);
}

void Client::slotNotifyClientMessageReceived(const QUuid &messageId, const UserAddres &addres)
{
    Message notifyMessage(SystemMessageReceived, messageId);
    sendByteArray(notifyMessage, addres);
}

void Client::slotSendMessage(const BaseMessageData &messageData, const UserAddres &addres)
{
    QByteArray data = messageData.getData();
    const int maxPacketSize = messageData.getMaxSize() - sizeof(Message::MessageHeader) - 1;
    int totalParts = (data.size() + maxPacketSize - 1) / maxPacketSize;

    for (int partIndex = 0; partIndex < totalParts; ++partIndex)
    {
        Message::MessageHeader header(messageData.type(), messageData.getId(), partIndex, totalParts);
        Message message(header, data.mid(partIndex * maxPacketSize, maxPacketSize));

        messageManager->addSentMessagePart(messageData.getId(), partIndex, message, addres);
        sendQueue.enqueue(message);
    }
}

void Client::sendByteArray(const Message &message, const UserAddres &addres)
{
    QByteArray data;
    QDataStream out(&data, QIODevice::WriteOnly);
    out << message;
    socket->writeDatagram(data, addres.first, addres.second);
}

void Client::slotSendPackage()
{
    if (!sendQueue.isEmpty())
    {
        Message message = sendQueue.dequeue();
        sendByteArray(message, messageManager->getClientAddres(message.getMessageId()));
    }
}

void Client::slotPortChanged(quint16 port)
{
    socket->close();
    socket->bind(QHostAddress::LocalHost, port);
}

// void Client::slotResendPackages()
// {
//     for (auto &packages : sentMessage)
//     {
//         for (auto &message : packages)
//         {
//             sendByteArray(message);
//         }
//     }
// }

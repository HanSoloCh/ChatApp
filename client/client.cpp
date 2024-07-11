#include "client.h"

#include <QDataStream>
#include <QDir>
#include <QFileInfo>

#include "message.h"

Client::Client(quint16 curPort, QObject *parent) : QObject(parent), port(curPort)
{
    socket = new QUdpSocket(this);
    socket->bind(QHostAddress::Any, port);
    connect(socket, &QUdpSocket::readyRead, this, &Client::slotReadyRead);

    resendTimer = new QTimer;
    connect(resendTimer, &QTimer::timeout, this, &Client::slotResendPackages);

    resendTimer->start(10 * 1000);

    Message helloMessage(SystemUserConnected);
    sendByteArray(helloMessage);
}

Client::~Client()
{
    Message byeMessage(SystemUserDisconnected);
    sendByteArray(byeMessage);
}

void Client::processIncomingMessage(const Message::MessageHeader &info, const QByteArray &data)
{
    if ((info.type == UserMessage || info.type == UserFile) && !completeMessage.contains(info.messageId))
    {
        messageParts[info.messageId][info.partIndex] = data;
        if (messageParts[info.messageId].size() == info.totalPartsCount)
        {
            makeCompleteMessage(info.messageId, info.totalPartsCount, info.type);
            messageParts.remove(info.messageId);
        }
        notifyServerMessagePartReceived(info.messageId, info.partIndex);
    }
    else if (info.type == SystemMessageReceived)
        serverReceivedMessage(info.messageId, info.partIndex);
    else if (info.type == SystemAllClientsReceivedMessage)
        emit signalAllClientsReceivedMessage(info.messageId);
}

void Client::serverReceivedMessage(const QUuid &messageId, const qint32 &messagePart)
{
    sentMessage[messageId].remove(messagePart);
    messageProcess[messageId].first++;
    if (messageProcess[messageId].first == messageProcess[messageId].second)
    {
        sentMessage.remove(messageId);
        messageProcess.remove(messageId);
        emit signalServerReceivedMessage(messageId);
    }
}

void Client::notifyServerMessagePartReceived(const QUuid &messageId, const qint32 &partIndex)
{
    Message message(SystemMessageReceived, messageId, partIndex);
    sendByteArray(message);
}

void Client::makeCompleteMessage(QUuid messageId, qint32 totalParts, MessageType type)
{
    QByteArray fullMessage;
    QDataStream in(&fullMessage, QIODevice::ReadOnly);
    for (qint32 i = 0; i < totalParts; ++i)
    {
        fullMessage.append(messageParts[messageId][i]);
    }

    if (type == UserMessage)
    {
        makeCompleteTextMessage(in, messageId);
    }
    else
    {
        makeCompleteFile(in, messageId);
    }
    completeMessage.insert(messageId);
}

void Client::makeCompleteTextMessage(QDataStream &in, QUuid messageId)
{
    QString nickname, text;
    in >> nickname >> text;
    emit showMessage(nickname, text, messageId);
}

void Client::makeCompleteFile(QDataStream &in, QUuid messageId)
{
    QString nickname, fileName;
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

        processIncomingMessage(message.header, message.messageData);
    }
}

void Client::slotSendToServer(const BaseCommand &command)
{
    QByteArray data = command.getData();
    const int maxPacketSize = command.getMaxSize() - sizeof(Message::MessageHeader) - 1;
    int totalParts = (data.size() + maxPacketSize - 1) / maxPacketSize;

    for (int partIndex = 0; partIndex < totalParts; ++partIndex)
    {
        Message::MessageHeader header(command.type(), command.getId(), partIndex, totalParts);
        Message message(header, data.mid(partIndex * maxPacketSize, maxPacketSize));

        sendQueue.enqueue(message);
    }
}

void Client::sendByteArray(const Message &message)
{
    QByteArray data;
    QDataStream out(&data, QIODevice::WriteOnly);
    out << message;
    socket->writeDatagram(data, QHostAddress::Broadcast, port);
}

void Client::slotSendPackage()
{
    if (!sendQueue.isEmpty())
    {
        const Message message = sendQueue.dequeue();
        sentMessage[message.getMessageId()][message.header.partIndex] = message;
        messageProcess[message.getMessageId()].second = message.header.totalPartsCount;
        sendByteArray(message);
    }
}

void Client::slotResendPackages()
{
    for (auto &packages : sentMessage)
    {
        for (auto &message : packages)
        {
            sendByteArray(message);
        }
    }
}

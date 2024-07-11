#include "client.h"

#include <QDataStream>
#include <QFileInfo>
#include <QDir>

#include "message.h"

Client::Client(quint16 curPort, QObject *parent)
    : QObject(parent)
    , port(curPort)
    , currentMessageId(0)
{
    socket = new QUdpSocket(this);
    socket->bind(QHostAddress::Any, port);
    connect(socket, &QUdpSocket::readyRead, this, &Client::slotReadyRead);

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
    if (info.type == UserMessage || info.type == UserFile)
    {
        messageParts[info.messageId][info.partIndex] = data;
        if (messageParts[info.messageId].size() == info.totalPartsCount)
        {
            makeCompleteMessage(info.messageId, info.totalPartsCount, info.type);
            messageParts.remove(info.messageId);
        }
        notifyServerMessagePartReceived(info.messageId, info.partIndex);
    } else if (info.type == SystemMessageReceived)
        serverReceivedMessage(info.messageId);
    else if (info.type == SystemAllClientsReceivedMessage)
        emit signalAllClientsReceivedMessage(info.messageId);
}

void Client::serverReceivedMessage(const QUuid &messageId)
{
    sentMessage.remove(messageId);
    emit signalServerReceivedMessage(messageId);
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
    QString nickname;

    for (qint32 i = 0; i < totalParts; ++i)
    {
        fullMessage.append(messageParts[messageId][i]);
    }

    if (type == UserMessage)
    {
        QString text;
        in >> nickname >> text;
        emit showMessage(nickname, text, messageId);
    }
    else
    {
        QString fileName;
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

    for (int partIndex = 0; partIndex < totalParts; ++partIndex) {
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
    qDebug() << data.size();
    socket->writeDatagram(data, QHostAddress::Broadcast, port);
}


void Client::slotSendPackage()
{
    if (!sendQueue.isEmpty())
    {
        const Message message = sendQueue.dequeue();
        sentMessage[message.getMessageId()] = message;
        sendByteArray(message);
    }
}



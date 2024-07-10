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

    Message helloMessage;
    helloMessage.header.type = SystemUserConnected;
    sendByteArray(helloMessage);
}

Client::~Client()
{
    Message byeMessage;
    byeMessage.header.type = SystemUserDisconnected;
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
    }
}

void Client::makeCompleteMessage(qint32 messageId, qint32 totalParts, MessageType type)
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
        emit showMessage(nickname, text);
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
            emit showFile(nickname, file.fileName());
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

        processIncomingMessage(message.header, message.data);
    }
}

void Client::slotSendMessageToServer(const QString &nickname, const QString &str, int maxSize)
{
    const int maxPacketSize = maxSize - sizeof(Message::MessageHeader) - 1;
    QByteArray data;
    QDataStream out(&data, QIODevice::WriteOnly);
    out << nickname << str;

    int totalParts = (data.size() + maxPacketSize - 1) / maxPacketSize;
    int messageId = currentMessageId++;

    for (int partIndex = 0; partIndex < totalParts; ++partIndex) {
        Message message;
        message.header.type = UserMessage;
        message.header.messageId = messageId;
        message.header.partIndex = partIndex;
        message.header.totalPartsCount = totalParts;
        message.data = data.mid(partIndex * maxPacketSize, maxPacketSize);

        sendQueue.enqueue(message);
    }
}

void Client::slotSendFileToServer(const QString &nickname, QFile &file, int maxSize)
{
    const int maxPacketSize = maxSize - sizeof(Message::MessageHeader) - 1;

    QByteArray data;
    QDataStream out(&data, QIODevice::WriteOnly);
    out << nickname << QFileInfo(file).baseName() << file.readAll();

    int totalParts = (data.size() + maxPacketSize - 1) / maxPacketSize;
    int messageId = currentMessageId++;

    for (int partIndex = 0; partIndex < totalParts; ++partIndex) {
        Message message;
        message.header.type = UserFile;
        message.header.messageId = messageId;
        message.header.partIndex = partIndex;
        message.header.totalPartsCount = totalParts;
        message.data = data.mid(partIndex * maxPacketSize, maxPacketSize);
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
        sendByteArray(sendQueue.dequeue());
}



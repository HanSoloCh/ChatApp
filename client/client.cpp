#include "client.h"

#include <QDataStream>

#include "message.h"

Client::Client(quint16 curPort, QObject *parent)
    : QObject(parent)
    , port(curPort)
{
    socket = new QUdpSocket(this);
    socket->bind(QHostAddress::Any, port);
    connect(socket, &QUdpSocket::readyRead, this, &Client::slotReadyRead);

    Message helloMessage;
    helloMessage.type = UserConnected;
    sendByteArray(helloMessage);
}

Client::~Client()
{
    Message byeMessage;
    byeMessage.type = UserDisconnected;
    sendByteArray(byeMessage);
}



void Client::processIncomingMessage(const Message &message)
{
    if (message.type == UserMessage)
    {
        messageParts[message.messageId][message.partIndex] = message.text;
        if (messageParts[message.messageId].size() == message.totalPartsCount)
        {
            QString completeMessage = makeCompleteMessage(message.messageId, message.totalPartsCount);
            messageParts.remove(message.messageId);
            emit showMessage(message.nickname, completeMessage);
        }
    }
}

QString Client::makeCompleteMessage(qint32 messageId, qint32 totalParts)
{
    QString completeMessage;
    for (qint32 i = 0; i < totalParts; ++i)
    {
        completeMessage.append(messageParts[messageId][i]);
    }
    return completeMessage;
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
        processIncomingMessage(message);
    }
}

void Client::slotSendToServer(const QString &nickname, const QString &str)
{
    const int maxPacketSize = 10;
    int totalParts = (str.size() + maxPacketSize - 1) / maxPacketSize;
    int messageId = currentMessageId++;

    for (int partIndex = 0; partIndex < totalParts; ++partIndex) {
        Message message;
        message.type = UserMessage;
        message.messageId = messageId;
        message.partIndex = partIndex;
        message.totalPartsCount = totalParts;
        message.nickname = nickname;
        message.text = str.mid(partIndex * maxPacketSize, maxPacketSize);

        sendByteArray(message);
    }
}

void Client::sendByteArray(const Message &message)
{
    QByteArray data;
    QDataStream out(&data, QIODevice::WriteOnly);
    out << message;

    socket->writeDatagram(data, QHostAddress::Broadcast, port);

}

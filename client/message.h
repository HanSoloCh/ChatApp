#ifndef MESSAGE_H
#define MESSAGE_H

#include <QDataStream>
#include <QString>
#include <QUuid>

enum MessageType
{
    SystemMessageReceived,
    UserMessage,
    UserFile,
};

struct Message
{
    struct MessageHeader;

    Message() = default;
    Message(MessageType curType, QUuid id, qint32 index, qint32 count, const QByteArray &data);
    Message(MessageType curType, QUuid id);
    Message(const Message::MessageHeader &head, const QByteArray &data);

    QUuid getMessageId() const { return header.messageId; }

    struct MessageHeader
    {
        MessageHeader() = default;
        explicit MessageHeader(MessageType curType, QUuid id = 0, qint32 index = 0, qint32 count = 0);
        MessageType type;
        QUuid messageId;
        qint32 partIndex;
        qint32 totalPartsCount;

    } header;
    QByteArray messageData;
};

QDataStream &operator<<(QDataStream &out, const Message &message);
QDataStream &operator>>(QDataStream &in, Message &message);

#endif // MESSAGE_H

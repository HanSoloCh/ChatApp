#ifndef MESSAGE_H
#define MESSAGE_H

#include <QDataStream>
#include <QString>
#include <QUuid>

enum MessageType {
    SystemMessageReceived,
    SystemRequestMessagePart,
    UserMessage,
    UserFile,
};

struct Message {
    struct MessageHeader;

    Message() = default;
    Message(MessageType curType, QUuid id, quint32 index, quint32 count, const QByteArray& data);
    Message(MessageType curType, QUuid id, quint32 index = 0);
    Message(const Message::MessageHeader& head, const QByteArray& data);

    QUuid getMessageId() const { return header.messageId; }
    qint32 getIndex() const { return header.partIndex; }
    qint32 getTotalPartsCount() const { return header.totalPartsCount; }
    MessageType getMessageType() const { return header.type; }

    struct MessageHeader {
        MessageHeader() = default;
        explicit MessageHeader(MessageType curType, QUuid id = 0, quint32 index = 0,
                               quint32 count = 0);
        MessageType type;
        QUuid messageId;
        qint32 partIndex;
        qint32 totalPartsCount;

    } header;
    QByteArray messageData;
};

QDataStream& operator<<(QDataStream& out, const Message& message);
QDataStream& operator>>(QDataStream& in, Message& message);

#endif  // MESSAGE_H

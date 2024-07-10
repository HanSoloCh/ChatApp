#ifndef MESSAGE_H
#define MESSAGE_H

#include <QString>
#include <QDataStream>

enum MessageType
{
    UserConnected,
    UserDisconnected,
    UserMessage,
    UserFile,
};

struct Message
{
    struct MessageHeader
    {
        MessageType type;
        qint32 messageId;
        qint32 partIndex;
        qint32 totalPartsCount;

    } header;
    QByteArray data;
};

QDataStream &operator<<(QDataStream &out, const Message &message);
QDataStream &operator>>(QDataStream &in, Message &message);

#endif // MESSAGE_H

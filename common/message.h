#ifndef MESSAGE_H
#define MESSAGE_H

#include <QString>
#include <QDataStream>

enum MessageType
{
    UserConnected,
    UserDisconnected,
    UserMessage,
};

struct Message
{
    MessageType type;
    qint32 messageId;
    qint32 partIndex;
    qint32 totalPartsCount;
    QString nickname;
    QString text;
};

QDataStream &operator<<(QDataStream &out, const Message &message);
QDataStream &operator>>(QDataStream &in, Message &message);

#endif // MESSAGE_H

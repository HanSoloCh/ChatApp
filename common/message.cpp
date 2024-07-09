#include "message.h"

QDataStream &operator<<(QDataStream &out, const Message &message)
{
    out << static_cast<qint8>(message.type)
        << message.messageId
        << message.partIndex
        << message.totalPartsCount
        << message.nickname
        << message.text;
    return out;
}


QDataStream &operator>>(QDataStream &in, Message &message)
{
    qint8 type;
    in >> type;
    message.type = static_cast<MessageType>(type);
    in  >> message.messageId
        >> message.partIndex
        >> message.totalPartsCount
        >> message.nickname
        >> message.text;
    return in;
}

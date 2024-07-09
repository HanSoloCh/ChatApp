#include "message.h"


QDataStream &operator<<(QDataStream &out, const Message::MessageHeader &messageHeader)
{
    out << static_cast<quint8>(messageHeader.type)
        << messageHeader.messageId
        << messageHeader.partIndex
        << messageHeader.totalPartsCount;
    return out;
}

QDataStream &operator>>(QDataStream &in, Message::MessageHeader &messageHeader)
{
    quint8 type;
    in >> type
       >> messageHeader.messageId
       >> messageHeader.partIndex
       >> messageHeader.totalPartsCount;
    messageHeader.type = static_cast<MessageType>(type);
    return in;
}



QDataStream &operator<<(QDataStream &out, const Message &message)
{
    out << message.header
        << message.data;
    return out;
}


QDataStream &operator>>(QDataStream &in, Message &message)
{
    in >> message.header
       >> message.data;
    return in;
}

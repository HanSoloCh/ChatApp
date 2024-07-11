#include "message.h"

QDataStream &operator<<(QDataStream &out, const Message::MessageHeader &messageHeader)
{
    out << static_cast<quint8>(messageHeader.type) << messageHeader.messageId << messageHeader.partIndex
        << messageHeader.totalPartsCount;
    return out;
}

QDataStream &operator>>(QDataStream &in, Message::MessageHeader &messageHeader)
{
    quint8 type;
    in >> type >> messageHeader.messageId >> messageHeader.partIndex >> messageHeader.totalPartsCount;
    messageHeader.type = static_cast<MessageType>(type);
    return in;
}

QDataStream &operator<<(QDataStream &out, const Message &message)
{
    out << message.header << message.messageData;
    return out;
}

QDataStream &operator>>(QDataStream &in, Message &message)
{
    in >> message.header >> message.messageData;
    return in;
}

Message::Message(MessageType curType, QUuid id, qint32 index, qint32 count, const QByteArray &data)
    : header(curType, id, index, count), messageData(data){};
Message::Message(MessageType curType, QUuid id, qint32 index) : header(curType, id, index){};
Message::Message(const Message::MessageHeader &head, const QByteArray &data) : header(head), messageData(data){};
Message::MessageHeader::MessageHeader(MessageType curType, QUuid id, qint32 index, qint32 count)
    : type(curType), messageId(id), partIndex(index), totalPartsCount(count){};

#include "messageData.h"

#include <QDataStream>
#include <QFileInfo>

QByteArray TextMessageData::getData() const
{
    QByteArray data;
    QDataStream out(&data, QIODevice::WriteOnly);
    out << nickname << messageText;
    return data;
}

QByteArray FileMessageData::getData() const
{
    QByteArray data;
    QDataStream out(&data, QIODevice::WriteOnly);
    out << nickname << QFileInfo(file).baseName() << file.readAll();
    return data;
}

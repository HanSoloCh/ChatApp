#include "command.h"

#include <QDataStream>
#include <QFileInfo>

QByteArray SendMessageCommand::getData() const
{
    QByteArray data;
    QDataStream out(&data, QIODevice::WriteOnly);
    out << nickname << messageText;
    return data;
}

QByteArray SendFileCommand::getData() const
{
    QByteArray data;
    QDataStream out(&data, QIODevice::WriteOnly);
    out << nickname << QFileInfo(file).baseName() << file.readAll();
    return data;
}

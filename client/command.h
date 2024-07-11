#ifndef COMMAND_H
#define COMMAND_H

#include <QFile>
#include <QString>
#include <QUuid>

#include "message.h"

class BaseCommand
{
  public:
    BaseCommand(MessageType type, const QString nick, const QUuid id, int size = 512)
        : messageType(type), messageId(id), nickname(nick), maxSize(size){};
    virtual ~BaseCommand() = default;
    virtual QByteArray getData() const = 0;

    MessageType type() const { return messageType; }
    int getMaxSize() const { return maxSize; }
    QUuid getId() const { return messageId; }

  protected:
    MessageType messageType;
    QUuid messageId;
    QString nickname;
    int maxSize;
};

class SendMessageCommand : public BaseCommand
{
  public:
    SendMessageCommand(MessageType type, const QString nick, const QString text, const QUuid id, int size = 512)
        : BaseCommand(type, nick, id, size), messageText(text){};
    virtual QByteArray getData() const override;

  protected:
    QString messageText;
};

class SendFileCommand : public BaseCommand
{
  public:
    SendFileCommand(MessageType type, const QString nick, QFile &conFile, const QUuid id, int size = 512)
        : BaseCommand(type, nick, id, size), file(conFile){};
    virtual QByteArray getData() const override;

  protected:
    QFile &file;
};

#endif // COMMAND_H

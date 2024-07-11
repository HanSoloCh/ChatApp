#ifndef MESSAGEDATA_H
#define MESSAGEDATA_H

#include <QFile>
#include <QString>
#include <QUuid>

#include "message.h"

class BaseMessageData
{
  public:
    BaseMessageData(MessageType type, const QString nick, const QUuid id, int size = 512)
        : messageType(type), messageId(id), nickname(nick), maxSize(size){};
    virtual ~BaseMessageData() = default;
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

class TextMessageData : public BaseMessageData
{
  public:
    TextMessageData(MessageType type, const QString nick, const QString text, const QUuid id, int size = 512)
        : BaseMessageData(type, nick, id, size), messageText(text){};
    virtual QByteArray getData() const override;

  protected:
    QString messageText;
};

class FileMessageData: public BaseMessageData
{
  public:
    FileMessageData(MessageType type, const QString nick, QFile &conFile, const QUuid id, int size = 512)
        : BaseMessageData(type, nick, id, size), file(conFile){};
    virtual QByteArray getData() const override;

  protected:
    QFile &file;
};

#endif // MESSAGEDATA_H

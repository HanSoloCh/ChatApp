#ifndef MESSAGEMANAGER_H
#define MESSAGEMANAGER_H

#include <QHostAddress>
#include <QMap>
#include <QUuid>

#include "message.h"

typedef QPair<QHostAddress, quint16> UserAddres;

class MessageManager : public QObject
{
    Q_OBJECT
  public:
    MessageManager(QObject *parent = nullptr) : QObject(parent)
    {};
    ~MessageManager() = default;

    void processIncomingMessage(const Message &message, const UserAddres &messageSender);
    void addSentMessagePart(const QUuid &messageId, const qint32 &messagePart, const Message &message, const UserAddres &addres);
    UserAddres getSender(const QUuid &messageId) const;
    UserAddres getClientAddres(const QUuid &messageId) const;

  signals:
    void textMessageComplete(const QUuid &messageId, QByteArray &message);
    void fileMessageComplete(const QUuid &messageId, QByteArray &message);

    void messageReceived(const QUuid &messageId);
    void notifyClientMessageReceived(const QUuid &messageId, const UserAddres &addres);

  private:
    QHash<QUuid, UserAddres> messageAddres; // Клиент, который должен получить то или иное сообщение
    QHash<QUuid, QHash<qint32, Message>> messages; // Части отправленных сообщений (пакеты сообщения)

    QHash<QUuid, QMap<qint32, QByteArray>> receivedParts; // Части полученных сообщений
    QHash<QUuid, UserAddres> senders; // Клиент, отправивший сообщение

    QSet<QUuid> completeMessage; // Все завершенные сообщения

    void messageProcess(const Message::MessageHeader &info, const QByteArray &data, const UserAddres &messageSender);

    void incommingMessagePart(const Message::MessageHeader &info, const QByteArray &data, const UserAddres &messageSender);
    void incommingAnswer(const QUuid &messageId);

    void makeCompleteMessage(const QUuid &messageId, qint32 totalParts, MessageType type);

    void removeMessage(const QUuid &messageId);
};

#endif // MESSAGEMANAGER_H

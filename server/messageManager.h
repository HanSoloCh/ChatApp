#ifndef MESSAGEMANAGER_H
#define MESSAGEMANAGER_H

#include <QHostAddress>
#include <QMap>
#include <QUuid>

#include "message.h"

typedef QPair<QHostAddress, quint16> UserAddres;
typedef QSet<UserAddres> UserAddreses;

class MessageManager : public QObject
{
    Q_OBJECT
  public:
    MessageManager() = default;
    ~MessageManager() = default;

    bool registerMessagePart(const Message &message, const UserAddres &sender, const UserAddreses &currentClients);
    void incommingAnswer(const QUuid &messageId, const qint32 &partIndex, const UserAddres &client);

    UserAddreses getUserAddreses(const QUuid &messageId) const;
    UserAddres popSender(const QUuid &messageId);

    QMap<QUuid, QMap<qint32, Message>> getMessageParts() const { return messageParts; }

    void deleteClient(const UserAddres &client);

  signals:
    void allClientsReceivedMessage(const QUuid &messageId, const UserAddres &sender);

  private:
    QMap<QUuid, UserAddreses> pendingClients; // Клиенты, которые должны получить сообщение
    QMap<QUuid, QMap<qint32, UserAddreses>> receivedAnswers; // Полученные ответы от клиентов
    QMap<QUuid, QMap<qint32, Message>> messageParts;         // Части сообщения
    QMap<QUuid, QPair<qint32, qint32>> messageProcess;       // Процесс отправки сообщения
    QMap<QUuid, UserAddres> senders;

    void allClientsReceivedPackage(const QUuid &messageId, const qint32 &partIndex);
    bool isMessageRegister(const QUuid &messageId);
    void removeMessage(const QUuid &messageId);
    void deleteClientAnswers(const QUuid &messageId, const UserAddres &client);
};

#endif // MESSAGEMANAGER_H

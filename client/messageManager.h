#ifndef MESSAGEMANAGER_H
#define MESSAGEMANAGER_H

#include <QHostAddress>
#include <QMap>
#include <QTimer>
#include <QUuid>

#include "message.h"

typedef QPair<QHostAddress, quint16> UserAddres;

class BaseMessageManager : public QObject {
   public:
    BaseMessageManager(QObject* parent = nullptr) : QObject(parent){};
    virtual ~BaseMessageManager() = 0;

    UserAddres getClient(const QUuid& messageId) const;

    virtual void addMessage(const Message& message, const UserAddres& messageSender);
    void removeMessage(const QUuid& messageId);

   protected:
    QHash<QUuid, UserAddres> messageClient;
    QHash<QUuid, QMap<qint32, Message>> messageParts;
};

class SendMessageManager : public BaseMessageManager {
    Q_OBJECT
   public:
    SendMessageManager(QObject* parent = nullptr) : BaseMessageManager(parent){};
    ~SendMessageManager() = default;

    Message getMessage(const QUuid& messageId, quint32 messagePart) const;
};

class ReceivedMessageManager : public BaseMessageManager {
    Q_OBJECT
   public:
    ReceivedMessageManager(QObject* parent = nullptr);
    ~ReceivedMessageManager() = default;

    void addMessage(const Message& message, const UserAddres& messageSender) override;

   signals:
    void textMessageComplete(const QUuid& messageId, QByteArray& message);
    void fileMessageComplete(const QUuid& messageId, QByteArray& message);

    void notifyClientMessageReceived(const QUuid& messageId, const UserAddres& addres);

    void requestMissingPart(const QUuid& messageId, quint32 messagePart);

   private slots:
    void slotRequestMissingParts();

   private:
    QSet<QUuid> completeMessage;

    QTimer* recedTimer;

    void messageComplete(const QUuid& messageId, MessageType type);
    void makeCompleteMessage(const QUuid& messageId, MessageType type);

    void requestMissingParts(const QUuid& messageId, quint32 totalPartsCount);
    quint32 getTotalPartsCount(const QUuid& messageId);
};

#endif  // MESSAGEMANAGER_H

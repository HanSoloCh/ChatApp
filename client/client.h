#ifndef CLIENT_H
#define CLIENT_H

#include <QFile>
#include <QMap>
#include <QObject>
#include <QQueue>
#include <QUdpSocket>
#include <QUuid>

#include "message.h"
#include "messageData.h"
#include "messageManager.h"

typedef QPair<QHostAddress, quint16> UserAddres;

class Client : public QObject {
    Q_OBJECT

   public:
    explicit Client(quint16 curPort, QObject* parent = nullptr);
    ~Client() = default;

   signals:
    void showMessage(const QString& nickname, const QString& message, QUuid messageId);
    void showFile(const QString& nickname, const QString& fileName, QUuid messageId);

    void showMessageReceived(const QUuid& messageId);

   public slots:
    void slotSendMessage(const BaseMessageData& messageData, const UserAddres& addres);
    void slotSendPackage();
    void slotPortChanged(quint16 port);

   private slots:
    void slotReadyRead();

    void slotTextMessageComplete(const QUuid& messageId, QByteArray& message);
    void slotFileMessageComplete(const QUuid& messageId, QByteArray& message);

    void slotMessageReceived(const QUuid& messageId);
    void slotNotifyClientMessageReceived(const QUuid& messageId, const UserAddres& addres);

    void slotRequestMissingPart(const QUuid& messageId, quint32 messagePart);

   private:
    QUdpSocket* socket;
    quint16 port;
    QTimer* resendTimer;
    QQueue<Message> sendQueue;

    ReceivedMessageManager* receivedMessageManager;
    SendMessageManager* sendMessageManager;

    void processIncomingMessage(const Message& message, const UserAddres& sender);
    void sendByteArray(const Message& message, const UserAddres& addres);
};

#endif  // CLIENT_H

#ifndef CLIENT_H
#define CLIENT_H

#include <QFile>
#include <QMap>
#include <QObject>
#include <QQueue>
#include <QTimer>
#include <QUdpSocket>
#include <QUuid>

#include "messageData.h"
#include "message.h"
#include "messageManager.h"

typedef QPair<QHostAddress, quint16> UserAddres;
typedef QSet<UserAddres> UserAddreses;


class Client : public QObject
{
    Q_OBJECT

  public:
    Client(quint16 curPort, QObject *parent = nullptr);
    ~Client() = default;

  private:
    QUdpSocket *socket;
    quint16 port;
    QTimer *resendTimer;
    QQueue<Message> sendQueue;

    MessageManager *messageManager;

    void processIncomingMessage(const Message::MessageHeader &info, const QByteArray &data);
    void sendByteArray(const Message &message, const UserAddres &addres);

  signals:
    void showMessage(const QString &nickname, const QString &message, QUuid messageId);
    void showFile(const QString &nickname, const QString &fileName, QUuid messageId);

    void showMessageReceived(const QUuid &messageId);

  public slots:
    void slotSendMessage(const BaseMessageData &messageData, const UserAddres &addres);
    void slotSendPackage();
    void slotPortChanged(quint16 port);

  private slots:
    void slotReadyRead();
    // void slotResendPackages();
    void slotTextMessageComplete(const QUuid &messageId, QByteArray &message);
    void slotFileMessageComplete(const QUuid &messageId, QByteArray &message);

    void slotMessageReceived(const QUuid &messageId);
    void slotNotifyClientMessageReceived(const QUuid &messageId, const UserAddres &addres);
};

#endif // CLIENT_H

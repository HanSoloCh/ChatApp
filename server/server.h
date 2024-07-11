#ifndef SERVER_H
#define SERVER_H

#include <QPair>
#include <QSet>
#include <QTimer>
#include <QUdpSocket>
#include <QUuid>

#include "message.h"
#include "messageManager.h"

typedef QPair<QHostAddress, quint16> UserAddres;
typedef QSet<UserAddres> UserAddreses;

class Server : public QObject
{
    Q_OBJECT

  public:
    Server();
    ~Server() = default;

  private:
    QUdpSocket *socket;
    quint16 port;
    QSet<UserAddres> clients;
    MessageManager messageManager;

    QTimer *resendTiemer;

    void processIncomingMessage(const Message &message, const UserAddres &sender);
    void sendToClients(const Message &message, const UserAddreses &clients);
    void sendToClients(const Message &message, const UserAddres &client);

    void notifyClientMessageReceived(const QUuid &messageId, const qint32 &messagePart, const UserAddres &client);

    QByteArray makeBytes(const Message &message);

  private slots:
    void slotReadyRead();
    void slotAllClientsReceivedMessage(const QUuid &messageId, const UserAddres &sender);
    void slotResendPackages();
};

#endif // SERVER_H

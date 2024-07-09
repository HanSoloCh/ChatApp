#ifndef SERVER_H
#define SERVER_H

#include <QUdpSocket>
#include <QSet>
#include <QPair>

#include "message.h"

class Server : public QObject
{
    Q_OBJECT

public:
    Server();
    ~Server() = default;

private:
    QUdpSocket *socket;
    QByteArray data;
    quint16 port;
    QSet<QPair<QHostAddress, quint16>> clients;

    void processIncomingMessage(const Message &message, const QHostAddress &sender, quint16 senderPort);

    void sendToClients(const Message &message, const QHostAddress &sender, quint16 senderPort);

public slots:
    void slotReadyRead();
};

#endif // SERVER_H

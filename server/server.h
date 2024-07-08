#ifndef SERVER_H
#define SERVER_H

#include <QUdpSocket>
#include <QSet>
#include <QPair>

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

    void sendToClients(const QString &str, const QString &nickname);

public slots:
    void slotReadyRead();
};

#endif // SERVER_H

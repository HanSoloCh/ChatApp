#ifndef CLIENT_H
#define CLIENT_H

#include <QObject>
#include <QUdpSocket>

class Client : public QObject
{
    Q_OBJECT

public:
    Client(quint16 curPort, QObject *parent = nullptr);
    ~Client() = default;

private:
    QUdpSocket *socket;
    QByteArray data;
    quint16 port;

signals:
    void showMessage(const QString &message, const QString &nickname);

public slots:
    virtual void slotReadyRead();
    virtual void slotSendToServer(const QString &str, const QString &nickname);
};


#endif // CLIENT_H

#ifndef CLIENT_H
#define CLIENT_H

#include <QObject>
#include <QUdpSocket>
#include <QMap>

#include "message.h"

class Client : public QObject
{
    Q_OBJECT

public:
    Client(quint16 curPort, QObject *parent = nullptr);
    ~Client();

private:
    QUdpSocket *socket;
    quint16 port;
    QMap<qint32, QMap<quint32, QString>> messageParts;
    int currentMessageId;

    void processIncomingMessage(const Message &message);
    QString makeCompleteMessage(qint32 messageId, qint32 totalParts);
    void sendByteArray(const Message &message);

signals:
    void showMessage(const QString &nickname, const QString &message);

public slots:
    void slotReadyRead();
    void slotSendToServer(const QString &nickname, const QString &message);
};


#endif // CLIENT_H

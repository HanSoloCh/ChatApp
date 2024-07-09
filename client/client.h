#ifndef CLIENT_H
#define CLIENT_H

#include <QObject>
#include <QUdpSocket>
#include <QMap>
#include <QQueue>

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
    QMap<qint32, QMap<qint32, QByteArray>> messageParts;
    int currentMessageId;

    QQueue<Message> sendQueue;


    void processIncomingMessage(const Message::MessageHeader &info, const QByteArray &data);
    void sendByteArray(const Message &message);
    void makeCompleteMessage(qint32 messageId, qint32 totalParts);

signals:
    void showMessage(const QString &nickname, const QString &message);

public slots:
    void slotReadyRead();
    void slotSendToServer(const QString &nickname, const QString &message, int maxSize = 512);
    void slotSendPackage();
};


#endif // CLIENT_H

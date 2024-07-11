#ifndef CLIENT_H
#define CLIENT_H

#include <QObject>
#include <QUdpSocket>
#include <QMap>
#include <QQueue>
#include <QFile>
#include <QUuid>

#include "message.h"
#include "command.h"

class Client : public QObject
{
    Q_OBJECT

public:
    Client(quint16 curPort, QObject *parent = nullptr);
    ~Client();

private:
    QUdpSocket *socket;
    quint16 port;
    QHash<QUuid, QMap<qint32, QByteArray>> messageParts;
    QQueue<Message> sendQueue;

    QHash<QUuid, QHash<qint32, Message>> sentMessage;

    void makeCompleteMessage(QUuid messageId, qint32 totalParts, MessageType type);
    void processIncomingMessage(const Message::MessageHeader &info, const QByteArray &data);
    void sendByteArray(const Message &message);
    void serverReceivedMessage(const QUuid &messageId, const qint32 &messagePart);
    void notifyServerMessagePartReceived(const QUuid &messageId, const qint32 &partIndex);
    void allClientsReceivedMessage() const;

signals:
    void showMessage(const QString &nickname, const QString &message, QUuid messageId);
    void showFile(const QString &nickname, const QString &fileName, QUuid messageId);

    void signalServerReceivedMessage(QUuid messageId);
    void signalAllClientsReceivedMessage(QUuid messageId);


public slots:
    void slotReadyRead();
    void slotSendToServer(const BaseCommand &command);
    void slotSendPackage();
};


#endif // CLIENT_H

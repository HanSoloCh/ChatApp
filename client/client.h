#ifndef CLIENT_H
#define CLIENT_H

#include <QObject>
#include <QUdpSocket>
#include <QMap>
#include <QQueue>
#include <QFile>
#include <QUuid>
#include <QTimer>

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
    QTimer *resendTimer;
    QHash<QUuid, QMap<qint32, QByteArray>> messageParts;
    QQueue<Message> sendQueue;

    QHash<QUuid, QHash<qint32, Message>> sentMessage;
    QHash<QUuid, QPair<qint32, qint32>> messageProcess;
    QSet<QUuid> completeMessage;

    void makeCompleteMessage(QUuid messageId, qint32 totalParts, MessageType type);
    void makeCompleteTextMessage(QDataStream &in, QUuid messageId);
    void makeCompleteFile(QDataStream &in, QUuid messageId);
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
    void slotSendToServer(const BaseCommand &command);
    void slotSendPackage();

private slots:
    void slotReadyRead();
    void slotResendPackages();

};


#endif // CLIENT_H

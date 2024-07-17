#include "messageManager.h"


void MessageManager::processIncomingMessage(const Message &message, const UserAddres &messageSender)
{
    messageProcess(message.header, message.messageData, messageSender);
}

void MessageManager::messageProcess(const Message::MessageHeader &info, const QByteArray &data, const UserAddres &messageSender)
{
    if (info.type == UserMessage || info.type == UserFile)
    {
        incommingMessagePart(info, data, messageSender);
    }
    else
        incommingAnswer(info.messageId);
}

void MessageManager::incommingMessagePart(const Message::MessageHeader &info, const QByteArray &data, const UserAddres &messageSender)
{
    if (completeMessage.contains(info.messageId))
        return;

    senders[info.messageId] = messageSender;
    receivedParts[info.messageId][info.partIndex] = data;
    if (receivedParts[info.messageId].size() == info.totalPartsCount)
    {
        makeCompleteMessage(info.messageId, info.totalPartsCount, info.type);
        UserAddres sender = getSender(info.messageId);
        removeMessage(info.messageId);
        emit notifyClientMessageReceived(info.messageId, sender);
    }
}

void MessageManager::makeCompleteMessage(const QUuid &messageId, qint32 totalParts, MessageType type)
{
    QByteArray fullMessage;
    for (qint32 i = 0; i < totalParts; ++i)
    {
        fullMessage.append(receivedParts[messageId][i]);
    }

    if (type == UserMessage)
    {
        emit textMessageComplete(messageId, fullMessage);
    }
    else
    {
        emit fileMessageComplete(messageId, fullMessage);
    }
    completeMessage.insert(messageId);
}

void MessageManager::addSentMessagePart(const QUuid &messageId, const qint32 &messagePart, const Message &message, const UserAddres &addres)
{
    messageAddres[messageId] = addres;
    messages[messageId][messagePart] = message;
}

UserAddres MessageManager::getSender(const QUuid &messageId) const
{
    return senders[messageId];
}

UserAddres MessageManager::getClientAddres(const QUuid &messageId) const
{
    return messageAddres[messageId];
}

void MessageManager::incommingAnswer(const QUuid &messageId)
{
    removeMessage(messageId);
    emit messageReceived(messageId);
}

void MessageManager::removeMessage(const QUuid &messageId)
{
    messageAddres.remove(messageId);
    messages.remove(messageId);
    receivedParts.remove(messageId);
    senders.remove(messageId);
}

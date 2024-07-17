#include "messageManager.h"

BaseMessageManager::~BaseMessageManager()
{
}

UserAddres BaseMessageManager::getClient(const QUuid &messageId) const
{
    return messageClient[messageId];
}

void BaseMessageManager::addMessage(const Message &message, const UserAddres &messageSender)
{
    messageClient[message.header.messageId] = messageSender;
    messageParts[message.header.messageId][message.header.partIndex] = message;
}

void BaseMessageManager::removeMessage(const QUuid &messageId)
{
    messageClient.remove(messageId);
    messageParts.remove(messageId);
}

ReceivedMessageManager::ReceivedMessageManager(QObject *parent): BaseMessageManager(parent)
{
    recedTimer = new QTimer(this);
    connect(recedTimer, &QTimer::timeout, this, &ReceivedMessageManager::slotRequestMissingParts);
    recedTimer->start(5 * 1000);
}

void ReceivedMessageManager::addMessage(const Message &message, const UserAddres &messageSender)
{
    if (completeMessage.contains(message.getMessageId()))
        return;

    BaseMessageManager::addMessage(message, messageSender);
    if (messageParts[message.getMessageId()].size() == message.getTotalPartsCount())
        messageComplete(message.getMessageId(), message.getMessageType());
}

void ReceivedMessageManager::slotRequestMissingParts()
{
    for (auto it = messageParts.begin(); it != messageParts.end(); ++it)
    {
        quint32 totalParts = getTotalPartsCount(it.key());
        requestMissingParts(it.key(), totalParts);
    }
}

void ReceivedMessageManager::requestMissingParts(const QUuid &messageId, quint32 totalPartsCount)
{
    for (size_t i = 0; i < totalPartsCount; ++i)
    {
        if (!messageParts[messageId].contains(i))
        {
            emit requestMissingPart(messageId, i);
        }
    }
}

quint32 ReceivedMessageManager::getTotalPartsCount(const QUuid &messageId)
{
    return messageParts[messageId].first().getTotalPartsCount();
}

void ReceivedMessageManager::messageComplete(const QUuid &messageId, MessageType type)
{
    makeCompleteMessage(messageId, type);
    UserAddres sender = getClient(messageId);
    removeMessage(messageId);
    emit notifyClientMessageReceived(messageId, sender);
}

void ReceivedMessageManager::makeCompleteMessage(const QUuid &messageId, MessageType type)
{
    QByteArray fullMessage;
    for (const auto &parts: qAsConst(messageParts[messageId]))
    {
        fullMessage.append(parts.messageData);
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

Message SendMessageManager::getMessage(const QUuid &messageId, quint32 messagePart) const
{
    return messageParts[messageId][messagePart];
}


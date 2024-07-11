#include "messageManager.h"


bool MessageManager::registerMessagePart(const Message &message, const UserAddres &sender, const UserAddreses &currentClients)
{
    UserAddreses users(currentClients);
    users.remove(sender);
    if (users.isEmpty())
        return false;

    if (!isMessageRegister(message.getMessageId()))
    {
        pendingClients[message.getMessageId()] = users;
        senders[message.getMessageId()] = sender;
        messageProcess[message.getMessageId()].second = message.header.totalPartsCount;
    }
    messageParts[message.getMessageId()][message.header.partIndex] = message;
    return true;
}

void MessageManager::incommingAnswer(const QUuid &messageId, const qint32 &partIndex, const UserAddres &client)
{
    receivedAnswers[messageId][partIndex].insert(client);
    // Если все клиенты получили пакет
    if (receivedAnswers[messageId][partIndex].size() == pendingClients[messageId].size())
    {
        allClientsReceivedPackage(messageId, partIndex);
    }
}

void MessageManager::deleteClient(const UserAddres &client)
{
    for (auto it = pendingClients.begin(); it != pendingClients.end(); ++it)
    {
        if (it.value().remove(client))
            deleteClientAnswers(it.key(), client);
    }
}

void MessageManager::deleteClientAnswers(const QUuid &messageId, const UserAddres &client)
{
    for (auto it = receivedAnswers[messageId].begin(); it != receivedAnswers[messageId].end(); ++it)
    {
        it.value().remove(client);
        if (receivedAnswers[messageId][it.key()].size() == pendingClients[messageId].size())
            allClientsReceivedPackage(messageId, it.key());
    }
}


UserAddreses MessageManager::getUserAddreses(const QUuid &messageId) const
{
    return pendingClients[messageId];
}

UserAddres MessageManager::popSender(const QUuid &messageId)
{
    UserAddres sender(senders[messageId]);
    senders.remove(messageId);
    return sender;
}

void MessageManager::allClientsReceivedPackage(const QUuid &messageId, const qint32 &partIndex)
{
    receivedAnswers[messageId].remove(partIndex);
    messageParts[messageId].remove(partIndex);
    messageProcess[messageId].first++;
    // Если все пакеты сообщения были доставлены
    if (messageProcess[messageId].first == messageProcess[messageId].second)
    {
        removeMessage(messageId);
        emit allClientsReceivedMessage(messageId, popSender(messageId));
    }
}

bool MessageManager::isMessageRegister(const QUuid &messageId)
{
    return messageParts.contains(messageId);
}

void MessageManager::removeMessage(const QUuid &messageId)
{
    messageParts.remove(messageId);
    receivedAnswers.remove(messageId);
    pendingClients.remove(messageId);
    messageProcess.remove(messageId);
}

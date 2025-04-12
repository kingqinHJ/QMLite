#include "message.h"

Message::Message(QObject *parent)
    : QObject{parent},mauthor(new MessageAuthor(this))
{}

QQmlListProperty<Messagebody> Message::messagebodys()
{
    return QQmlListProperty<Messagebody>(this, &m_bodyList,
                                       &Message::appendMessageBody,
                                       &Message::MessageBodyCount,
                                       &Message::MessageBody,
                                       &Message::clearMessageBody,
                                       &Message::replaceMessageBody,
                                       &Message::removeLastMessageBody);
}

void Message::appendMessagebody(Messagebody *p)
{
    m_bodyList.append(p);
    emit messageBodyListChanged();
}

int Message::messagebodyCount() const
{
    return m_bodyList.count();
}

Messagebody *Message::messagebody(int index) const
{
    return m_bodyList.at(index);
}

void Message::clearMessagebody()
{
    m_bodyList.clear();
    emit messageBodyListChanged();
}

void Message::replaceMessagebody(int index, Messagebody *p)
{
    if (index >= 0 && index < m_bodyList.count()) {
        m_bodyList[index] = p;
        emit messageBodyListChanged();
    }
}

void Message::removeLastMessagebody()
{
    if (!m_bodyList.isEmpty()) {
        m_bodyList.removeLast();
        emit messageBodyListChanged();
    }
}

void Message::appendMessageBody(QQmlListProperty<Messagebody> *list, Messagebody *p)
{
    static_cast<Message*>(list->object)->appendMessagebody(p);
}

int Message::MessageBodyCount(QQmlListProperty<Messagebody> *list)
{
    return static_cast<Message*>(list->object)->messagebodyCount();
}

Messagebody *Message::MessageBody(QQmlListProperty<Messagebody> *list, int i)
{
    return static_cast<Message*>(list->object)->messagebody(i);
}

void Message::clearMessageBody(QQmlListProperty<Messagebody> *list)
{
    static_cast<Message*>(list->object)->clearMessagebody();
}

void Message::replaceMessageBody(QQmlListProperty<Messagebody> *list, int i, Messagebody *p)
{
    static_cast<Message*>(list->object)->replaceMessagebody(i, p);
}

void Message::removeLastMessageBody(QQmlListProperty<Messagebody> *list)
{
    static_cast<Message*>(list->object)->removeLastMessagebody();
}

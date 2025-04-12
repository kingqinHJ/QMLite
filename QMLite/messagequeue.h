#ifndef MESSAGEQUEUE_H
#define MESSAGEQUEUE_H

#include <QObject>
#include <QQmlListProperty>
#include <QQmlEngine>
#include "message.h"

class MessageQueue : public QObject
{
    Q_OBJECT
    Q_PROPERTY(Message *curMessage READ curMessage WRITE setCurMessage)
    Q_PROPERTY(QQmlListProperty<Message> messages READ messages)
    Q_CLASSINFO("DefaultProperty", "messages")
    Q_PROPERTY(int messagesCount READ messagesCount CONSTANT)
    QML_ELEMENT
public:
    explicit MessageQueue(QObject *parent = nullptr);

    Message *curMessage() const{
        return m_curMessage;
    }
    void setCurMessage(Message * v_message){
        m_curMessage=v_message;
         qDebug()<<"do it1";
    }

    QQmlListProperty<Message> messages(){
        qDebug()<<"do it";
        return {this, &m_messages};                  //使用C11特性初始化链表QQmlListProperty<Message>
    }
    int messagesCount() const{
        return m_messages.size();
    }
    Message *message(int index) const{
        return m_messages[index];
    }

private:
    Message *m_curMessage;
    QList<Message *> m_messages;
};

#endif // MESSAGEQUEUE_H

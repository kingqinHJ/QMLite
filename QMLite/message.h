#ifndef MESSAGE_H
#define MESSAGE_H

#include <QObject>
#include "messagebody.h"
#include <QQmlListProperty>
#include "messageauthor.h"

class Message : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString name READ name WRITE setName NOTIFY nameChanged FINAL) //使用C++属性
    Q_PROPERTY(Messagebody* body READ messageBody WRITE setMessageBody NOTIFY messageBodyChanged FINAL) //对象类型的属性
    Q_PROPERTY(QQmlListProperty<Messagebody> bodyList READ messagebodys NOTIFY messageBodyListChanged ) //对象列表类型的属性
    Q_PROPERTY(MessageAuthor *author READ author)
public:
    Message(QObject *parent = nullptr);

    QString name(){ return mname;}

    void setName(QString name)
    {
        mname=name;
    }

    Messagebody* messageBody() { return m_curMessageBody; }
    void setMessageBody(Messagebody* messageBody) {
        if (m_curMessageBody != messageBody) {
            m_curMessageBody = messageBody;
            emit messageBodyChanged();
        }
    }

    Q_INVOKABLE QQmlListProperty<Messagebody> messagebodys();
    Q_INVOKABLE void appendMessagebody(Messagebody*);
    Q_INVOKABLE int messagebodyCount() const;
    Q_INVOKABLE Messagebody *messagebody(int) const;
    Q_INVOKABLE void clearMessagebody();
    Q_INVOKABLE void replaceMessagebody(int, Messagebody*);
    Q_INVOKABLE void removeLastMessagebody();

    Q_INVOKABLE MessageAuthor* author()const {
        return mauthor;
    }

signals:
    void nameChanged();
    void messageBodyChanged();
    void messageBodyListChanged();

    void  testSignal();              //信号

private:
    static void appendMessageBody(QQmlListProperty<Messagebody>*, Messagebody*);
    static int MessageBodyCount(QQmlListProperty<Messagebody>*);
    static Messagebody* MessageBody(QQmlListProperty<Messagebody>*, int);
    static void clearMessageBody(QQmlListProperty<Messagebody>*);
    static void replaceMessageBody(QQmlListProperty<Messagebody>*, int, Messagebody*);
    static void removeLastMessageBody(QQmlListProperty<Messagebody>*);

    QString mname="Kingdom";
    Messagebody* m_curMessageBody = nullptr;
    QList<Messagebody*> m_bodyList;
    MessageAuthor *mauthor= nullptr;
};

#endif // MESSAGE_H

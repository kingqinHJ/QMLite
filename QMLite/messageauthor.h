#ifndef MESSAGEAUTHOR_H
#define MESSAGEAUTHOR_H

#include <QObject>

//分组属性
class MessageAuthor : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString name READ name WRITE setName)
    Q_PROPERTY(QString email READ email WRITE setEmail)

public:
    explicit MessageAuthor(QObject *parent = nullptr);

    Q_INVOKABLE QString name(){
        return mname;
    }

    Q_INVOKABLE void setName(QString name){
        mname=name;
    }

    Q_INVOKABLE QString email(){
        return mmail;
    }

    Q_INVOKABLE void setEmail(QString mail){
        mname=mail;
    }

private:
    QString mname;
    QString mmail;
signals:
};

#endif // MESSAGEAUTHOR_H

#ifndef MESSAGEBODY_H
#define MESSAGEBODY_H

#include <QObject>
#include<QDebug>

//对象类型的属性
class Messagebody : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString content READ content WRITE setContent NOTIFY contentChanged FINAL)
public:
    explicit Messagebody(QObject *parent = nullptr){

    }

    // Add properties and methods for Messagebody here

    Q_INVOKABLE QString content() { return m_content; }
    Q_INVOKABLE void setContent(QString content) {
        if (m_content != content) {
            m_content = content;
            emit contentChanged();
            qDebug()<<content;
        }
    }

signals:
        void contentChanged();

private:
    QString m_content;
};

#endif // MESSAGEBODY_H

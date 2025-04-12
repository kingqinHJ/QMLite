#ifndef MESSAGEBOARDATTACHTYPE_H
#define MESSAGEBOARDATTACHTYPE_H

#include <QObject>
#include <QtQml/qqml.h>
//提供附加对象注解数据
class MessageBoardAttachType : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool expire READ expire WRITE setExpire NOTIFY expireChange)
    QML_ANONYMOUS
public:
    explicit MessageBoardAttachType(QObject *parent = nullptr);

    bool expire() const{
        return mexpire;
    }
    void setExpire(bool expire){
        if (mexpire != expire) {
            mexpire = expire;
            emit expireChange();
        }
    }

signals:
    void expireChange();

private:
    bool mexpire=false;
};

class MessageBoard : public QObject
{
    Q_OBJECT
    QML_ELEMENT
public:
    MessageBoard(QObject *parent = nullptr)
    {

    }

    static MessageBoardAttachType *qmlAttachedProperties(QObject * obj){
         return new MessageBoardAttachType(obj);
    }
};

QML_DECLARE_TYPEINFO(MessageBoard, QML_HAS_ATTACHED_PROPERTIES)
#endif // MESSAGEBOARDATTACHTYPE_H

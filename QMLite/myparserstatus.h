#ifndef MYPARSERSTATUS_H
#define MYPARSERSTATUS_H

#include <QObject>
#include <QQmlParserStatus>
#include <qDebug>
#include <QTimer>

/*QQmlParserStatus 是一个用于 QML 的接口类，主要功能是为 QML 组件提供解析和初始化时的生命周期管理。它允许自定义组件在 QML 解析器解析和创建对象时执行特定的初始化逻辑。主要方法包括：
componentComplete()：在 QML 组件解析完成、所有属性绑定设置完毕后调用，用于执行初始化操作。
classBegin()：在组件实例化开始时调用，通常用于准备工作。
通过实现这个接口，开发者可以在 QML 组件的解析过程中插入自定义行为，适合需要动态初始化或复杂逻辑的场景。
*/

class MyParserStatus  : public QObject , public QQmlParserStatus
{
    Q_OBJECT
    Q_INTERFACES(QQmlParserStatus)
    Q_PROPERTY(QString result READ result NOTIFY operationFinished)

public:
    MyParserStatus(QObject *parent = 0)
    {

    }

    //在类创建之后，但在设置任何属性之前调用。
    void classBegin()
    {
        qDebug()<<"classBegin()";

        // QTimer::singleShot(2000, this, [this]() {
        //     m_result = "昂贵操作已完成！";
        //     qDebug() << "Expensive operation finished, result:" << m_result;
        //     emit operationFinished();
        // });
    }

    //在导致此实例化的根组件完成构造后调用。此时，所有静态值和绑定值都已分配给类。
    void componentComplete()
    {
        qDebug()<<"componentComplete(),start complex function";

        QTimer::singleShot(2000, this, [this]() {
            m_result = "昂贵操作已完成！";
            qDebug() << "Expensive operation finished, result:" << m_result;
            emit operationFinished();
        });
    }

    QString result() const { return m_result; }

signals:
    // 当昂贵操作完成后发出信号，可以让 QML 界面响应变化
    void operationFinished();

private:
    QString m_result;
};

#endif // MYPARSERSTATUS_H

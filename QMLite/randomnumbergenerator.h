#ifndef RANDOMNUMBERGENERATOR_H
#define RANDOMNUMBERGENERATOR_H

#include <QObject>
#include <QQmlEngine>
#include <QQmlPropertyValueSource>
#include <QTimer>
#include <QQmlProperty>
#include <QRandomGenerator>

class RandomNumberGenerator : public QObject, public QQmlPropertyValueSource
{
    Q_OBJECT
    Q_INTERFACES(QQmlPropertyValueSource)
    Q_PROPERTY(int maxValue READ maxValue WRITE setMaxValue NOTIFY maxValueChanged);
    QML_ELEMENT
public:
    // 修改这里：给 parent 添加默认值 nullptr
    // 最好也加上 explicit 关键字
    explicit RandomNumberGenerator(QObject *parent = nullptr)
        : QObject(parent), m_maxValue(100)
    {
        QObject::connect(&m_timer, SIGNAL(timeout()), SLOT(updateProperty()));
        m_timer.start(500);
    }

    int maxValue() const
    {
        return m_maxValue;
    }
    void setMaxValue(int maxValue)
    {
        m_maxValue=maxValue;
    }

    virtual void setTarget(const QQmlProperty &prop) { m_targetProperty = prop; }

signals:
    void maxValueChanged();

private slots:
    void updateProperty() {
        int a=QRandomGenerator::global()->bounded(m_maxValue);
        qDebug()<<a;
        m_targetProperty.write(a);
    }

private:
    QQmlProperty m_targetProperty;
    QTimer m_timer;
    int m_maxValue;
};

#endif // RANDOMNUMBERGENERATOR_H

#pragma once

#include <QObject>
#include <QString>

class GtpoTest : public QObject
{
    Q_OBJECT
public:
    explicit GtpoTest(QObject* parent = nullptr);

    Q_INVOKABLE QString runPhase1Test() const;
};

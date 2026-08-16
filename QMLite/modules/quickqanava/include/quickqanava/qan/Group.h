#pragma once

// ============================================================================
// Group.h — qan::Group 精简参考实现
// ============================================================================
//
// 组 = 普通节点 + is_group=true。可以容纳子节点。

#include<QObject>
#include<QQuickItem>
#include<QQmlComponent>
#include <quickqanava/qan/Node.h>

namespace qan {

class Group : public Node
{
    Q_OBJECT
    QML_ELEMENT

public:
    explicit Group( QObject* parent = nullptr );
    virtual ~Group() override=default;
    Group( const Group& other ) = delete;

    //组内节点
    Q_INVOKABLE bool hasNode( const Node* node ) const;

    //组可视化项
    GroupItem* getGroupItem() const;
    const GroupItem* getGroupItem() const noexcept;
    void setItem( GroupItem* item ) noexcept override;

    static QQmlComponent* delegate(QQmlEngine &engine,QObject *parent = nullptr)noexcept;
    static class NodeStyle* style(QObject *parent = nullptr) noexcept;
};

} // ::qan

QML_DECLARE_TYPE(qan::Group)

#pragma once

// ============================================================================
// Group.h — qan::Group 精简参考实现
// ============================================================================
//
// 组 = 普通节点 + is_group=true。可以容纳子节点。

#include <QObject>
#include <QQmlEngine>
#include <QQuickItem>

#include "Node.h"

namespace qan {

class GroupItem;

class Group : public Node
{
    Q_OBJECT
    QML_ELEMENT

public:
    explicit Group(QObject* parent = nullptr);
    virtual ~Group() override = default;
    Group(const Group&) = delete;

    // ── 组内节点 ──
    Q_INVOKABLE bool hasNode(const Node* node) const;

    // ── 组可视化项 ──
    GroupItem*        getGroupItem() noexcept;
    const GroupItem*  getGroupItem() const noexcept;
    void              setItem(NodeItem* item) noexcept override;

    // ── 静态工厂 ──
    static QQmlComponent*  delegate(QQmlEngine& engine, QObject* parent = nullptr) noexcept;
    static class NodeStyle* style(QObject* parent = nullptr) noexcept;
};

} // ::qan

QML_DECLARE_TYPE(qan::Group)

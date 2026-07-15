#include "Group.h"
#include "NodeItem.h"

#include <QQmlEngine>

namespace qan {

// ============================================================================
// Group 构造 — 设置 is_group=true
// ============================================================================

Group::Group(QObject* parent)
    : Node{parent}
{
    // 关键：标记为组，使其可以容纳子节点
    this->set_is_group(true);
}

// ── 组内节点查詢 ──

bool Group::hasNode(const Node* node) const
{
    return has_node(node);
}

// ── 可视化项 ──

GroupItem* Group::getGroupItem() noexcept
{
    return qobject_cast<GroupItem*>(_item.data());
}

const GroupItem* Group::getGroupItem() const noexcept
{
    return qobject_cast<const GroupItem*>(_item.data());
}

void Group::setItem(NodeItem* item) noexcept
{
    Node::setItem(item);
    // GroupItem 特有的初始化可在此处进行
}

// ── 静态工厂 ──

QQmlComponent* Group::delegate(QQmlEngine& engine, QObject* parent) noexcept
{
    Q_UNUSED(engine);
    Q_UNUSED(parent);
    return nullptr;
}

NodeStyle* Group::style(QObject* parent) noexcept
{
    Q_UNUSED(parent);
    return nullptr;
}

} // ::qan

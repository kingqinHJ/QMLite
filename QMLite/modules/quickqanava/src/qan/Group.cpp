// ============================================================================
// Group.cpp — qan::Group 实现
// ============================================================================

#include <quickqanava/qan/Group.h>

namespace qan { // ::qan

Group::Group(QObject* parent)
    : Node{parent}
{
    // 关键：标记为组，使其可以容纳子节点
    this->set_is_group(true);
}

// ── 组内节点查询 ──
bool Group::hasNode(const Node* node) const
{
    return has_node(node);
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

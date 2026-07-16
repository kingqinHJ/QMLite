#include "Node.h"
#include "Graph.h"
#include "NodeItem.h"

namespace qan {

// ============================================================================
// Node 构造/析构
// ============================================================================

Node::Node(QObject* parent)
    : super_t{parent}
{
}

// ── 图访问 ──

Graph* Node::getGraph() noexcept
{
    return get_graph();
}

const Graph* Node::getGraph() const noexcept
{
    return get_graph();
}

// ── 可视化项 ──

NodeItem* Node::getItem() noexcept
{
    return _item;
}

const NodeItem* Node::getItem() const noexcept
{
    return _item;
}

void Node::setItem(NodeItem* item) noexcept
{
    _item = item;
    if (item)
        item->setNode(this);
}

// ── 标签 ──

bool Node::setLabel(const QString& label)
{
    if (_label != label) {
        _label = label;
        emit labelChanged();
        return true;
    }
    return false;
}

// ── 锁定/保护 ──

bool Node::setLocked(bool locked)
{
    if (_locked != locked) {
        _locked = locked;
        emit lockedChanged();
        return true;
    }
    return false;
}

bool Node::setIsProtected(bool isProtected)
{
    if (_isProtected != isProtected) {
        _isProtected = isProtected;
        emit isProtectedChanged();
        return true;
    }
    return false;
}

// ── 度 ──

int Node::getInDegree() const
{
    return static_cast<int>(get_in_degree());
}

int Node::getOutDegree() const
{
    return static_cast<int>(get_out_degree());
}

// ── 静态工厂 ──

QQmlComponent* Node::delegate(QQmlEngine& engine, QObject* parent) noexcept
{
    Q_UNUSED(engine);
    Q_UNUSED(parent);
    // Phase 3+: 返回默认 QML 节点组件
    return nullptr;
}

NodeStyle* Node::style(QObject* parent) noexcept
{
    Q_UNUSED(parent);
    // Phase 3+: 返回默认节点样式
    return nullptr;
}

} // ::qan

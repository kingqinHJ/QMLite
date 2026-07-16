#include "Edge.h"
#include "Graph.h"
#include "Node.h"
#include "EdgeItem.h"

namespace qan {

// ============================================================================
// Edge 构造
// ============================================================================

Edge::Edge(QObject* parent)
    : super_t{parent}
{
}

// ── 图访问 ──

Graph* Edge::getGraph() noexcept
{
    return get_graph();
}

const Graph* Edge::getGraph() const noexcept
{
    return get_graph();
}

// ── 可视化项 ──

EdgeItem* Edge::getItem() noexcept
{
    return _item;
}

const EdgeItem* Edge::getItem() const noexcept
{
    return _item;
}

void Edge::setItem(EdgeItem* item) noexcept
{
    _item = item;
    if (item)
        item->setEdge(this);
}

// ── 标签 ──

bool Edge::setLabel(const QString& label)
{
    if (_label != label) {
        _label = label;
        emit labelChanged();
        return true;
    }
    return false;
}

// ── 权重 ──

bool Edge::setWeight(qreal weight)
{
    if (!qFuzzyCompare(_weight, weight)) {
        _weight = weight;
        emit weightChanged();
        return true;
    }
    return false;
}

// ── 锁定 ──

bool Edge::setLocked(bool locked)
{
    if (_locked != locked) {
        _locked = locked;
        emit lockedChanged();
        return true;
    }
    return false;
}

// ── 静态工厂 ──

QQmlComponent* Edge::delegate(QQmlEngine& engine, QObject* parent) noexcept
{
    Q_UNUSED(engine);
    Q_UNUSED(parent);
    // Phase 3+: 返回默认 QML 边组件
    return nullptr;
}

EdgeStyle* Edge::style(QObject* parent) noexcept
{
    Q_UNUSED(parent);
    // Phase 3+: 返回默认边样式
    return nullptr;
}

} // ::qan

#include "Graph.h"
#include "Node.h"
#include "Edge.h"
#include "Group.h"
#include "NodeItem.h"
#include "EdgeItem.h"

#include <QQmlEngine>
#include <QQmlContext>
#include <QQuickItem>

namespace qan {

// ============================================================================
// Graph 构造/析构
// ============================================================================

Graph::Graph(QQuickItem* parent) noexcept
    : QQuickItem{parent}
{
    // containerItem 默认指向自己，因此所有图形项默认成为 graph 的子项
    _containerItem = this;
}

Graph::~Graph()
{
    // gtpo::graph::~graph() 自动 delete 所有 node/edge
}

// ── QML 解析生命周期 ──

void Graph::classBegin()
{
    // QML 组件开始解析，什么都不做
}

void Graph::componentComplete()
{
    // QML 组件解析完毕，此处可初始化委托组件
}

// ── 容器图形项 ──

void Graph::setContainerItem(QQuickItem* item)
{
    if (_containerItem != item) {
        _containerItem = item;
        emit containerItemChanged();
    }
}

// ── 委托组件 ──

void Graph::setNodeDelegate(QQmlComponent* comp) noexcept
{
    _nodeDelegate.reset(comp);
    emit nodeDelegateChanged();
}

void Graph::setEdgeDelegate(QQmlComponent* comp) noexcept
{
    _edgeDelegate.reset(comp);
    emit edgeDelegateChanged();
}

void Graph::setGroupDelegate(QQmlComponent* comp) noexcept
{
    _groupDelegate.reset(comp);
    emit groupDelegateChanged();
}

void Graph::setSelectionDelegate(QQmlComponent* comp) noexcept
{
    _selectionDelegate.reset(comp);
    emit selectionDelegateChanged();
}

// ── QML 组件创建 ──

std::unique_ptr<QQmlComponent> Graph::createComponent(const QString& url)
{
    QQmlEngine* engine = qmlEngine(this);
    if (!engine)
        return nullptr;
    auto comp = std::make_unique<QQmlComponent>(engine, url, this);
    if (comp->isError()) {
        qWarning() << comp->errorString();
        return nullptr;
    }
    return comp;
}

QQuickItem* Graph::createItemFromComponent(QQmlComponent* component)
{
    if (!component)
        return nullptr;
    QQmlEngine* engine = qmlEngine(this);
    if (!engine)
        return nullptr;

    QQmlContext* ctx = QQmlEngine::contextForObject(this);
    QObject* obj = component->beginCreate(ctx ? ctx : engine->rootContext());
    if (!obj) {
        qWarning() << component->errorString();
        return nullptr;
    }

    QQuickItem* item = qobject_cast<QQuickItem*>(obj);
    if (!item) {
        delete obj;
        return nullptr;
    }

    // 设置父项（containerItem），纳入 Qt 父子树管理
    QQuickItem* container = getContainerItem();
    item->setParentItem(container ? container : this);

    component->completeCreate();
    return item;
}

QQuickItem* Graph::createFromComponent(QQmlComponent* component,
                                        Node*  node,
                                        Edge*  edge,
                                        Group* group) noexcept
{
    QQuickItem* item = createItemFromComponent(component);
    if (!item)
        return nullptr;

    // 根据类型绑定数据模型
    if (node) {
        NodeItem* nodeItem = qobject_cast<NodeItem*>(item);
        if (nodeItem)
            node->setItem(nodeItem);
    }
    if (edge) {
        EdgeItem* edgeItem = qobject_cast<EdgeItem*>(item);
        if (edgeItem)
            edge->setItem(edgeItem);
    }
    if (group) {
        NodeItem* groupItem = qobject_cast<NodeItem*>(item);
        if (groupItem)
            group->setItem(groupItem);
    }

    return item;
}

// ── 清空图 ──

void Graph::clearGraph() noexcept
{
    // 利用 gtpo::graph::clear() 销毁所有数据模型
    // 对应的 QQuickItem 由 Qt 父子树自动删除
    clear();
}

} // ::qan

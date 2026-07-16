#pragma once

#include <QObject>
#include <quickqanava/gtpo/node.h>
#include <QString>
#include <QPointer>
#include <QQmlEngine>
#include <QQuickItem>

namespace qan {

class Graph;
class Edge;
class Group;
class NodeItem;

// Phase 2+：qan::Node 继承 gtpo::node，把 node_base_t 设为 QObject
//
// TODO:
// - class Node : public gtpo::node<QObject, qan::Graph, qan::Node, qan::Edge, qan::Group>
// - Q_OBJECT
// - 属性：label, locked, isProtected
// - 持有 _item 指针（NodeItem）
// - 静态工厂 delegate() / style()

class Node : public QObject, public gtpo::node<QObject, Graph, Node, Edge, Group>
{
    Q_OBJECT
    QML_ELEMENT
public:
    using super_t = gtpo::node<QObject, Graph, Node, Edge, Group>;
    explicit Node(QObject* parent = nullptr);
    virtual ~Node();
    Node(const Node& other)=delete;

    Q_PROPERTY(Graph* graph READ getGraph CONSTANT FINAL)
    const Graph* getGraph() const noexcept;
    Graph* getGraph() noexcept;

    Q_PROPERTY(NodeItem* item READ getItem CONSTANT)
    NodeItem* getItem() const noexcept;
    const NodeItem* getItem() const noexcept;
    virtual void setItem(NodeItem* item)noexcept;
};

} // ::qan

QML_DECLARE_TYPE(qan::Node)

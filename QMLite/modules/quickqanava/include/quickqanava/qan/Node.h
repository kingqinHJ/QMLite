#pragma once

#include <QObject>
#include <quickqanava/gtpo/node.h>

namespace qan {

class Graph;
class Edge;
class Group;

// Phase 2+：qan::Node 继承 gtpo::node，把 node_base_t 设为 QObject
//
// TODO:
// - class Node : public gtpo::node<QObject, qan::Graph, qan::Node, qan::Edge, qan::Group>
// - Q_OBJECT
// - 属性：label, locked, isProtected
// - 持有 _item 指针（NodeItem）
// - 静态工厂 delegate() / style()

class Node : public QObject
{
    Q_OBJECT
    // TODO: Phase 2 开始实现
};

} // ::qan

QML_DECLARE_TYPE(qan::Node)

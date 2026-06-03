#pragma once

#include <QObject>
#include <quickqanava/gtpo/edge.h>

namespace qan {

class Graph;
class Node;

// Phase 2+：qan::Edge 继承 gtpo::edge，把 edge_base_t 设为 QObject
//
// TODO:
// - class Edge : public gtpo::edge<QObject, qan::Graph, qan::Node>
// - Q_OBJECT
// - 属性：label, weight, locked, isProtected
// - 持有 _item 指针（EdgeItem）

class Edge : public QObject
{
    Q_OBJECT
    // TODO: Phase 2 开始实现
};

} // ::qan

QML_DECLARE_TYPE(qan::Edge)

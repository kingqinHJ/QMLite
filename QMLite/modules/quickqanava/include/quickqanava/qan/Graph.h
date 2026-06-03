#pragma once

#include <QQuickItem>
#include <quickqanava/gtpo/graph.h>

// 前向声明
namespace qan {
class Node;
class Edge;
class Group;
}

namespace qan {

// Phase 2+：qan::Graph 继承 gtpo::graph，把 graph_base_t 设为 QQuickItem
//
// TODO:
// - class Graph : public gtpo::graph<QQuickItem, qan::Node, qan::Group, qan::Edge>
// - Q_OBJECT, QML_ELEMENT
// - 工厂方法 insertNode()：创建 Node + 创建 NodeItem（通过 QML delegate）
// - 管理 nodeDelegate / edgeDelegate / groupDelegate（QQmlComponent）
// - 信号：nodeInserted, nodeRemoved, edgeInserted, edgeRemoved...

class Graph : public QQuickItem
{
    Q_OBJECT
    // TODO: Phase 2 开始实现
};

} // ::qan

#pragma once

#include <QQuickItem>

namespace qan {

class Node;
class Graph;

// Phase 2+：可视化的节点图形项
//
// TODO:
// - 继承 QQuickItem
// - QML_ELEMENT
// - 属性：x, y, width, height, minimumSize
// - 鼠标事件：拖拽移动
// - 样式属性：style（颜色、边框等）
// - 指向 qan::Node 的 _node 指针

class NodeItem : public QQuickItem
{
    Q_OBJECT
    // TODO: Phase 2 开始实现
};

} // ::qan

QML_DECLARE_TYPE(qan::NodeItem)

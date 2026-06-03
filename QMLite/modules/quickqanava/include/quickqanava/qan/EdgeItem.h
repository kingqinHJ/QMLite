#pragma once

#include <QQuickItem>

namespace qan {

class Edge;
class Graph;
class NodeItem;

// Phase 2+：可视化的边图形项
//
// TODO:
// - 继承 QQuickItem
// - QML_ELEMENT
// - 属性：p1, p2（端点）, c1, c2（贝塞尔控制点）
// - 箭头几何：srcA1/A2/A3, dstA1/A2/A3
// - updateItem()：根据 sourceItem/destinationItem 位置重计算几何
// - 指向 qan::Edge 的 _edge 指针

class EdgeItem : public QQuickItem
{
    Q_OBJECT
    // TODO: Phase 2 开始实现
};

} // ::qan

QML_DECLARE_TYPE(qan::EdgeItem)

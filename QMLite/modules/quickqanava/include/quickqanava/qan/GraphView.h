#pragma once

#include <QQuickItem>

namespace qan {

class Graph;

// Phase 3：视图容器，支持缩放/平移
//
// TODO:
// - 继承 QQuickItem（或 Flickable）
// - QML_ELEMENT
// - 属性：graph（指向 qan::Graph）
// - 鼠标事件：滚轮缩放、拖拽平移
// - 内部维护一个变换矩阵（scale + translate）

class GraphView : public QQuickItem
{
    Q_OBJECT
    // TODO: Phase 3 开始实现
};

} // ::qan

QML_DECLARE_TYPE(qan::GraphView)

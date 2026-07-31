#pragma once

// ============================================================================
// GraphView.h — 图视图容器（Phase 3 缩放/平移）
// ============================================================================
//
// 职责：包裹 Graph，提供缩放（滚轮）和平移（鼠标中键/手势）。
// Phase 2 可跳过，直接在 QML 中放 Graph 组件即可。
// ============================================================================

#include <QQuickItem>
#include <QPointer>

namespace qan {

class Graph;

class GraphView : public QQuickItem
{
    Q_OBJECT
    QML_ELEMENT

public:
    explicit GraphView( QQuickItem* parent = nullptr )
            : QQuickItem( parent ){}

    //图绑定
    Q_PROPERTY( Graph* graph READ getGraph WRITE setGraph NOTIFY graphChanged )
    Graph* getGraph() noexcept{ return _graph; }
    const Graph* getGraph() const noexcept{ return _graph; }
    void setGraph(Graph* graph) {
        if (_graph != graph) {
            _graph = graph;
            emit graphChanged();
        }
    }

signals:
    void graphChanged();

protected:
    QPointer<Graph> _graph;
};

} // ::qan

QML_DECLARE_TYPE(qan::GraphView)

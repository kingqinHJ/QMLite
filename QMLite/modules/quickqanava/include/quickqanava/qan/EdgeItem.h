#pragma once

// ============================================================================
// EdgeItem.h — qan::EdgeItem 精简参考实现
// ============================================================================
//
// 可视化边：在两个 NodeItem 之间画一条直线。
//
// 绘制在 updatePaintNode() 中（QSG），不使用子 QML Item。
// 注意：所有 Q_PROPERTY/getter/setter 必须在 public 区域（MOC + 外部调用），
//      信号集中一个段，成员集中 private 段。
// ============================================================================

#include <QQuickItem>
#include <QPointer>
#include <QColor>
#include <QPointF>
#include <QPolygonF>
#include <quickqanava/qan/Edge.h>
#include <quickqanava/qan/NodeItem.h>

namespace qan {

class Graph;

class EdgeItem : public QQuickItem
{
    Q_OBJECT
    QML_ELEMENT

public:
    explicit EdgeItem(QQuickItem* parent = nullptr);
    virtual ~EdgeItem() override = default;
    EdgeItem(const EdgeItem&) = delete;

    // ── 关联的边数据 ──
    Q_PROPERTY(Edge* edge READ getEdge CONSTANT)
    Edge* getEdge() noexcept { return _edge; }
    void setEdge(Edge* edge) noexcept { _edge = edge; }

    // ── 端点图形项 ──
    Q_PROPERTY(NodeItem* sourceItem READ getSourceItem WRITE setSourceItem NOTIFY sourceItemChanged FINAL)
    NodeItem* getSourceItem() noexcept { return _sourceItem; }
    void setSourceItem(NodeItem* item);

    Q_PROPERTY(NodeItem* destinationItem READ getDestinationItem WRITE setDestinationItem NOTIFY destinationItemChanged FINAL)
    NodeItem* getDestinationItem() const noexcept { return _destinationItem; }
    void setDestinationItem(NodeItem* item);

    // ── 线类型 ──
    enum class LineType {
        Straight = 1,
        Curved = 2
    };
    Q_PROPERTY(LineType lineType READ getLineType WRITE setLineType NOTIFY lineTypeChanged FINAL)
    LineType getLineType() const { return _lineType; }
    void setLineType(LineType type) { _lineType = type; updateItem(); }

    // ── 线颜色 ──
    Q_PROPERTY(QColor lineColor READ getLineColor WRITE setLineColor NOTIFY lineColorChanged FINAL)
    QColor getLineColor() const { return _lineColor; }
    void setLineColor(const QColor& color) { _lineColor = color; update(); }

    // ── 线宽 ──
    Q_PROPERTY(qreal lineWidth READ getLineWidth WRITE setLineWidth NOTIFY lineWidthChanged FINAL)
    qreal getLineWidth() const { return _lineWidth; }
    void setLineWidth(qreal width) { _lineWidth = width; update(); }

    // ── 虚线 ──
    Q_PROPERTY(bool dashed READ getDashed WRITE setDashed NOTIFY dashedChanged FINAL)
    bool getDashed() const { return _dashed; }
    void setDashed(bool dashed) { _dashed = dashed; update(); }

    // ── 箭头大小 ──
    Q_PROPERTY(qreal arrowSize READ getArrowSize WRITE setArrowSize NOTIFY arrowSizeChanged FINAL)
    qreal getArrowSize() const { return _arrowSize; }
    void setArrowSize(qreal size) { _arrowSize = size; update(); }

    // ── 端点坐标 ──
    Q_PROPERTY(QPointF p1 READ getP1 WRITE setP1 NOTIFY p1Changed FINAL)
    QPointF getP1() const { return _p1; }
    void setP1(const QPointF& p) { _p1 = p; update(); }

    Q_PROPERTY(QPointF p2 READ getP2 WRITE setP2 NOTIFY p2Changed FINAL)
    QPointF getP2() const { return _p2; }
    void setP2(const QPointF& p) { _p2 = p; update(); }

    // ── 几何更新（节点移动后调用）──
public slots:
    void updateItem();

signals:
    void sourceItemChanged();
    void destinationItemChanged();
    void lineTypeChanged();
    void lineColorChanged();
    void lineWidthChanged();
    void dashedChanged();
    void arrowSizeChanged();
    void p1Changed();
    void p2Changed();

protected:
    QSGNode* updatePaintNode(QSGNode* node, UpdatePaintNodeData* data) override;
    void geometryChanged(const QRectF& newGeometry, const QRectF& oldGeometry) override;

private:
    QPointer<Edge> _edge;
    QPointer<NodeItem> _sourceItem;
    QPointer<NodeItem> _destinationItem;
    LineType _lineType = LineType::Straight;
    QColor _lineColor{ "black" };
    qreal _lineWidth = 3.0;
    bool _dashed = false;
    qreal _arrowSize = 6.0;
    QPointF _p1;
    QPointF _p2;
};

} // ::qan

QML_DECLARE_TYPE(qan::EdgeItem)

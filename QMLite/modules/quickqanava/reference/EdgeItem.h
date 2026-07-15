#pragma once

// ============================================================================
// EdgeItem.h — qan::EdgeItem 精简参考实现
// ============================================================================
//
// 可视化边：在两个 NodeItem 之间画一条线。
//
// 核心功能：
//   1) 持有 sourceItem / destinationItem 两个 NodeItem 引用
//   2) 调用 updateItem() 根据两端坐标重新计算线几何
//   3) 支持直线和贝塞尔曲线（lineType 切换）
//   4) 可以绘制箭头（dstShape / srcShape）
//
// 属性：
//   - sourceItem / destinationItem : 端点对应的 NodeItem
//   - lineType : Straight / Curved
//   - lineWidth / lineColor / dashed
//   - dstShape / srcShape : 箭头形状（Arrow / Circle / None 等）
//
// 绘制在 updatePaintNode() 中（QSG），不使用子 QML Item。
// ============================================================================

#include <QQuickItem>
#include <QPointer>
#include <QColor>
#include <QPointF>
#include <QPolygonF>

namespace qan {

class Edge;
class NodeItem;

class EdgeItem : public QQuickItem
{
    Q_OBJECT
    QML_ELEMENT

public:
    explicit EdgeItem(QQuickItem* parent = nullptr);
    virtual ~EdgeItem() override = default;
    EdgeItem(const EdgeItem&) = delete;

    // ── 关联的边数据模型 ──
    Q_PROPERTY(Edge* edge READ getEdge CONSTANT)
    Edge*  getEdge() noexcept { return _edge; }
    void   setEdge(Edge* edge) noexcept { _edge = edge; }
private:
    QPointer<Edge> _edge;

    // ── 端点图形项 ──
    Q_PROPERTY(NodeItem* sourceItem READ getSourceItem
               WRITE setSourceItem NOTIFY sourceItemChanged FINAL)
    NodeItem* getSourceItem() const { return _sourceItem; }
    void      setSourceItem(NodeItem* item);
signals:
    void      sourceItemChanged();
private:
    QPointer<NodeItem> _sourceItem;

    Q_PROPERTY(NodeItem* destinationItem READ getDestinationItem
               WRITE setDestinationItem NOTIFY destinationItemChanged FINAL)
    NodeItem* getDestinationItem() const { return _destinationItem; }
    void      setDestinationItem(NodeItem* item);
signals:
    void      destinationItemChanged();
private:
    QPointer<NodeItem> _destinationItem;

    // ── 线类型 ──
    enum class LineType { Straight = 1, Curved = 2 };
    Q_ENUM(LineType)

    Q_PROPERTY(LineType lineType READ getLineType
               WRITE setLineType NOTIFY lineTypeChanged FINAL)
    LineType getLineType() const { return _lineType; }
    void     setLineType(LineType t) { _lineType = t; updateItem(); }
signals:
    void     lineTypeChanged();
private:
    LineType _lineType = LineType::Straight;

    // ── 线属性 ──
    Q_PROPERTY(QColor lineColor READ getLineColor WRITE setLineColor NOTIFY lineColorChanged FINAL)
    QColor  getLineColor() const { return _lineColor; }
    void    setLineColor(QColor c) { _lineColor = c; update(); }
signals:
    void    lineColorChanged();
private:
    QColor  _lineColor{ "black" };

    Q_PROPERTY(qreal lineWidth READ getLineWidth WRITE setLineWidth NOTIFY lineWidthChanged FINAL)
    qreal   getLineWidth() const { return _lineWidth; }
    void    setLineWidth(qreal w) { _lineWidth = w; update(); }
signals:
    void    lineWidthChanged();
private:
    qreal   _lineWidth = 3.0;

    Q_PROPERTY(bool dashed READ getDashed WRITE setDashed NOTIFY dashedChanged FINAL)
    bool    getDashed() const { return _dashed; }
    void    setDashed(bool d) { _dashed = d; update(); }
signals:
    void    dashedChanged();
private:
    bool    _dashed = false;

    // ── 箭头 ──
    Q_PROPERTY(qreal arrowSize READ getArrowSize WRITE setArrowSize NOTIFY arrowSizeChanged FINAL)
    qreal   getArrowSize() const { return _arrowSize; }
    void    setArrowSize(qreal s) { _arrowSize = s; update(); }
signals:
    void    arrowSizeChanged();
private:
    qreal   _arrowSize = 6.0;

    // ── 端点坐标 P1/P2（内部计算，从 sourceItem/destinationItem 推导）──
    Q_PROPERTY(QPointF p1 READ getP1 NOTIFY p1Changed FINAL)
    QPointF  getP1() const { return _p1; }
    void     setP1(QPointF p) { _p1 = p; }
signals:
    void     p1Changed();
private:
    QPointF  _p1;

    Q_PROPERTY(QPointF p2 READ getP2 NOTIFY p2Changed FINAL)
    QPointF  getP2() const { return _p2; }
    void     setP2(QPointF p) { _p2 = p; }
signals:
    void     p2Changed();
private:
    QPointF  _p2;

    // ── 更新几何 ──
public slots:
    //! 根据 sourceItem/destinationItem 的位置重新计算 P1/P2 和箭头几何
    void updateItem();

    // ── 绘制 ──
protected:
    QSGNode* updatePaintNode(QSGNode* oldNode, UpdatePaintNodeData* data) override;
    void     geometryChanged(const QRectF& newGeometry, const QRectF& oldGeometry) override;
};

} // ::qan

QML_DECLARE_TYPE(qan::EdgeItem)

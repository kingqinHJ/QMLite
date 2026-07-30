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
    QML_ELEMENT
public:
    explicit EdgeItem( QObject* parent /*= nullptr*/ );
    virtual ~EdgeItem() override= default;
    EdgeItem( const EdgeItem& ) = delete;

    //关联的边数据数据
    Q_PROPERTY(Edge* edge READ getEdge CONSTANT)
    Edge* getEdge() noexcept{return _edge;}
    void setEdge( Edge* edge ) noexcept{_edge = edge;}
private:
    QPointer<Edge> _edge;

    //端点图形项
    Q_PROPERTY(NodeItem* sourceItem READ getSourceItem WRITE setSourceItem NOTIFY sourceItemChanged FINAL)
    NodeItem* getSourceItem() noexcept{return _sourceItem;}
    void setSourceItem( NodeItem* item );
signals:
    void sourceItemChanged();
private:
    QPointer<NodeItem> _sourceItem;

    Q_PROPERTY(NodeItem* destinationItem READ getDestinationItem WRITE setDestinationItem NOTIFY destinationItemChanged FINAL)
    NodeItem* getDestinationItem() const noexcept{return _destinationItem;}
    void setDestinationItem( NodeItem* item );
signals:
    void destinationItemChanged();
private:
    QPointer<NodeItem> _destinationItem;

    //线类型
    enum LineType {
        Straight=1,
        Curved=2
    };
    Q_PROPERTY(LineType lineType READ getLineType WRITE setLineType NOTIFY lineTypeChanged FINAL)
    LineType getLineType() const {return _lineType;}
    void setLineType( LineType type ) {_lineType = type;updateItem();}
signals:
    void lineTypeChanged();
private:
    LineType _lineType = LineType::Straight;

    //线属性
    Q_PROPERTY(qreal lineColor READ getLineColor WRITE setLineColor NOTIFY lineColorChanged FINAL)
    QColor getLineColor() const {return _lineColor;}
    void setLineColor( const QColor& color ) {_lineColor = color;update();}
signals:
    void lineColorChanged();
private:
    QColor _lineColor {"black"};

    Q_PROPERTY(qreal lineWidth READ getLineWidth WRITE setLineWidth NOTIFY lineWidthChanged FINAL)
    qreal getLineWidth() const {return _lineWidth;}
    void setLineWidth( qreal width ) {_lineWidth = width;update();}
signals:
    void lineWidthChanged();
private:
    qreal _lineWidth = 3.0;

    //箭头大小
    Q_PROPERTY(qreal arrowSize READ getArrowSize WRITE setArrowSize NOTIFY arrowSizeChanged FINAL)
    qreal getArrowSize() const {return _arrowSize;}
    void setArrowSize( qreal size ) {_arrowSize = size;update();}
signals:
    void arrowSizeChanged();
private:
    qreal _arrowSize = 6.0;

    //端点坐标
    Q_PROPERTY(QPointF p1 READ getP1 WRITE setP1 NOTIFY p1Changed FINAL)
};

} // ::qan

QML_DECLARE_TYPE(qan::EdgeItem)

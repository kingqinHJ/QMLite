#include "EdgeItem.h"
#include "Edge.h"
#include "NodeItem.h"

#include <QSGSimpleRectNode>
#include <QSGNode>
#include <QLineF>
#include <QtMath>

namespace qan {

// ============================================================================
// 构造
// ============================================================================

EdgeItem::EdgeItem(QQuickItem* parent)
    : QQuickItem{parent}
{
    // 边通常不需要接收鼠标事件（Phase 3+ 可支持选择/拖拽）
    setAcceptedMouseButtons(Qt::NoButton);

    // 默认 z 值比节点低，确保节点显示在边上方
    setZ(-1);
}

// ── 端点图形项 ──

void EdgeItem::setSourceItem(NodeItem* item)
{
    if (_sourceItem != item) {
        _sourceItem = item;
        emit sourceItemChanged();
        updateItem();
    }
}

void EdgeItem::setDestinationItem(NodeItem* item)
{
    if (_destinationItem != item) {
        _destinationItem = item;
        emit destinationItemChanged();
        updateItem();
    }
}

// ── 更新几何 ──

void EdgeItem::updateItem()
{
    if (!_sourceItem || !_destinationItem)
        return;

    // 计算源和目标节点的中心点
    QPointF srcCenter = _sourceItem->mapToItem(
        this, QPointF(_sourceItem->width() / 2.0, _sourceItem->height() / 2.0));

    QPointF dstCenter = _destinationItem->mapToItem(
        this, QPointF(_destinationItem->width() / 2.0, _destinationItem->height() / 2.0));

    _p1 = srcCenter;
    _p2 = dstCenter;
    emit p1Changed();
    emit p2Changed();

    // 更新 item 自身的几何范围以容纳整条线
    prepareGeometryChange();

    QRectF rect(QPointF(qMin(_p1.x(), _p2.x()), qMin(_p1.y(), _p2.y())),
                QSizeF(qAbs(_p2.x() - _p1.x()), qAbs(_p2.y() - _p1.y())));

    // 给一些额外空间用于箭头
    rect.adjust(-_arrowSize, -_arrowSize, _arrowSize, _arrowSize);

    setX(rect.x());
    setY(rect.y());
    setWidth(qMax(rect.width(), 1.0));
    setHeight(qMax(rect.height(), 1.0));

    update();
}

// ── 绘制 ──

QSGNode* EdgeItem::updatePaintNode(QSGNode* oldNode, UpdatePaintNodeData* data)
{
    Q_UNUSED(data);

    delete oldNode;
    oldNode = nullptr;

    if (!_sourceItem || !_destinationItem)
        return nullptr;

    // ── 简单实现：绘制一条直线作为占位 ──
    // 生产代码应使用自定义 QSG 节点绘制贝塞尔曲线 + 箭头

    // 转换端点到本地坐标
    QPointF localP1 = mapFromItem(this,
        QPointF(_sourceItem->x() + _sourceItem->width() / 2.0,
                _sourceItem->y() + _sourceItem->height() / 2.0));

    QPointF localP2 = mapFromItem(this,
        QPointF(_destinationItem->x() + _destinationItem->width() / 2.0,
                _destinationItem->y() + _destinationItem->height() / 2.0));

    Q_UNUSED(localP1);
    Q_UNUSED(localP2);

    // 创建矩形来覆盖线条区域（Phase 2 阶段用 QQuickPaintedItem 或
    // QML Canvas 更好；纯 QSG 实现较复杂，这里仅作占位）

    QSGSimpleRectNode* node = new QSGSimpleRectNode();
    node->setRect(boundingRect());

    // 简单的颜色占位：使用 lineColor 填充整个包围盒
    // 真正的绘制在 Phase 3+ 通过 QSGNinePatchNode 或自定义 QSGNode 实现
    node->setColor(_lineColor);

    return node;
}

void EdgeItem::geometryChanged(const QRectF& newGeometry, const QRectF& oldGeometry)
{
    QQuickItem::geometryChanged(newGeometry, oldGeometry);
    update();
}

} // ::qan

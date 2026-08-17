// ============================================================================
// EdgeItem.cpp — qan::EdgeItem 实现（Phase 2 精简版）
// ============================================================================

#include <quickqanava/qan/EdgeItem.h>
#include <quickqanava/qan/NodeItem.h>

#include <QSGNode>
#include <QSGGeometryNode>
#include <QSGGeometry>
#include <QSGFlatColorMaterial>
#include <QtMath>

namespace qan { // ::qan

EdgeItem::EdgeItem(QQuickItem* parent)
    : QQuickItem{parent}
{
    // 边不接收鼠标事件，让事件穿透到节点
    setAcceptedMouseButtons(Qt::NoButton);
    setZ(-1);   // 边始终在节点下方

    // 关键：让场景图调用 updatePaintNode()，否则边画不出来
    setFlag(QQuickItem::ItemHasContents);
}

// ── 端点绑定 ──
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

// ── 几何更新 ──
void EdgeItem::updateItem()
{
    if (!_sourceItem || !_destinationItem)
        return;

    // 计算源和目标中心点（映射到本 item 坐标系）
    const QPointF srcCenter = _sourceItem->mapToItem(
        this, QPointF(_sourceItem->width() / 2., _sourceItem->height() / 2.));
    const QPointF dstCenter = _destinationItem->mapToItem(
        this, QPointF(_destinationItem->width() / 2., _destinationItem->height() / 2.));

    _p1 = srcCenter;
    _p2 = dstCenter;
    emit p1Changed();
    emit p2Changed();

    // 更新自身几何范围以容纳整条线（含箭头余量）
    // 注：setX/setY/setWidth/setHeight 内部会自动触发几何更新，
    //     不需要（也没有）prepareGeometryChange()（那是 Qt6 的方法）
    QRectF rect(QPointF(qMin(_p1.x(), _p2.x()), qMin(_p1.y(), _p2.y())),
                QSizeF(qAbs(_p2.x() - _p1.x()), qAbs(_p2.y() - _p1.y())));
    rect.adjust(-_arrowSize, -_arrowSize, _arrowSize, _arrowSize);

    setX(rect.x());
    setY(rect.y());
    setWidth(rect.width());
    setHeight(rect.height());

    update();
}

// ── 绘制：Phase 2 用一条直线段占位 ──
QSGNode* EdgeItem::updatePaintNode(QSGNode* oldNode, UpdatePaintNodeData* data)
{
    Q_UNUSED(data);
    delete oldNode;   // 丢弃旧场景图节点，重建

    if (!_sourceItem || !_destinationItem)
        return nullptr;

    // 端点映射到本 item 本地坐标
    const QPointF srcCenter = _sourceItem->mapToItem(
        this, QPointF(_sourceItem->width() / 2., _sourceItem->height() / 2.));
    const QPointF dstCenter = _destinationItem->mapToItem(
        this, QPointF(_destinationItem->width() / 2., _destinationItem->height() / 2.));

    // 用 QSGGeometryNode + 2 顶点画一条线
    auto* geometry = new QSGGeometry(QSGGeometry::defaultAttributes_Point2D(), 2);
    geometry->setDrawingMode(QSGGeometry::DrawLines);
    geometry->setLineWidth(static_cast<float>(_lineWidth));

    auto* vertices = geometry->vertexDataAsPoint2D();
    vertices[0].set(static_cast<float>(srcCenter.x()), static_cast<float>(srcCenter.y()));
    vertices[1].set(static_cast<float>(dstCenter.x()), static_cast<float>(dstCenter.y()));

    auto* material = new QSGFlatColorMaterial;
    material->setColor(_lineColor);

    auto* node = new QSGGeometryNode;
    node->setGeometry(geometry);
    node->setMaterial(material);
    node->setFlag(QSGNode::OwnsGeometry);
    node->setFlag(QSGNode::OwnsMaterial);

    return node;
}

void EdgeItem::geometryChanged(const QRectF& newGeometry, const QRectF& oldGeometry)
{
    QQuickItem::geometryChanged(newGeometry, oldGeometry);
    update();
}

} // ::qan

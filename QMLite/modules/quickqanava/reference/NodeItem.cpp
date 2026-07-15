#include "NodeItem.h"
#include "Node.h"
#include "NodeStyle.h"

#include <QPainter>
#include <QMouseEvent>
#include <QSGSimpleRectNode>
#include <QSGSimpleTextureNode>
#include <QSGNode>
#include <QFont>
#include <QFontMetrics>

namespace qan {

// ============================================================================
// 构造
// ============================================================================

NodeItem::NodeItem(QQuickItem* parent)
    : QQuickItem{parent}
{
    // 默认尺寸
    setWidth(120);
    setHeight(50);

    // 接收鼠标事件
    setAcceptedMouseButtons(Qt::LeftButton);
    setFlag(QQuickItem::ItemIsFocusScope);
    setFlag(QQuickItem::ItemAcceptsInputMethod);

    // 可拖拽
    setFlag(QQuickItem::ItemIsMovable, false);  // 自己实现拖拽逻辑
}

// ── 样式 ──

void NodeItem::setStyle(NodeStyle* style)
{
    _style = style;
    Q_UNUSED(_style);
    // Phase 3+: 从 style 对象读取属性并同步到自身的 Q_PROPERTY
    update();
}

// ── 拖拽 ──

void NodeItem::mousePressEvent(QMouseEvent* event)
{
    if (event->button() == Qt::LeftButton) {
        _dragStartPos = event->pos();
        _dragging = true;
        event->accept();
        // 置顶：拖拽时把节点移到最前面
        setZ(++_maxZ);
    } else {
        QQuickItem::mousePressEvent(event);
    }
}

void NodeItem::mouseMoveEvent(QMouseEvent* event)
{
    if (_dragging) {
        // 计算位移并应用
        QPointF delta = event->pos() - _dragStartPos;
        setPosition(position() + delta);
        event->accept();

        // 通知相连的 EdgeItem 更新几何
        // Phase 3+: 通过 signal 或直接遍历 edge 列表
    } else {
        QQuickItem::mouseMoveEvent(event);
    }
}

void NodeItem::mouseReleaseEvent(QMouseEvent* event)
{
    if (_dragging && event->button() == Qt::LeftButton) {
        _dragging = false;
        event->accept();
    } else {
        QQuickItem::mouseReleaseEvent(event);
    }
}

void NodeItem::mouseDoubleClickEvent(QMouseEvent* event)
{
    Q_UNUSED(event);
    // Phase 3+: 发射 nodeDoubleClicked 信号
}

// ── 绘制 ──

QSGNode* NodeItem::updatePaintNode(QSGNode* oldNode, UpdatePaintNodeData* data)
{
    Q_UNUSED(data);

    // 删除旧节点，每次重新绘制（简单但非最优；生产代码应使用 QSG 缓存）
    delete oldNode;
    oldNode = nullptr;

    // ── 创建一个简单的矩形节点作为背景 ──
    // 注意：QSGSimpleRectNode 只能画纯色矩形。
    // 更复杂的绘制（圆角、文字、边框）需要用 QSG 自定义节点
    // 或使用 QQuickPaintedItem + QPainter。

    // 背景矩形
    QSGSimpleRectNode* rectNode = new QSGSimpleRectNode();
    rectNode->setRect(boundingRect());
    rectNode->setColor(_backColor);

    return rectNode;
}

void NodeItem::geometryChanged(const QRectF& newGeometry, const QRectF& oldGeometry)
{
    QQuickItem::geometryChanged(newGeometry, oldGeometry);
    update();
}

} // ::qan

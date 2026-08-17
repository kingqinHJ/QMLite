#pragma once

// ============================================================================
// NodeItem.h — qan::NodeItem 精简参考实现
// ============================================================================
//
// 可视化节点：一个可拖拽的矩形 + 标签文字。
//
// 绘制在 QQuickItem::updatePaintNode() 中（QSG），不使用子 QML Item。
// ============================================================================

#include <QQuickItem>
#include <QPointer>
#include <QColor>
#include <QSizeF>
#include <quickqanava/qan/Node.h>
#include <quickqanava/qan/NodeStyle.h>

namespace qan {

class Graph;

class NodeItem : public QQuickItem
{
    Q_OBJECT
    QML_ELEMENT

public:
    explicit NodeItem(QQuickItem* parent = nullptr);
    virtual ~NodeItem() override = default;
    NodeItem(const NodeItem&) = delete;

    // ── 关联的节点数据模型 ──
    Q_PROPERTY(qan::Node* node READ getNode CONSTANT)
    Node* getNode() noexcept { return _node; }
    void setNode(Node* node) noexcept { _node = node; }

private:
    QPointer<Node> _node;

    // ── 背景 ──
    Q_PROPERTY(QColor backColor READ getBackColor WRITE setBackColor NOTIFY backColorChanged FINAL)
    QColor getBackColor() const noexcept { return _backColor; }
    void setBackColor(const QColor& color) noexcept { _backColor = color; update(); }
signals:
    void backColorChanged();
private:
    QColor _backColor{ "white" };

    // ── 边框 ──
    Q_PROPERTY(QColor borderColor READ getBorderColor WRITE setBorderColor NOTIFY borderColorChanged FINAL)
    QColor getBorderColor() const noexcept { return _borderColor; }
    void setBorderColor(const QColor& color) noexcept { _borderColor = color; update(); }
signals:
    void borderColorChanged();
private:
    QColor _borderColor{ "black" };

    Q_PROPERTY(qreal borderWidth READ getBorderWidth WRITE setBorderWidth NOTIFY borderWidthChanged FINAL)
    qreal getBorderWidth() const noexcept { return _borderWidth; }
    void setBorderWidth(qreal width) noexcept { _borderWidth = width; update(); }
signals:
    void borderWidthChanged();
private:
    qreal _borderWidth{ 1.0 };

    // ── 标签文字 ──
    Q_PROPERTY(QColor labelColor READ getLabelColor WRITE setLabelColor NOTIFY labelColorChanged FINAL)
    QColor getLabelColor() const noexcept { return _labelColor; }
    void setLabelColor(const QColor& color) noexcept { _labelColor = color; update(); }
signals:
    void labelColorChanged();
private:
    QColor _labelColor{ "black" };

    Q_PROPERTY(int fontSize READ getFontSize WRITE setFontSize NOTIFY fontSizeChanged FINAL)
    int getFontSize() const noexcept { return _fontSize; }
    void setFontSize(int size) noexcept { _fontSize = size; update(); }
signals:
    void fontSizeChanged();
private:
    int _fontSize{ 12 };

    Q_PROPERTY(bool fontBold READ getFontBold WRITE setFontBold NOTIFY fontBoldChanged FINAL)
    bool getFontBold() const noexcept { return _fontBold; }
    void setFontBold(bool bold) noexcept { _fontBold = bold; update(); }
signals:
    void fontBoldChanged();
private:
    bool _fontBold{ false };

    // ── 最小尺寸 ──
    Q_PROPERTY(QSizeF minimumSize READ getMinimumSize WRITE setMinimumSize NOTIFY minimumSizeChanged FINAL)
    QSizeF getMinimumSize() const noexcept { return _minimumSize; }
    void setMinimumSize(const QSizeF& size) noexcept { _minimumSize = size; }
signals:
    void minimumSizeChanged();
private:
    QSizeF _minimumSize{ 100.0, 40.0 };

    // ── 样式对象（可选，后续对接 NodeStyle）──
    Q_PROPERTY(NodeStyle* style READ getStyle WRITE setStyle NOTIFY styleChanged FINAL)
    NodeStyle* getStyle() noexcept { return _style; }
    void setStyle(NodeStyle* style);
signals:
    void styleChanged();
private:
    QPointer<NodeStyle> _style;

    // ── 拖拽 ──
protected:
    void mousePressEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;
    void mouseDoubleClickEvent(QMouseEvent* event) override;

    // ── 绘制 ──
protected:
    QSGNode* updatePaintNode(QSGNode* node, QQuickItem::UpdatePaintNodeData* data) override;
    void geometryChanged(const QRectF& newGeometry, const QRectF& oldGeometry) override;

    // ── 拖拽状态 ──
private:
    QPointF _dragStartPos;
    bool    _dragging{ false };
};

} // ::qan

QML_DECLARE_TYPE(qan::NodeItem)

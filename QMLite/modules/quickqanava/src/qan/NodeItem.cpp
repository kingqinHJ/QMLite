/*  Copyright (c) 2008-2024, Benoit AUTHEMAN All rights reserved.
    Redistribution and use in source and binary forms, with or without
    modification, are permitted provided that the following conditions are met:
       * Redistributions of source code must retain the above copyright
         notice, this list of conditions and the following disclaimer.
       * Redistributions in binary form must reproduce the above copyright
         notice, this list of conditions and the following disclaimer in the
         documentation and/or other materials provided with the distribution.
       * Neither the name of the author or Destrat.io nor the
         names of its contributors may be used to endorse or promote products
         derived from this software without specific prior written permission.
    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
    ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
    WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
    DISCLAIMED. IN NO EVENT SHALL AUTHOR BE LIABLE FOR ANY
    DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
    (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
    LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
    ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
    (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
    SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/
//-----------------------------------------------------------------------------
// This file is a part of the QuickQanava software library.
//
// \file	qanNode.cpp
// \author	benoit@destrat.io
// \date	2004 February 15
//-----------------------------------------------------------------------------

// Qt headers

// QuickQanava headers
#include <quickqanava/qan/NodeItem.h>
#include <quickqanava/qan/Node.h>
#include <quickqanava/qan/NodeStyle.h>

#include <QPointer>
#include <QMouseEvent>
#include <QSGNode>
#include <QSGSimpleTextureNode>
#include <QSGSimpleRectNode>
#include <QFont>
#include <QFontMetrics>

namespace qan { // ::qan

NodeItem::NodeItem( QQuickItem* parent )
    : QQuickItem{parent}
{
    //默认尺寸
    setWidth(120);
    setHeight(50);

    //接收鼠标事件
    setAcceptedMouseButtons(Qt::LeftButton);
    setFlag(QQuickItem::ItemIsFocusScope);
    setFlag(QQuickItem::ItemAcceptsInputMethod);

    // 关键：告诉场景图"这个 item 有自定义内容"，
    // 没有它 updatePaintNode() 不会被调用，节点显示不出来
    setFlag(QQuickItem::ItemHasContents);
}

void NodeItem::setStyle( NodeStyle* style )
{
    _style=style;
    Q_UNUSED(style);
    update();
}

//拖拽
void NodeItem::mousePressEvent(QMouseEvent* event)
{
    if (event->button() == Qt::LeftButton) {
        _dragStartPos = event->pos();
        _dragging = true;
        event->accept();
        // 置顶：拖拽时把节点移到最前面（提升 z 值）
        setZ(1.0);
    }
    else{
        QQuickItem::mousePressEvent(event);
    }
}

void  NodeItem::mouseMoveEvent(QMouseEvent* event)
{
    if (_dragging) {
        //计算位移并应用
        QPointF delta=event->pos()-_dragStartPos;
        setPosition(position() + delta);
        event->accept();
    }else{
        QQuickItem::mouseMoveEvent(event);
    }
}

void NodeItem::mouseReleaseEvent(QMouseEvent* event)
{
    if (_dragging && event->button() == Qt::LeftButton) {
        _dragging = false;
        event->accept();
    }
    else{
        QQuickItem::mouseReleaseEvent(event);
    }
}

void NodeItem::mouseDoubleClickEvent(QMouseEvent* event)
{
    Q_UNUSED(event);
}

QSGNode* NodeItem::updatePaintNode(QSGNode* node, QQuickItem::UpdatePaintNodeData* data)
{
    Q_UNUSED(data);

    delete node;
    node = nullptr;

    // ── 创建一个简单的矩形节点作为背景 ──
    // 注意：QSGSimpleRectNode 只能画纯色矩形。
    // 更复杂的绘制（圆角、文字、边框）需要用 QSG 自定义节点
    // 或使用 QQuickPaintedItem + QPainter。

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

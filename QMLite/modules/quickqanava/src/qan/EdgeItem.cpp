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
#include "NodeItem.h"
#include "Node.h"
#include "NodeStyle.h"

#include <QPointer>
#include <QMouseEvent>
#include <QSGNode>
#include <QSGSimpleTextureNode>
#include <QSGSimpleRectNode>
#include <QFont>
#include <QFontMetrics>

namespace qan { // ::qan

EdgeItem::EdgeItem( QQuickItem* parent )
    : QQuickItem{parent}
{
    setAcceptedMouseButtons(Qt::NoButton);

    setZ(-1);
}

void EdgeItem::setSourceItem(NoteItem* item)
{
    if(_sourceItem!=item){
        _sourceItem=item;
        emit sourceItemChanged();
        updateItem();
    }
}

void EdgeItem::setDestinationItem(NoteItem* item)
{
    if(_destinationItem!=item){
        _destinationItem=item;
        emit destinationItemChanged();
        updateItem();
    }
}

void EdgeItem::updateItem()
{
    if(!_sourceItem || !_destinationItem)
        return;

    //计算源和目标的中心点
    QPointF srcCenter = _sourceItem->mapToItem(this,
            QPointF(_sourceItem->width()/2, _sourceItem->height()/2));
    QPointF destCenter = _destinationItem->mapToItem(this,
            QPointF(_destinationItem->width()/2, _destinationItem->height()/2));

    _p1=srcCenter;
    _p2=destCenter;
    emit p1Changed();
    emit p2Changed();

    //更新item自身的几何范围以容纳整条线
    prepareGeometryChange();

    //给一些额外空间用于箭头
    QRectF rect (QPointF(qMin(_p1.x(), _p2.x()), qMin(_p1.y(), _p2.y())),
                QSizeF(qAbs(_p2.x() - _p1.x()), qAbs(_p2.y() - _p1.y())));

    rect.adjust(-arrowSize, -arrowSize, arrowSize, arrowSize);

    setX(rect.x());
    setY(rect.y());
    setWidth(rect.width(),1.0);
    setHeight(rect.height(),1.0);

    update();
}

QSGNode* EdgeItem::updatePaintNode(QSGNode* node, QQuickItem::UpdatePaintNodeData* data)
{
    Q_UNUSED(data);
    delete oldNode;
    oldNode = nullptr;

    if(!sourceItem || !destinationItem)
        return nullptr;

    // ── 简单实现：绘制一条直线作为占位 ──
    // 生产代码应使用自定义 QSG 节点绘制贝塞尔曲线 + 箭头

    // 转换端点到本地坐标
    QPointF localP1 = mapFromItem(this, QPointF(_sourceItem->x()+_sourceItem->width()/2, 
                    _sourceItem->y()+_sourceItem->height()/2));
    QPointF localP2 = mapFromItem(this, QPointF(_destinationItem->x()+_destinationItem->width()/2, 
                    _destinationItem->y()+_destinationItem->height()/2));

    Q_UNUSED(localP1);
    Q_UNUSED(localP2);

    // 创建矩形来覆盖线条区域（Phase 2 阶段用 QQuickPaintedItem 或
    // QML Canvas 更好；纯 QSG 实现较复杂，这里仅作占位）
    QSGSimpleLineNode* node = new QSGSimpleLineNode;
    node->setRect(boundingRect());

    // 简单的颜色占位：使用 lineColor 填充整个包围盒
    // 真正的绘制在 Phase 3+ 通过 QSGNinePatchNode 或自定义 QSGNode 实现
    node->setColor(lineColor);

    return node;
}

void EdgeItem::geometryChanged(const QRectF& newGeometry, const QRectF& oldGeometry)
{
    QQuickItem::geometryChanged(newGeometry, oldGeometry);
    update();
}
} // ::qan

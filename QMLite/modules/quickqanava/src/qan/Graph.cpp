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
#include <quickqanava/qan/Edge.h>
#include <quickqanava/qan/Graph.h>
#include <quickqanava/qan/Node.h>
#include <quickqanava/qan/EdgeItem.h>
#include <quickqanava/qan/NodeItem.h>
#include <quickqanava/qan/Group.h>

#include<QQmlEngine>
#include<QQmlContext>
#include<QQuickItem>

namespace qan { // ::qan

Graph::Graph( QObject* parent /*= nullptr*/ ) noexcept
    : super_t{qobject_cast<QQuickItem*>(parent)}
{
    _containerItem = this;
}

Graph::~Graph() 
{

}

void Graph::classBegin()
{
}

void Graph::componentComplete()
{
    
}

void Graph::setContainerItem( QQuickItem* item )
{
    if ( _containerItem != item ) {
        _containerItem = item;
        emit containerItemChanged();
    }
}

void Graph::setNodeDelegate( QQmlComponent* delegate )
{
    _nodeDelegate = delegate;
    emit nodeDelegateChanged();
}

void Graph::setEdgeDelegate( QQmlComponent* delegate )
{
    _edgeDelegate = delegate;
    emit edgeDelegateChanged();
}

void Graph::setGroupDelegate( QQmlComponent* delegate )
{
    _groupDelegate = delegate;
    emit groupDelegateChanged();
}

void Graph::setSelectionDelegate( QQmlComponent* delegate )
{
    _selectionDelegate.reset( delegate );
    emit selectionDelegateChanged();
}

std::unique_ptr<QQmlComponent> Graph::createComponent( const QString& url )
{
    QQmlEngine *engine = qmlEngine(this);
    if (!engine )
        return nullptr;
    auto comp=std::make_unique<QQmlComponent>( engine, QUrl( url ),this );
    if(comp->isError()){
        qWarning() << "Graph::createComponent():" << comp->errorString();
        return nullptr;
    }
    return comp;
}

QQuickItem* Graph::createItemFromComponent( QQmlComponent* component )
{
    if(!component)
        return nullptr;
    QQmlEngine* engine = qmlEngine(this);
    if(!engine)
        return nullptr;

    QQmlContext* ctx=QQmlEngine::contextForObject(this);
    QObject* obj=component->beginCreate(ctx?ctx:engine->rootContext());
    if (!obj)
    {
        qWarning() << "Graph::createItemFromComponent(): cannot create item from delegate"
                   << component->errorString();
        return nullptr;
    }

    QQuickItem* item = qobject_cast<QQuickItem*>(obj);
    if (!item)
    {
        delete obj;
        qWarning() << "Graph::createItemFromComponent(): cannot create item from delegate";
        return nullptr;
    }

    // 设置父项（containerItem），纳入 Qt 父子树管理
    QQuickItem* container = getContainerItem();
    item->setParentItem( container ?container:this );

    component->completeCreate();
    return item;
}


// ── 工厂：创建节点 ──
Node* Graph::insertNode(qreal x, qreal y, const QString& label)
{
    // 1. 创建节点数据模型（gtpo 工厂：new qan::Node）
    Node* node = create_node();
    if (node == nullptr)
        return nullptr;
    node->setLabel(label);
    insert_node(node);                     // 注册进图：set_graph + 入容器 + 通知

    // 2. 创建可视化 NodeItem，挂到 Graph（QQuickItem 父子树）
    auto* item = new NodeItem(this);
    node->setItem(item);                   // item->setNode(node) 双向绑定
    item->setX(x);
    item->setY(y);
    return node;
}

// ── 工厂：创建边 ──
Edge* Graph::insertEdge(Node* source, Node* destination)
{
    if (source == nullptr || destination == nullptr)
        return nullptr;

    // 1. 创建边数据模型（gtpo 工厂：new qan::Edge + src/dst 双向注册）
    Edge* edge = insert_edge(source, destination);
    if (edge == nullptr)
        return nullptr;

    // 2. 创建可视化 EdgeItem，连接两个 NodeItem
    auto* edgeItem = new EdgeItem(this);
    edge->setItem(edgeItem);               // edgeItem->setEdge(edge) 双向绑定
    edgeItem->setSourceItem(qobject_cast<NodeItem*>(source->getItem()));
    edgeItem->setDestinationItem(qobject_cast<NodeItem*>(destination->getItem()));
    edgeItem->updateItem();
    return edge;
}

void Graph::clearGraph() noexcept
{
    // 1. 先删除所有可视化图形项（NodeItem / EdgeItem）
    //    它们在 Qt 父子树下，deleteLater 交给事件循环安全释放
    const auto children = childItems();
    for (QQuickItem* child : children)
        child->deleteLater();

    // 2. 再清空数据模型（gtpo::graph::clear() 逐个 delete node/edge）
    //    数据模型析构时 QPointer<Item> 自动置空，不会悬空
    clear();
}
} // ::qan

#pragma once

// ============================================================================
// Graph.h — qan::Graph 精简参考实现
// ============================================================================
//
// 继承 gtpo::graph，注入 QQuickItem。QML 顶层入口。
//
// 核心职责：
//   1) 工厂：insertNode() / insertEdge() — 创建数据模型 + 对应可视化图形项
//   2) 委托：nodeDelegate / edgeDelegate / groupDelegate — QML 组件
//   3) 容器：containerItem — 所有图形项的 QML 父节点
//
// 不依赖 Style / Behaviour / Connector / quickcontainers。

#include <QQuickItem>
#include <QQmlComponent>
#include <QQmlParserStatus>
#include <QPointer>
#include <Memory>
#include <quickqanava/gtpo/graph.h>

#include"./Node.h"
#include "./Edge.h"
#include "./Group.h"

namespace qan {

class NodeItem;
class EdgeItem;
class GroupItem;

class Graph : public QQuickItem,
            public gtpo::graph<QQuickItem, qan::Node, qan::Group, qan::Edge>,
            public QQmlParserStatus
{
    Q_OBJECT
    QML_ELEMENT
    Q_INTERFACES(QQmlParserStatus)

    using super_t = gtpo::graph<QQuickItem, qan::Node, qan::Group, qan::Edge>;
    // TODO: Phase 2 开始实现
public:
    explicit Graph(QObject* parent = nullptr) noexcept;
    virtual ~Graph() override;

    // ── 拷贝/移动：禁止 ──
    Graph(const Graph& other) = delete;
    Graph& operator=(const Graph& other) = delete;
    Graph(Graph&& other) =delete;
    Graph& operator=(Graph&& other) =delete;

    // ── QML 解析生命周期
    virtual void classBegin() override;
    virtual void componentComplete() override;

    // ── 容器图形项
    Q_PROPERTY(QQuickItem* containerItem READ getContainerItem NOTIFY containerItemChanged FINAL)
    QQuickItem* getContainerItem() noexcept{return _containerItem.data();};
    void setContainerItem(QQuickItem* item);
signals:
    void containerItemChanged();
private:
    QPointer<QQuickItem> _containerItem;

    //委托组件
    Q_PROPERTY(QQmlComponent* nodeDelegate READ getNodeDelegate WRITE setNodeDelegate NOTIFY nodeDelegateChanged FINAL)
    QQmlComponent* getNodeDelegate() noexcept{return _nodeDelegate.data();};
    void setNodeDelegate(QQmlComponent* delegate);
signals:
    void nodeDelegateChanged();
private:
    QPointer<QQmlComponent> _nodeDelegate;

    Q_PROPERTY(QQmlComponent* edgeDelegate READ getEdgeDelegate WRITE setEdgeDelegate NOTIFY edgeDelegateChanged FINAL)
    QQmlComponent* getEdgeDelegate() noexcept{return _edgeDelegate.data();};
    void setEdgeDelegate(QQmlComponent* delegate);
signals:
    void edgeDelegateChanged();
private:
    QPointer<QQmlComponent> _edgeDelegate;

    Q_PROPERTY(QQmlComponent* groupDelegate READ getGroupDelegate WRITE setGroupDelegate NOTIFY groupDelegateChanged FINAL)
    QQmlComponent* getGroupDelegate() noexcept{return _groupDelegate.data();};
    void setGroupDelegate(QQmlComponent* delegate);
signals:
    void groupDelegateChanged();
private:
    QPointer<QQmlComponent> _groupDelegate;

public:
    Q_PROPERTY(QQmlComponent* selectionDelegate READ getSelectionDelegate WRITE setSelectionDelegate NOTIFY selectionDelegateChanged FINAL)    
    QQmlComponent* getSelectionDelegate() noexcept{return _selectionDelegate.data();};
    void setSelectionDelegate(QQmlComponent* delegate);
signals:
    void selectionDelegateChanged();
private:
    std::unique_ptr<QQmlComponent> _selectionDelegate;

protected:
    std::unique_ptr<QQmlComponent> createComponent(const QString& url);
    QQuickItem* createItemFromComponent(QQmlComponent* delegate);

    Q_INVOKABLE void clearGraph() noexcept;
};

} // ::qan

QML_DECLARE_TYPE(qan::Graph)//不知道这个用来干嘛
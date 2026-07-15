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
#include <memory>

#include <quickqanava/gtpo/graph.h>

#include "./Node.h"
#include "./Edge.h"
#include "./Group.h"

namespace qan {

class NodeItem;
class EdgeItem;
class GroupItem;

class Graph : public QQuickItem,
              public gtpo::graph<QQuickItem, Node, Group, Edge>,
              public QQmlParserStatus
{
    Q_OBJECT
    QML_ELEMENT
    Q_INTERFACES(QQmlParserStatus)

    using super_t = gtpo::graph<QQuickItem, Node, Group, Edge>;

public:
    explicit Graph(QQuickItem* parent = nullptr) noexcept;
    virtual ~Graph() override;

    // ── 拷贝/移动：禁止 ──
    Graph(const Graph&) = delete;
    Graph& operator=(const Graph&) = delete;
    Graph(Graph&&) = delete;
    Graph& operator=(Graph&&) = delete;

    // ── QML 解析生命周期 ──
    void classBegin() override;
    void componentComplete() override;

    // ── 容器图形项 ──
    Q_PROPERTY(QQuickItem* containerItem READ getContainerItem
               NOTIFY containerItemChanged FINAL)
    QQuickItem*        getContainerItem() noexcept { return _containerItem.data(); }
    void               setContainerItem(QQuickItem* item);
signals:
    void               containerItemChanged();
private:
    QPointer<QQuickItem> _containerItem;

    // ── 委托组件 ──
    Q_PROPERTY(QQmlComponent* nodeDelegate READ getNodeDelegate
               WRITE setNodeDelegate NOTIFY nodeDelegateChanged FINAL)
    QQmlComponent* getNodeDelegate() noexcept { return _nodeDelegate.get(); }
    void           setNodeDelegate(QQmlComponent* comp) noexcept;
signals:
    void           nodeDelegateChanged();
private:
    std::unique_ptr<QQmlComponent> _nodeDelegate;

    Q_PROPERTY(QQmlComponent* edgeDelegate READ getEdgeDelegate
               WRITE setEdgeDelegate NOTIFY edgeDelegateChanged FINAL)
    QQmlComponent* getEdgeDelegate() noexcept { return _edgeDelegate.get(); }
    void           setEdgeDelegate(QQmlComponent* comp) noexcept;
signals:
    void           edgeDelegateChanged();
private:
    std::unique_ptr<QQmlComponent> _edgeDelegate;

    Q_PROPERTY(QQmlComponent* groupDelegate READ getGroupDelegate
               WRITE setGroupDelegate NOTIFY groupDelegateChanged FINAL)
    QQmlComponent* getGroupDelegate() noexcept { return _groupDelegate.get(); }
    void           setGroupDelegate(QQmlComponent* comp) noexcept;
signals:
    void           groupDelegateChanged();
private:
    std::unique_ptr<QQmlComponent> _groupDelegate;

public:
    Q_PROPERTY(QQmlComponent* selectionDelegate READ getSelectionDelegate
               WRITE setSelectionDelegate NOTIFY selectionDelegateChanged FINAL)
    QQmlComponent* getSelectionDelegate() noexcept { return _selectionDelegate.get(); }
    void           setSelectionDelegate(QQmlComponent* comp) noexcept;
signals:
    void           selectionDelegateChanged();
private:
    std::unique_ptr<QQmlComponent> _selectionDelegate;

    // ── 从 QML 组件创建图形项 ──
protected:
    QQuickItem* createFromComponent(QQmlComponent* component,
                                    Node*  node  = nullptr,
                                    Edge*  edge  = nullptr,
                                    Group* group = nullptr) noexcept;
    // ── 内部辅助 ──
private:
    std::unique_ptr<QQmlComponent> createComponent(const QString& url);
    QQuickItem* createItemFromComponent(QQmlComponent* component);

    // ── 清空图 ──
    Q_INVOKABLE void clearGraph() noexcept;
};

} // ::qan

QML_DECLARE_TYPE(qan::Graph)

#pragma once

// ============================================================================
// Node.h — qan::Node（拓扑节点 + QObject 属性）
// ============================================================================
//
// 继承 gtpo::node，node_base_t 注入 QObject。
// 注意：Q_PROPERTY 与 Q_SIGNALS 不能交替出现（MOC 解析问题），
//      所有属性声明集中在上方，所有信号集中在下方同一个 signals 段。

#include <QObject>
#include <QString>
#include <QPointer>
#include <QQmlEngine>
#include <QQuickItem>
#include <quickqanava/gtpo/node.h>

namespace qan {

class Graph;
class Edge;
class Group;
class NodeItem;

class Node : public gtpo::node<QObject, Graph, Node, Edge, Group>
{
    Q_OBJECT
    QML_ELEMENT

public:
    using super_t = gtpo::node<QObject, Graph, Node, Edge, Group>;

    explicit Node(QObject* parent = nullptr);
    virtual ~Node() = default;
    Node(const Node& other) = delete;

    // ── 图访问 ──
    Q_PROPERTY(Graph* graph READ getGraph CONSTANT FINAL)
    const Graph* getGraph() const noexcept;
    Graph* getGraph() noexcept;

    // ── 可视化项 ──
    Q_PROPERTY(NodeItem* item READ getItem CONSTANT FINAL)
    NodeItem* getItem() noexcept;
    const NodeItem* getItem() const noexcept;
    virtual void setItem(NodeItem* item) noexcept;

    // ── 标签 ──
    Q_PROPERTY(QString label READ getLabel WRITE setLabel NOTIFY labelChanged FINAL)
    QString getLabel() const { return _label; }
    bool setLabel(const QString& label);

    // ── 锁定 / 保护 ──
    Q_PROPERTY(bool locked READ getLocked WRITE setLocked NOTIFY lockedChanged FINAL)
    bool getLocked() const { return _locked; }
    virtual bool setLocked(bool locked);

    Q_PROPERTY(bool isProtected READ getIsProtected WRITE setIsProtected NOTIFY isProtectedChanged FINAL)
    bool getIsProtected() const { return _isProtected; }
    bool setIsProtected(bool isProtected);

    // ── 组访问 ──
    Q_PROPERTY(Group* group READ getGroup CONSTANT FINAL)
    const Group* getGroup() const { return get_group(); }
    Group* getGroup() { return get_group(); }
    Q_INVOKABLE bool hasGroup() const { return get_group() != nullptr; }
    Q_INVOKABLE bool isGroup() const { return is_group(); }

    // ── 入度 / 出度 ──
    Q_PROPERTY(int inDegree READ getInDegree NOTIFY inDegreeChanged FINAL)
    int getInDegree() const;

    Q_PROPERTY(int outDegree READ getOutDegree NOTIFY outDegreeChanged FINAL)
    int getOutDegree() const;

    // ── 静态工厂 ──
    static QQmlComponent* delegate(QQmlEngine& engine, QObject* parent = nullptr) noexcept;
    static class NodeStyle* style(QObject* parent = nullptr) noexcept;

Q_SIGNALS:
    void labelChanged();
    void lockedChanged();
    void isProtectedChanged();
    void inDegreeChanged();
    void outDegreeChanged();

protected:
    QPointer<NodeItem> _item;

private:
    QString _label = "";
    bool _locked = false;
    bool _isProtected = false;
};

} // ::qan

QML_DECLARE_TYPE(qan::Node)

#pragma once

// ============================================================================
// Node.h — qan::Node 精简参考实现
// ============================================================================
//
// 继承 gtpo::node，注入 QObject：
//   - 获得 QObject 的信号槽、QML 属性绑定、父子树内存管理
//   - 获得 gtpo::node 的出入边集合、组语义、观察者能力
//
// 成员：
//   - _label        : 节点标签（QML 中绑定到 NodeItem 的文字显示）
//   - _locked       : 锁定后禁止选中/拖拽
//   - _isProtected  : 保护后禁止拖拽但允许选择
//   - _item         : QPointer<NodeItem>，指向可视化图形项
//
// 三五原则：
//   - 拷贝 = delete（继承自 gtpo::node 的 delete + QObject 隐式 delete）
//   - 虚析构 = default
// ============================================================================

#include <QObject>
#include <QString>
#include <QPointer>
#include <QQmlEngine>
#include <QQuickItem>

#include <quickqanava/gtpo/node.h>

namespace qan {

// 前向声明
class Graph;
class Edge;
class Group;
class NodeItem;

class Node : public QObject,
             public gtpo::node<QObject, Graph, Node, Edge, Group>
{
    Q_OBJECT
    QML_ELEMENT

public:
    using super_t = gtpo::node<QObject, Graph, Node, Edge, Group>;

    // ── 构造/析构 ──
    explicit Node(QObject* parent = nullptr);
    virtual ~Node() = default;
    Node(const Node&) = delete;

    // ── 图访问 ──
    Q_PROPERTY(Graph* graph READ getGraph CONSTANT FINAL)
    Graph*        getGraph() noexcept;
    const Graph*  getGraph() const noexcept;

    // ── 可视化项 ──
    Q_PROPERTY(NodeItem* item READ getItem CONSTANT)
    NodeItem*        getItem() noexcept;
    const NodeItem*  getItem() const noexcept;
    virtual void     setItem(NodeItem* item) noexcept;
protected:
    QPointer<NodeItem> _item;

    // ── 标签 ──
    Q_PROPERTY(QString label READ getLabel WRITE setLabel NOTIFY labelChanged FINAL)
    bool     setLabel(const QString& label);
    QString  getLabel() const { return _label; }
signals:
    void     labelChanged();
private:
    QString  _label = "";

    // ── 锁定/保护 ──
    Q_PROPERTY(bool locked READ getLocked WRITE setLocked NOTIFY lockedChanged FINAL)
    virtual bool  setLocked(bool locked);
    bool          getLocked() const { return _locked; }
signals:
    void          lockedChanged();
private:
    bool          _locked = false;

    Q_PROPERTY(bool isProtected READ getIsProtected WRITE setIsProtected NOTIFY isProtectedChanged FINAL)
    bool     setIsProtected(bool isProtected);
    bool     getIsProtected() const { return _isProtected; }
signals:
    void     isProtectedChanged();
private:
    bool     _isProtected = false;

    // ── 组访问 ──
    Q_PROPERTY(Group* group READ getGroup FINAL)
    const Group*  getGroup() const { return get_group(); }
    Group*        getGroup() { return get_group(); }
    Q_INVOKABLE bool hasGroup() const { return get_group() != nullptr; }
    Q_INVOKABLE bool isGroup() const { return is_group(); }

    // ── 入度/出度 ──
    Q_PROPERTY(int inDegree READ getInDegree NOTIFY inDegreeChanged FINAL)
    int  getInDegree() const;
signals:
    void inDegreeChanged();

    Q_PROPERTY(int outDegree READ getOutDegree NOTIFY outDegreeChanged FINAL)
    int  getOutDegree() const;
signals:
    void outDegreeChanged();

    // ── 静态工厂 ──
public:
    static QQmlComponent*  delegate(QQmlEngine& engine, QObject* parent = nullptr) noexcept;
    static class NodeStyle* style(QObject* parent = nullptr) noexcept;
};

} // ::qan

QML_DECLARE_TYPE(qan::Node)

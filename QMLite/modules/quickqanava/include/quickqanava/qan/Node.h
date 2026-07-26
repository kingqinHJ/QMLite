#pragma once

#include <QObject>
#include <quickqanava/gtpo/node.h>
#include <QString>
#include <QPointer>
#include <QQmlEngine>
#include <QQuickItem>

namespace qan {

class Graph;
class Edge;
class Group;
class NodeItem;

// Phase 2+：qan::Node 继承 gtpo::node，把 node_base_t 设为 QObject
//
// TODO:
// - class Node : public gtpo::node<QObject, qan::Graph, qan::Node, qan::Edge, qan::Group>
// - Q_OBJECT
// - 属性：label, locked, isProtected
// - 持有 _item 指针（NodeItem）
// - 静态工厂 delegate() / style()

class Node : public gtpo::node<QObject, Graph, Node, Edge, Group>
{
    Q_OBJECT
    QML_ELEMENT
public:
    //构造/析构
    using super_t = gtpo::node<QObject, Graph, Node, Edge, Group>;
    explicit Node(QObject* parent = nullptr);
    virtual ~Node()=default;
    Node(const Node& other)=delete;

    //图
    Q_PROPERTY(Graph* graph READ getGraph CONSTANT FINAL)
    const Graph* getGraph() const noexcept;
    Graph* getGraph() noexcept;

    //可视化项
    Q_PROPERTY(NodeItem* item READ getItem CONSTANT FINAL)
    NodeItem* getItem() const noexcept;
    const NodeItem* getItem() const noexcept;
    virtual void setItem(NodeItem* item)noexcept;
protected:
    QPointer<NodeItem> _item;

    //标签
    Q_PROPERTY(QString label READ getLabel WRITE setLabel NOTIFY labelChanged FINAL)
    QString getLabel() { return _label; }
    bool setLabel(const QString& label);
    Q_SIGNALS labelChanged(QString label )
private:
    QString _label = "";
    
    //锁定/保护
    Q_PROPERTY(bool locked READ getLocked WRITE setLocked NOTIFY lockedChanged FINAL)
    bool getLocked() { return _locked; }
    bool setLocked(bool locked);
    Q_SIGNALS lockedChanged(bool locked )
private:
    bool _locked = false;
    Q_PROPERTY(bool isProtected READ getIsProtected WRITE setIsProtected NOTIFY isProtectedChanged FINAL)
    bool getIsProtected() { return _isProtected; }
    bool setIsProtected(bool isProtected);
    Q_SIGNALS isProtectedChanged(bool isProtected )
private:
    bool _isProtected = false;

    //组访问
    Q_PROPERTY(Group* group READ getGroup CONSTANT FINAL)
    const Group* getGroup() const { return get_group(); }
    Group* getGroup() {return get_group(); }
    Q_INVOKABLE bool hasGroup() const { return get_group() != nullptr; }
    Q_INVOKABLE bool isGroup() const { return is_group(); }

    Q_PROPERTY(int inDegree READ getInDegree NOTIFY inDegreeChanged FINAL)
    int getInDegree() const ;
    Q_SIGNALS inDegreeChanged(int inDegree )

    Q_PROPERTY(int outDegree READ getOutDegree NOTIFY outDegreeChanged FINAL)
    int getOutDegree() const ;
    Q_SIGNALS outDegreeChanged(int outDegree )

public:
    //静态工厂
    static QQmlComponent* delegate(QQmlEngine &engine, QObject* parent=nullptr) noexcept;
    static class NodeStyle* style(QObject* parent=nullptr) noexcept;
};

} // ::qan

QML_DECLARE_TYPE(qan::Node)

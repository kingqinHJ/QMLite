#pragma once

// ============================================================================
// Edge.h — qan::Edge 精简参考实现
// ============================================================================
//
// 继承 gtpo::edge，注入 QObject。
// 成员：_label（边标签）、_weight（权重）、_item（可视化 EdgeItem 回指针）。

#include <QObject>
#include <QString>
#include <QPointer>
#include <QQmlEngine>

#include <quickqanava/gtpo/edge.h>

namespace qan {

class Graph;
class Node;
class EdgeItem;

class Edge : public gtpo::edge<QObject, Graph, Node>
{
    Q_OBJECT
    QML_ELEMENT

public:
    using super_t = gtpo::edge<QObject, Graph, Node>;

    explicit Edge(QObject* parent = nullptr);
    virtual ~Edge() override = default;
    Edge(const Edge&) = delete;

    // ── 图访问 ──
    Q_PROPERTY(Graph* graph READ getGraph CONSTANT FINAL)
    Graph*        getGraph() noexcept;
    const Graph*  getGraph() const noexcept;

    // ── 可视化项 ──
    Q_PROPERTY(EdgeItem* item READ getItem CONSTANT)
    EdgeItem*        getItem() noexcept;
    const EdgeItem*  getItem() const noexcept;
    virtual void     setItem(EdgeItem* item) noexcept;
protected:
    QPointer<EdgeItem> _item;

    // ── 源/目标节点 ──
    Q_INVOKABLE Node* getSource()      { return get_src(); }
    Q_INVOKABLE Node* getDestination() { return get_dst(); }

    // ── 标签 ──
    Q_PROPERTY(QString label READ getLabel WRITE setLabel NOTIFY labelChanged FINAL)
    bool          setLabel(const QString& label);
    const QString& getLabel() const { return _label; }
signals:
    void          labelChanged();
private:
    QString       _label = "";

    // ── 权重 ──
    Q_PROPERTY(qreal weight READ getWeight WRITE setWeight NOTIFY weightChanged FINAL)
    qreal  getWeight() const { return _weight; }
    bool   setWeight(qreal weight);
signals:
    void   weightChanged();
private:
    qreal  _weight = 1.0;

    // ── 锁定 ──
    Q_PROPERTY(bool locked READ getLocked WRITE setLocked NOTIFY lockedChanged FINAL)
    bool  setLocked(bool locked);
    bool  getLocked() const { return _locked; }
signals:
    void  lockedChanged();
private:
    bool  _locked = false;

    // ── 静态工厂 ──
public:
    static QQmlComponent*  delegate(QQmlEngine& engine, QObject* parent = nullptr) noexcept;
    static class EdgeStyle* style(QObject* parent = nullptr) noexcept;
};

} // ::qan

QML_DECLARE_TYPE(qan::Edge)

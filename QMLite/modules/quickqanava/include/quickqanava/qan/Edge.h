#pragma once

#include <QObject>
#include <quickqanava/gtpo/edge.h>
#include <QString>
#include <QPointer>
#include <QQmlEngine>


namespace qan {

class Graph;
class Node;
class Group;
class Nodeitem;

// Phase 2+：qan::Edge 继承 gtpo::edge，把 edge_base_t 设为 QObject
//
// TODO:
// - class Edge : public gtpo::edge<QObject, qan::Graph, qan::Node>
// - Q_OBJECT
// - 属性：label, weight, locked, isProtected
// - 持有 _item 指针（EdgeItem）

class Edge : public gtpo::edge<QObject, Graph, Node>
{
    Q_OBJECT
    QML_ELEMENT
    // TODO: Phase 2 开始实现

public:
    using super_t = gtpo::edge<QObject, Graph, Node>;

    explicit Edge(QObject* parent = nullptr);
    virtual ~Edge() override= default;
    Edge(const Edge& other) = delete;

    //图访问
    Q_PROPERTY(Graph* graph READ getGraph CONSTANT FINAL)
    Graph* getGraph() const noexcept;
    const Graph* getGraph() const noexcept;

    //可视化项
    Q_PROPERTY(EdgeItem* item READ getItem CONSTANT)
    EdgeItem* getItem() const noexcept;
    const EdgeItem* getItem() const noexcept;
    virtual void setItem(EdgeItem* item) noexcept;

protected:
    QPointer<EdgeItem> _item;

    //源/目标节点
    Q_INVOKABLE NODE* getSource() {return get_src();}
    Q_INVOKABLE NODE* getDestination() {return get_dst();}
    
    //标签
    Q_PROPERTY(QString label READ getLabel WRITE setLabel NOTIFY labelChanged)
    bool setLabel(const QString& label);
    const QString& getLabel() const{return _label;}
signals:
    void labelChanged();
private:
    QString _label="";

    //权重
    Q_PROPERTY(qreal weight READ getWeight WRITE setWeight NOTIFY weightChanged)
    qreal getWeight() const{return _weight;}
    bool setWeight(qreal weight);
signals:
    void weightChanged();
private:
    qreal _weight=1.0;

    Q_PROPERTY(bool locked READ isLocked WRITE setLocked NOTIFY lockedChanged)
    bool isLocked() const{return _locked;}
    void setLocked(bool locked);
signals:
    void lockedChanged();
private:
    bool _locked=false;

    //静态工厂
public:
    static QQmlComponent* delegate(QQmlEngine &engine, QObject* parent=nullptr)noexcept;
    static class EdgeStyle* style(QObject* parent=nullptr) noexcept;
};

} // ::qan

QML_DECLARE_TYPE(qan::Edge)

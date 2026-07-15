#pragma once

// ============================================================================
// node.h — gtpo::node<> 图节点
// ============================================================================
//
// 继承链（多混入）：
//   node_base_t                                   ← 注入基类（如 QObject）
//   graph_property_impl<graph_t>                  ← "我属于哪张图"
//   observable_node<node_t, edge_t>               ← "我可以被观察"
//     └── node<node_base_t, graph_t, node_t, edge_t, group_t>
//
// 职责：
//   1) 维护出入边集合（_in_edges / _out_edges）
//   2) 维护出入节点冗余缓存（_in_nodes / _out_nodes）—— 空间换时间
//   3) 支持组语义（_is_group / _nodes，组也是一种 node）
//   4) 提供观察者通知能力（继承自 observable_node）
//
// 冗余缓存的设计意图：
//   _in_nodes 是 _in_edges 中每条边的 src 节点的集合。
//   可以从 edge->get_src() 反查，但图算法（DFS/BFS/拓扑排序）中
//   "遍历所有入节点"是高频操作。多存一份指针 → O(1) vs O(E)。
//
// 三五原则（第五种模式）：
//   析构     → 清理拓扑 + 警告 + 置空图指针
//   拷贝构造 → 空操作！（允许拷贝属性，但拓扑关系绝不复製）
//   拷贝赋值 → = delete
//   移动     → 隐式不生成（声明了拷贝构造）
//
//   这是"身份语义"的精准表达：
//     节点身份 = 指针地址，拷贝属性可以，拷贝拓扑关系不行。
// ============================================================================

#include <QVector>         // QVector
#include <unordered_set>
#include <cassert>
#include <iterator>
#include <iostream>        // std::cerr

#include "./graph_property.h"
#include "./observer.h"
#include "./observable.h"
#include "./container_adapter.h"

namespace gtpo { // ::gtpo

template <class node_base_t, class graph_t, class node_t, class edge_t, class group_t>
class node : public node_base_t,
             public graph_property_impl<graph_t>,
             public gtpo::observable_node<node_t, edge_t>
{
public:
    // ── 类型别名 ──
    // 注意：edges_t（复数）是边的容器，edge_t（单数，模板参数）是边的类型
    using edges_t = QVector<edge_t*>;     // 边集合
    using nodes_t = QVector<node_t*>;      // 节点集合
    using observable_base_t = gtpo::observable_node<node_t, edge_t>;

    // graph_t 需要直接操作 _in_edges / _out_edges 等成员
    friend graph_t;

    // ── 构造/析构 ──
    explicit node(node_base_t* parent = nullptr) noexcept
        : node_base_t{}
    {
        static_cast<void>(parent);  // parent 仅用于 QObject 等基类，EmptyBase 忽略
    }

    //! 虚析构：清理拓扑关联，如果还绑在图上则发出警告
    virtual ~node() noexcept
    {
        _in_edges.clear();
        _out_edges.clear();
        _in_nodes.clear();
        _out_nodes.clear();

        if (this->_graph != nullptr) {
            std::cerr << "gtpo::node<>::~node(): Warning: Node has been "
                         "destroyed before being removed from the graph."
                      << std::endl;
        }
        this->_graph = nullptr;
    }

    // ── 拷贝控制（身份语义）──

    //! 拷贝构造：空操作 —— 基类拓扑成员（出入边/组）不拷。
    //!
    //! 为什么不是 = delete？
    //!   派生类（如 qan::Node）有自己的属性（label、weight 等），
    //!   它们需要通过派生类拷贝构造来复制。= delete 会把这条路堵死。
    //!
    //! 为什么不是 = default？
    //!   默认逐成员拷贝会把 _in_edges/_out_edges 的指针也拷过去，
    //!   导致两个节点共享同一套拓扑关系 → double-free。
    //!
    //! 所以空操作是唯一正确选择：
    //!   "拓扑是基类的，我不拷。属性是派生类的，你自己拷。"
    //!   派生类写法：MyNode(const MyNode& o) : node(o), _label(o._label) {}
    node(const node& node)
    {
        static_cast<void>(node);   // 拓扑成员不拷贝，派生类属性请自行拷贝
    }

    //! 拷贝赋值：禁止 —— 节点身份不能"覆盖"
    auto operator=(const node&) = delete;

    // ── 观察者 ──
public:
    //! 添加节点观察者（sink：交出所有权，自动设置 target）
    auto add_node_observer(std::unique_ptr<gtpo::node_observer<node_t, edge_t>> observer) noexcept -> void
    {
        if (observer)
            observer->set_target(reinterpret_cast<node_t*>(this));
        observable_base_t::add_node_observer(std::move(observer));
    }

    // ── 出入边管理 ──
public:
    //! 添加出边（同时维护 _out_edges 和 _out_nodes 缓存）
    auto add_out_edge(edge_t* outEdge) -> bool;
    //! 添加入边
    auto add_in_edge(edge_t* inEdge) -> bool;
    //! 移除出边
    auto remove_out_edge(const edge_t* outEdge) -> bool;
    //! 移除入边
    auto remove_in_edge(const edge_t* inEdge) -> bool;

    // ── 只读访问器 ──
    inline auto get_in_edges()  const noexcept -> const edges_t& { return _in_edges; }
    inline auto get_out_edges() const noexcept -> const edges_t& { return _out_edges; }
    inline auto get_in_nodes()  const noexcept -> const nodes_t& { return _in_nodes; }
    inline auto get_out_nodes() const noexcept -> const nodes_t& { return _out_nodes; }

    //! 入度
    inline auto get_in_degree()  const noexcept -> int { return static_cast<int>(_in_edges.size()); }
    //! 出度
    inline auto get_out_degree() const noexcept -> int { return static_cast<int>(_out_edges.size()); }

    // ── 成员变量 ──
private:
    edges_t _in_edges;     // 入边集合（缓存：避免通过全图反查）
    edges_t _out_edges;    // 出边集合
    nodes_t _in_nodes;     // 入节点集合（冗余缓存：图算法高频查询优化）
    nodes_t _out_nodes;    // 出节点集合

    // ── 组相关（节点同时也可以是"组"，容纳子节点）──
public:
    //! 设置所属组（仅 graph_t 友元可调用）
    inline auto set_group(const group_t* group) noexcept -> void
    {
        _group = const_cast<group_t*>(group);
    }

    //! 获取所属组（nullptr = 未分组）
    inline auto get_group() noexcept -> group_t* { return _group; }
    //! 获取所属组（const 版本）
    inline auto get_group() const noexcept -> const group_t* { return _group; }

    //! 当前节点是否是一个组（可包含子节点）
    inline auto is_group() const noexcept -> bool { return _is_group; }

    //! 获取组内子节点列表
    inline auto get_nodes() const noexcept -> const nodes_t& { return _nodes; }
    inline auto get_nodes()       noexcept ->       nodes_t& { return _nodes; }

    //! 判断某个节点是否在此组内
    auto has_node(const node_t* node) const noexcept -> bool
    {
        if (node == nullptr)
            return false;
        return _nodes.contains(const_cast<node_t*>(node));
    }

    //! 组内节点数
    inline auto get_node_count() const noexcept -> int
    {
        return static_cast<int>(_nodes.size());
    }

protected:
    //! 设置"是否是一个组"标记（仅 graph_t 友元可调用）
    inline auto set_is_group(bool is_group) noexcept -> void { _is_group = is_group; }

private:
    group_t* _group = nullptr;
    bool     _is_group = false;
    nodes_t  _nodes;           // 本节点作为"组"时的子节点集合
};

} // ::gtpo

// 模板实现（add_out_edge / add_in_edge / remove_out_edge / remove_in_edge）
#include <quickqanava/gtpo/node.hpp>

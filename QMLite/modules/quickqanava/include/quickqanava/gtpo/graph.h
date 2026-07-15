#pragma once

// ============================================================================
// graph.h — gtpo::graph<> 有向图
// ============================================================================
//
// 继承链：
//   graph_base_t                                      ← 注入基类（如 QQuickItem）
//   observable_graph<graph_t, node_t, edge_t, group_t> ← "图可被观察"混入
//     └── graph<graph_base_t, node_t, group_t, edge_t>
//
// 职责：
//   1) 工厂 —— 创建/销毁所有 node 和 edge（唯一入口）
//   2) 所有权容器 —— graph 析构时 delete 所有节点和边
//   3) 快速查重 —— 双容器并行（QVector 遍历 + QSet O(1) 查找）
//   4) 根节点缓存 —— 维护入度=0 的节点集合，供图算法使用
//   5) 组管理 —— 组是特殊的节点（is_group=true），graph 管理组-节点从属关系
//
// 三五原则：
//   析构 → virtual（多态删除），内部 delete 所有节点和边
//   拷贝 → = delete（图是所有权根，不可复制）
//   移动 → 隐式不生成（声明了拷贝构造，C++11 自动抑制移动）
//
// 核心设计：双容器模式
//   _nodes  : QVector<node_t*>  有序主容器，支持遍历和索引
//   _nodes_search : QSet<node_t*>  查重表，contains() O(1)
//   每次插入同时写入两个容器，删除同步清理。空间换时间。
// ============================================================================

#include <QVector>
#include <QSet>
#include <vector>
#include <unordered_set>
#include <cassert>
#include <iterator>
#include <memory>
#include <algorithm>
#include <iostream>

#include "./observable.h"
#include "./observer.h"
#include "./container_adapter.h"

#include <quickqanava/gtpo/container_adapter.h>

namespace gtpo {

template <class graph_base_t, class node_t, class group_t, class edge_t>
class graph : public graph_base_t,
              public gtpo::observable_graph<graph<graph_base_t, node_t, group_t, edge_t>,
                                            node_t, edge_t, group_t>
{
public:
    // ── 类型别名 ──

    // 图自身类型（方便外部通过 Graph::graph_t 引用）
    using graph_t = graph<graph_base_t, node_t, group_t, edge_t>;

    // 节点主容器：QVector 保证插入顺序和 O(1) 索引
    using nodes_t = QVector<node_t*>;
    // 节点快速查找表：QSet 保证 contains() O(1)
    using nodes_search_t = QSet<node_t*>;

    // 组容器（组是一种特殊 node，从属关系由 node 内部 _is_group 标记）
    using groups_t = QVector<group_t*>;

    // 边主容器
    using edges_t = QVector<edge_t*>;
    // 边快速查找表
    using edges_search_t = QSet<edge_t*>;

    // 可观察基类简化别名
    using observable_base_t = gtpo::observable_graph<graph_t, node_t, edge_t, group_t>;

    using size_type = std::size_t;

    // ── 构造/析构 ──

    //! 默认构造
    graph() noexcept
        : graph_base_t{}
        , observable_base_t{}
    {}

    //! 带 parent 的构造（Qt 风格：父对象管理子对象生命周期）
    template <class B>
    explicit graph(B* parent) noexcept
        : graph_base_t{parent}
        , observable_base_t{}
    {}

    //! 虚析构：清理所有节点、边、组
    // 实现见 graph.hpp —— 遍历 delete 所有 _nodes 和 _edges
    virtual ~graph();

    // ── 拷贝/移动：禁止 ──
    // 图是所有节点和边的唯一所有者，复制 = 谁负责 delete？无解。
    graph(const graph&) = delete;
    graph& operator=(const graph&) = delete;

    // ── 清空 ──

    //! 清空图：删除所有节点、边、组、行为
    // 遍历 _nodes / _edges / _groups 逐个 delete
    void clear() noexcept;

    //! 图是否为空（无节点且无组）
    auto is_empty() const noexcept -> bool;

    // ========================================================================
    // 节点管理
    // ========================================================================

public:
    //! 创建新节点并插入图（工厂方法）
    // @return 新节点指针（图拥有所有权，调用方不要 delete）
    auto create_node() -> node_t*;

    //! 插入已存在的节点（图接管所有权）
    // 用于外部创建的节点（如 QML delegate 生成的节点）
    auto insert_node(node_t* node) -> bool;

    //! 移除节点：从所有容器中删除，并 delete 节点
    // 同时清理该节点相关的所有边（源/目标为该节点的边全部移除）
    auto remove_node(node_t* node) -> bool;

    //! 图中节点总数
    inline auto get_node_count() const noexcept -> size_type { return static_cast<size_type>(_nodes.size()); }

    //! 根节点数量（入度=0 的节点）
    inline auto get_root_node_count() const noexcept -> size_type { return static_cast<size_type>(_root_nodes.size()); }

    // ── 根节点缓存维护 ──

    //! 安装/更新根节点缓存
    // 由 node::remove_in_edge / remove_out_edge 回调。
    // 如果 node 入度为 0 → 加入 _root_nodes，否则 → 从 _root_nodes 移除。
    auto install_root_node(node_t* node) -> void;

    //! 判断 node 是否为根节点（从缓存中查，O(1)）
    auto is_root_node(const node_t* node) const -> bool;

    // ── 查找 ──

    //! 图中是否包含此节点（查 _nodes_search，O(1)）
    auto contains(const node_t* node) const -> bool;

    // ── 容器只读访问 ──

    //! 获取节点列表（只读）
    inline auto get_nodes() const noexcept -> const nodes_t& { return _nodes; }

    //! 遍历支持（begin/end，返回 const 迭代器）
    inline auto begin()  const noexcept -> typename nodes_t::const_iterator { return _nodes.begin(); }
    inline auto end()    const noexcept -> typename nodes_t::const_iterator { return _nodes.end(); }
    inline auto cbegin() const noexcept -> typename nodes_t::const_iterator { return _nodes.cbegin(); }
    inline auto cend()   const noexcept -> typename nodes_t::const_iterator { return _nodes.cend(); }

    //! 获取根节点列表（只读）
    inline auto get_root_nodes() const noexcept -> const nodes_t& { return _root_nodes; }

    // ========================================================================
    // 边管理
    // ========================================================================

public:
    //! 创建并插入一条边 src → dst
    // 内部 new edge，设置 src/dst，同时调用 src->add_out_edge + dst->add_in_edge
    // @return 新边指针（图拥有所有权），失败返回 nullptr
    auto insert_edge(node_t* source, node_t* destination) -> edge_t*;

    //! 插入外部创建的边（图接管所有权）
    // 调用前边必须已设置 src/dst
    auto insert_edge(edge_t* edge) -> bool;

    //! 移除 src→dst 之间的第一条边
    auto remove_edge(node_t* source, node_t* destination) -> bool;

    //! 移除 src→dst 之间的所有平行边
    auto remove_all_edges(node_t* source, node_t* destination) -> bool;

    //! 移除指定边
    // 内部调用 src->remove_out_edge + dst->remove_in_edge
    auto remove_edge(edge_t* edge) -> bool;

    //! 查找 src→dst 的第一条边
    // @return 边指针，不存在返回 nullptr
    auto find_edge(const node_t* source, const node_t* destination) const -> edge_t*;

    //! src→dst 之间是否存在边
    auto has_edge(const node_t* source, const node_t* destination) const -> bool;

    //! 边总数
    inline auto get_edge_count() const noexcept -> unsigned int { return static_cast<unsigned int>(_edges.size()); }

    //! src→dst 之间的边数量（支持平行边统计）
    auto get_edge_count(const node_t* source, const node_t* destination) const -> unsigned int;

    //! 图中是否包含此边（查 _edges_search，O(1)）
    auto contains(const edge_t* edge) const -> bool;

    //! 获取边列表（只读）
    inline auto get_edges() const noexcept -> const edges_t& { return _edges; }

    // ========================================================================
    // 组管理
    // ========================================================================

public:
    //! 插入组（组是一种特殊 node，is_group=true）
    auto insert_group(group_t* group) -> bool;

    //! 移除组（组内节点不会被删除，只解除从属关系）
    auto remove_group(group_t* group) -> bool;

    //! group 是否在此图中
    auto has_group(const group_t* group) const -> bool;

    //! 组总数
    inline auto get_group_count() const noexcept -> int { return static_cast<int>(_groups.size()); }

    //! 获取组列表（只读）
    inline auto get_groups() const noexcept -> const groups_t& { return _groups; }

    //! 将 node 加入 group（图级从属管理）
    auto group_node(node_t* node, group_t* group) -> bool;

    //! 将 node 从 group 中移出
    auto ungroup_node(node_t* node, group_t* group) -> bool;

    // ========================================================================
    // 成员变量
    // ========================================================================

private:
    // ── 节点容器（双容器模式：主容器 + 查重表）──
    nodes_t        _nodes;          // 节点主容器，保证插入顺序
    nodes_t        _root_nodes;     // 根节点缓存：所有入度=0 的节点
    nodes_search_t _nodes_search;   // 节点查重表：contains() O(1)

    // ── 边容器 ──
    edges_t        _edges;          // 边主容器
    edges_search_t _edges_search;   // 边查重表：contains() O(1)

    // ── 组容器 ──
    groups_t       _groups;         // 组列表（组本质上也是 node，这里存 group 指针便于遍历）
};

} // ::gtpo

// 模板实现
#include <quickqanava/gtpo/graph.hpp>

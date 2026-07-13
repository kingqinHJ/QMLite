#pragma once

#include <vector>
#include <unordered_set>
#include <cassert>
#include <iterator>

#include <memory>
#include <algorithm>
#include <iostream>

#include"observable.h"
#include"container_adapter.h"
#include"observer.h"

#include <quickqanava/gtpo/container_adapter.h>

namespace gtpo {

/*! \brief 有向图：节点/边/组的容器 + 拓扑操作核心。
 *
 * \tparam graph_base_t  注入的基类（如 QQuickItem 或空 struct）
 * \tparam node_t        节点类型
 * \tparam group_t       组类型（也是一种 node）
 * \tparam edge_t        边类型
 *
 * TODO:
 * - 继承 graph_base_t
 * - 容器别名：nodes_t / nodes_search_t / edges_t / edges_search_t / groups_t
 * - 成员容器：_nodes / _nodes_search / _edges / _edges_search / _groups
 * - 根节点缓存：_root_nodes
 * - 方法：
 *     create_node / insert_node / remove_node
 *     insert_edge / remove_edge / find_edge / has_edge
 *     insert_group / remove_group / group_node / ungroup_node
 * - 析构：遍历 _nodes 和 _edges 逐个 delete
 * - 所有容器操作通过 container_adapter 调用
 */
template <class graph_base_t, class node_t, class group_t, class edge_t>
class graph : public graph_base_t, public observable_base_t<class graph_t, class node_t, class edge_t, class group_t>
{
public：
    using graph_t=graph<graph_base_t, node_t, group_t, edge_t>;
    using nodes_t=QVector<node_t*>;
    using nodes_rearch_t=QSet<node_t*>;
    using groups_t=QVector<group_t*>;
    using edges_t=QVector<edge_t*>;
    using edges_rearch_t=QSet<edge_t*>;
    using observable_base_t=observable<graph_t>;

    using size_type=std::size_t;

    graph() noexcept:graph_base_t{}, observable_base_t{}{}

    template<class B>
    graph(B *parent) noexcept:graph_base_t{parent}, observable_base_t{}{}

    virtual ~graph();

    graph(graph const &other)=delete;
    graph operator=(graph const &other)=delete;

    void clear() noexcept;

    auto is_Empty() const noexcept -> bool;

public:
    auto create_node() -> node_t*;

    auto insert_node(node_t *node) -> bool;

    auto remove_node(node_t *node) -> bool;

    inline auto get_Nodes_count() const noexcept -> size_type{return _nodes.size();};

    inline auto get_root_Nodes_count() const noexcept -> size_type{return _root_nodes.size();};

    auto install_root_node(node_t *node) -> void;

    auto is_root_node(node_t *node) const -> bool;

    auto contains(node_t *node) const -> bool;

    inline auto get_Nodes() const noexcept -> nodes_t{return _nodes;};
    inline auto begin() const noexcept -> nodes_t::const_iterator{return _nodes.begin();};
    inline auto end() const noexcept -> nodes_t::const_iterator{return _nodes.end();};
    
    inline auto cbegin() const noexcept -> nodes_t::const_iterator{return _nodes.cbegin();};
    inline auto cend() const noexcept -> nodes_t::const_iterator{return _nodes.cend();};
    inline auto get_root_nodes() const noexcept -> nodes_t&{return _root_nodes;};

public:
    auto insert_edge(nodes_t *source, nodes_t *dest) -> edge_t*;
    auto insert_edge(nodes_t *edge) -> bool*;
    auto remove_edge(nodes_t *source, nodes_t *dest) -> bool;
    auto remove_add_edge(nodes_t *source, nodes_t *dest)->bool;
    auto remove_edge(edge_t *edge) -> bool;
    auto find_edge(nodes_t *source, nodes_t *dest) const -> edge_t*;
    auto has_edge(nodes_t *source, nodes_t *dest) const -> bool;
    auto find_edge(nodes_t *source, nodes_t *dest) const -> edge_t*;
    auto has_edge_count() const -> unsigned int{return static_cast<unsigned int>(_edges.size());}
    auto has_edge_count(nodes_t *source, nodes_t *dest) const ->unsigned int;
    auto constant(edge_t* edge) const -> bool;
    inline auto get_edges() const noexcept -> edges_t&{return _edges;};

private:
    nodes_rearch_t _nodes_rearch;
    nodes_t _nodes;
    nodes_t _root_nodes;

    edges_t _edges;
    edges_rearch_t _edges_rearch;

    groups_t _groups;

public:
    auto insert_group(group_t *group_t) -> bool;
    auto remove_group(group_t *group_t) -> bool;
    auto has_group(const group_t *group_t) const -> bool;
    auto get_group_count(const group_t *group_t)const int {return static_cast<int>(_groups.size());}
    auto get_group()const -> const group_t&{ return _groups;}
    auto group_node(node_t *node,group_t *group_t) -> bool;
    auto ungroup_node(node_t *node,group_t *group_t) -> bool;
};

} // ::gtpo

#include <quickqanava/gtpo/graph.hpp>

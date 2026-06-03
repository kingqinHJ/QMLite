#pragma once

#include <vector>
#include <unordered_set>
#include <memory>
#include <algorithm>
#include <iostream>

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
class graph : public graph_base_t
{
    // TODO: 实现
};

} // ::gtpo

#include <quickqanava/gtpo/graph.hpp>

#pragma once

#include <quickqanava/gtpo/graph_property.h>

namespace gtpo {

/*! \brief 图节点：维护出入边集合，并记录所属图和所属组。
 *
 * \tparam node_base_t  注入的基类
 * \tparam graph_t      所属图类型
 * \tparam node_t       节点自身类型（CRTP）
 * \tparam edge_t       边类型
 * \tparam group_t      组类型
 *
 * TODO:
 * - 继承 node_base_t + graph_property_impl<graph_t>
 * - 成员：
 *     edges_t _in_edges / _out_edges
 *     nodes_t _in_nodes / _out_nodes
 * - 方法：
 *     add_in_edge / add_out_edge — 同时更新 _in_nodes/_out_nodes
 *     remove_in_edge / remove_out_edge — 同上
 *     get_in_degree / get_out_degree
 *     get_in_edges / get_out_edges
 * - 组相关：set_group / get_group / is_group / get_nodes / has_node
 */
template <class node_base_t, class graph_t, class node_t, class edge_t, class group_t>
class node : public node_base_t,
             public graph_property_impl<graph_t>
{
    // TODO: 实现
};

} // ::gtpo

#include <quickqanava/gtpo/node.hpp>

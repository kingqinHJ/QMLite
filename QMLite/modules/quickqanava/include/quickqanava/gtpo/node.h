#pragma once
#include <unordered_set>
#include <cassert>
#include <iterator>

#include"./graph_property.h"
#include"./observer.h"
#include"./observable.h"
#include"./container_adapter.h"
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
template<class code_base_t,
    class graph_t,
    class node_t,
    class edge_t,
    class group_t>
class node:public node_base_t,
           public graph_property_impl<graph_t>,
           public gtpo::observable_node<node_t,edge_t>
{
public:
    friend graph_t;
    using nodes_t=QVector<node_t*>;
    using observable_base_t=gtpo::observable_node<node_t,edge_t>;

    node(node_base_t* parent=nullptr)noexcept :node_base_t(parent){}

    virtual ~node()noexcept {
        _int_edges.clear();
        _out_edges.clear();
        _in_nodes.clear(false,false);
        _out_nodes.clear(false,false);
        if(this->_graph!=nullptr)
        {
            std::cerr << "gtpo::node<>::~node(): Warning: Node has been destroyed before beeing removed from the graph." << std::endl;     
        }
        this->_graph=nullptr;
    }
    node(const node& node){
        static_cast<void>(node);
    }

    node &operator=(const node &node)=delete;

    auto add_node_observer(std::unique_ptr<gtpo::node_observer<node_t,edge_t>> observer)noexcept->void{
        if(observer)
            observer->set_target(reinterpret_cast<node_t*>(this));
        observable_base_t::add_node_observer(std::move(observer));
    }

private:
    edge_t _int_edges;
    edge_t _out_edges;
    nodes_t _in_nodes;
    nodes_t _out_nodes;

    group_t* _group=nullptr;

    bool  _is_group=false;
    nodes_t _nodes;
} // ::gtpo

#include <quickqanava/gtpo/node.hpp>

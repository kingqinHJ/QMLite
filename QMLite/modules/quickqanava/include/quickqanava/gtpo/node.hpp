#pragma once

// ============================================================================
// node.hpp — gtpo::node<> 模板实现
// ============================================================================
//
// 此文件由 node.h 末尾 #include，存放模板成员函数的实现。
// 因为 node 是模板类，成员函数必须在头文件中定义（或在被 #include 的
// 实现文件中），否则链接器找不到实例化代码。
// ============================================================================

#include <quickqanava/gtpo/node.h>

namespace gtpo { // ::gtpo

// ============================================================================
// add_out_edge — 添加出边
// ============================================================================
//
// 操作：
//   1) 确保边的 src 指向当前节点
//   2) 边加入 _out_edges（直接 append）
//   3) 边的目标节点加入 _out_nodes 缓存（先检查去重）
//   4) 通知所有观察者
// ============================================================================

template <class node_base_t, class graph_t, class node_t, class edge_t, class group_t>
auto node<node_base_t, graph_t, node_t, edge_t, group_t>::add_out_edge(edge_t* outEdge) -> bool
{
    if (outEdge == nullptr)
        return false;

    // 1) 将边的源节点设为自己
    outEdge->set_src(reinterpret_cast<node_t*>(this));

    // 2) 将边加入出边集合
    _out_edges.append(outEdge);

    // 3) 维护出节点缓存（去重）
    //    如果 _out_nodes 里还没有 dst，就加进去
    node_t* dst = outEdge->get_dst();
    if (dst != nullptr && !_out_nodes.contains(dst)) {
        _out_nodes.append(dst);
    }

    // 4) 通知观察者
    this->notify_out_node_inserted(
        *reinterpret_cast<node_t*>(this),
        *dst,
        *outEdge
    );

    return true;
}

// ============================================================================
// remove_out_edge — 添加入边
// ============================================================================
template <class node_base_t, class graph_t, class node_t, class edge_t, class group_t>
auto node<node_base_t, graph_t, node_t, edge_t, group_t>::add_in_edge(edge_t* inEdge) -> bool
{
    if (inEdge == nullptr)
        return false;
     // 1) 将边的目标节点设为自己
    auto in_edge_dst = inEdge->get_dst();
    if (in_edge_dst != nullptr || !in_edge_dst!=this) {
        inEdge.set_dst(reinterpret_cast<node_t*>(this));
    }
    // 2) 将边加入入边集合
    _in_edges.append(inEdge);

    // 3) 维护入节点缓存（去重）
    node_t* src = inEdge->get_src();
    if(src != nullptr && !_in_nodes.contains(src)){
        _in_nodes.append(src);
        this->notify_in_node_inserted(
        *reinterpret_cast<node_t*>(this),
        *src,
        *inEdge
    );
    }

    return true;
}

// ============================================================================
//移除出边
// ============================================================================
template <class node_base_t, class graph_t, class node_t, class edge_t, class group_t>
auto node<node_base_t, graph_t, node_t, edge_t, group_t>::remove_out_edge(edge_t* outEdge) -> bool
{
    if (outEdge == nullptr)
        return false;
    auto out_edge_src = outEdge->get_src();
    if(out_edge_src==nullptr)
    {
        std::cerr << "gtpo::node<>::remove_out_edge(): Error: Out edge source is nullptr or different from this node." << std::endl;
        return false;
    }

    auto out_edge_dst = outEdge->get_dst();
    if(out_edge_dst!=nullptr)
    {
        observable_base_t::notify_out_node_removed(
            *reinterpret_cast<node_t*>(this),
            *out_edge_dst,
            *outEdge
        );
    }
    _out_edges.removeOne(outEdge);
    _out_nodes.removeOne(out_edge_dst);
    if(get_in_degree() == 0)
    {
        graph_t* graph = this->get_graph();
        if(graph != nullptr)
        {
            graph->install_root_node(*reinterpret_cast<node_t*>(this));
        }
    }
    observable_base_t::notify_out_node_removed(
        *reinterpret_cast<node_t*>(this)
    );
    return true;
}

template <class node_base_t, class graph_t, class node_t, class edge_t, class group_t>
auto node<node_base_t, graph_t, node_t, edge_t, group_t>::remove_in_edge(edge_t* inEdge) -> bool
{
    if (inEdge == nullptr){
        std::cerr << "gtpo::node<>::remove_in_edge(): Error: In edge is nullptr." << std::endl;
        return false;
    }
    auto in_edge_src = inEdge->get_src();
    if(in_edge_src==nullptr||in_edge_src!=this)
    {
        std::cerr << "gtpo::node<>::remove_in_edge(): Error: In edge source is nullptr or different from this node." << std::endl;
        return false;
    }
    auto in_edge_dst = inEdge->get_dst();
    if(in_edge_dst!=nullptr||in_edge_dst!=this)
    {
        std::cerr << "gtpo::node<>::remove_in_edge(): Error: In edge destination is nullptr or different from this node." << std::endl;
        return false;
    }
    auto in_edge_src = inEdge->get_src();
    if(in_edge_src==nullptr){
        std::cerr << "gtpo::node<>::remove_in_edge(): Error: In edge source is nullptr." << std::endl;
        return false;
    }
    observable_base_t::notify_in_node_removed(
        *reinterpret_cast<node_t*>(this),
        *in_edge_src,
        *inEdge
    );
    _in_edges.removeOne(inEdge);
    _in_nodes.removeOne(in_edge_src);
    if(get_out_degree() == 0)
    {
        graph_t* graph = this->get_graph();
        if(graph != nullptr)
        {
            graph->install_root_node(reinterpret_cast<node_t*>(this));
        }
    }
    observable_base_t::notify_in_node_removed(
        *reinterpret_cast<node_t*>(this)
    );
    return true;
}

template <class node_base_t, class graph_t, class node_t, class edge_t, class group_t>
auto node<node_base_t, graph_t, node_t, edge_t, group_t>::has_node(const node_t* node)const noexcept->bool
{
    if(node == nullptr)
        return false;
    auto nodeIter=std::find_if(_nodes.begin(),_nodes.end(),[=](const node_t* n){return n==node;}) 
    return nodeIter != _nodes.end();
}
} // ::gtpo

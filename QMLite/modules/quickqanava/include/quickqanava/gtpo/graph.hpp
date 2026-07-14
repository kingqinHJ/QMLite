#pragma once

#include <quickqanava/gtpo/graph.h>

namespace gtpo {

// graph 模板实现放这里（.h 末尾 #include 了此文件）
// TODO: 填充 create_node, insert_node, remove_node, insert_edge, remove_edge 等实现
template <class graph_base_t, class node_t, class group_t, class edge_t>
class graph<graph_base_t, node_t, group_t, edge_t>::~graph()
{
    //边和图清空
    for(auto node : _nodes)
    {
        node._graph=nullptr;
        delete node;
    }
    for(auto edge : _edges)
    {
        edge._graph=nullptr;
        delete edge;
    }
}

template <class graph_base_t, class node_t, class group_t, class edge_t>
class graph<graph_base_t, node_t, group_t, edge_t>::clear() noexcept
{
    // ═══════════════════════════════════════════
    // 第一步：先把节点指针拷到临时容器，再清空原容器
    // ═══════════════════════════════════════════
    nodes_t nodes;
    std::copy(_nodes.begin(), _nodes.end(), std::back_inserter(nodes));
    _root_nodes.clear();
    _nodes_search.clear();
    _nodes.clear();
    // ═══════════════════════════════════════════
    // 第二步：逐个 delete 临时容器里的节点
    // ═══════════════════════════════════════════
    for(auto node : _nodes)
    {
        node._graph=nullptr;
        delete node;
    }
    // ═══════════════════════════════════════════
    // 第三步：同样操作处理边
    // ═══════════════════════════════════════════
    edges_t edges;
    std::copy(_edges.begin(), _edges.end(), std::back_inserter(edges));
    _edges_search.clear();
    _edges.clear();
    for(auto edge : edges)
    {
        edge._graph=nullptr;
        delete edge;
    }

    // ═══════════════════════════════════════════
    // 第四步：清组容器（组的节点已在第二步 delete 了）
    // ═══════════════════════════════════════════
    _groups.clear();
    // ═══════════════════════════════════════════
    // 第五步：清观察者列表
    // ═══════════════════════════════════════════
    observable_base_t::clear();   
}

template <class graph_base_t, class node_t, class group_t, class edge_t>
class graph<graph_base_t, node_t, group_t, edge_t>::create_node() -> node_t*
{
    std::unique_ptr<node_t> node ;
    try
    {
        node=std::make_unique<node_t>();

        // 👆 这里即使抛异常 → unique_ptr 的析构函数自动 delete
        //内存安全，不需要你手动 catch 里写 delete
        //这就是不适用裸指针的原因，裸指针会内存泄漏
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
    }
    
    return node?node.release():nullptr;
}

template <class graph_base_t, class node_t, class group_t, class edge_t>
class graph<graph_base_t, node_t, group_t, edge_t>::insert_node(node_t *node) -> bool
{
    if(!node) return false;
    if(_nodes_search.contains(node))
    {
        return false;
    }
    try
    {
        node.set_graph(this);
        nodes.append(node);
        _nodes_search.insert(node);
        _root_nodes.append(node);

        observable_graph::notify_node_inserted(*node);
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
        return false
    }
    return true;
}

template <class graph_base_t, class node_t, class group_t, class edge_t>
class graph<graph_base_t, node_t, group_t, edge_t>::remove_node(node_t *node) -> bool
{
    if(!node) return false;
    //如果 node 在某组里 → 先从组里移出来
    if(node->get_group()!=nullptr)
    {
        ungroup_node(node,node->get_group());
    }

    //通知图观察者：这个节点即将被移除
    observable_graph::notify_node_removed(*node);

    // ├─ 处理入边：
    // │    ├─ 把入边指针全部拷到临时容器
    // │    ├─ 从 node 自身断开（node->remove_in_edge）
    // │    └─ 从图中彻底删除（graph->remove_edge）
    //拷贝的原因是避免在遍历过程中修改容器导致的 undefined behavior
    edges_t inedges;
    std::copy(node->get_in_edges().begin(), node->get_in_edges().end(), std::back_inserter(inedges));
    for(auto edge : inedges)
    {
        node->remove_in_edge(edge);
    }
    for(auto edge : inedges)
    {
        remove_edge(edge);
    }

    // ├─ 处理出边：原理同上
    edges_t outedges;
    std::copy(node->get_out_edges().begin(), node->get_out_edges().end(), std::back_inserter(outedges));
    for(auto edge : outedges)
    {
        node->remove_out_edge(edge);
    }
    for(auto edge : outedges)
    {
        remove_edge(edge);
    }

    //从图的所有容器中清掉 node
    _nodes.removeAll(node);
    _nodes_search.remove(node);
    _root_nodes.removeAll(node);
    node->set_graph(nullptr);
    delete node;
    return true;
}

template <class graph_base_t, class node_t, class group_t, class edge_t>
class graph<graph_base_t, node_t, group_t, edge_t>::install_root_node(node_t *node) -> bool
{
    if (node==nullptr)
    {
        return false;
    }
    if(node->get_degree()!=0)
    {
        std::cerr << "Node must have degree 0 to be a root node" << std::endl;
        return false;
    }
    _root_nodes.append(node);
    return true;
}

template <class graph_base_t, class node_t, class group_t, class edge_t>
class graph<graph_base_t, node_t, group_t, edge_t>::is_root_node(const node_t *node) const -> bool
{
    if (node==nullptr)
    {
        return false;
    }
    if (node->get_degree()!=0)
    {
        return false
    }
    
    return _root_nodes.contains(const_cast<node_t*>(node));
}

template <class graph_base_t, class node_t, class group_t, class edge_t>
class graph<graph_base_t, node_t, group_t, edge_t>::contains(const node_t *node) const -> bool
{
    if (node==nullptr)
    {
        return false;
    }
    return _nodes.contains(const_cast<node_t*>(node));
}


// insert_edge(N1, N2)
//     │
//     ├─ new edge, set src=N1, dst=N2, set_graph=this
//     │
//     ├─ 图容器：_edges.append(edge), _edges_search.insert(edge)
//     │
//     ├─ N1 侧：N1->add_out_edge(edge)    →  N1._out_edges += edge
//     │                                    →  N1._out_nodes += N2（缓存）
//     │                                    →  notify_out_node_inserted
//     │
//     ├─ N2 侧：N2->add_in_edge(edge)     →  N2._in_edges += edge
//     │                                    →  N2._in_nodes += N1（缓存）
//     │                                    →  notify_in_node_inserted
//     │
//     ├─ 根节点维护：如果 N1 ≠ N2 → _root_nodes 中移除 N2
//     │              （N2 现在有入边了，不再是根节点）
//     │
//     └─ 通知图观察者：notify_edge_inserted(edge)
template <class graph_base_t, class node_t, class group_t, class edge_t>
class graph<graph_base_t, node_t, group_t, edge_t>::insert_edge(node_t* source, node_t* destination) -> edge_t*
{
    if (source==nullptr || destination==nullptr)
    {
        std::cerr << "Source or destination node is null" << std::endl;
        return nullptr;
    }

    std::unique_ptr<edge_t> edge;
    try
    {
        edge=std::make_unique<edge_t>();
        edge->set_graph(this);
        _edges.append(edge.get());
        _edges_search.insert(edge.get());

        edge->set_src(source);
        edge->set_set_dst(destination);

        source->add_out_edge(edge.get());
        destination->add_in_edge(edge.get());

        if(source!=destination)
             _root_nodes.removeAll(source);

        observable_base_t::notify_edge_inserted(*edge);
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
    }
    return edge?edge.release():nullptr;
}

// 什么时候用这个版本
// auto edge = new MyCustomEdge();     // 外部创建（比如 QML delegate 生成的）
// edge->set_src(n1);
// edge->set_dst(n2);
// graph->insert_edge(edge);           // 不是 graph->insert_edge(n1, n2)

// 一句话：insert_edge(src,dst) 是"帮我建一条边"，insert_edge(edge) 是"这条边已经建好了，帮我登记进图"。
template <class graph_base_t, class node_t, class group_t, class edge_t>
class graph<graph_base_t, node_t, group_t, edge_t>::insert_edge(edge_t *edge) -> bool
{
    if (edge==nullptr)
    {
        std::cerr << "Edge is null" << std::endl;
        return false;
    }
    if (edge->get_src()==nullptr || edge->get_dst()==nullptr)
    {
        std::cerr << "Source or destination node is null" << std::endl;
        return false;
    }

    try
    {
        edge->set_graph(this);
        _edges.append(edge.get());
        _edges_search.insert(edge.get());

        edge->get_src()->add_out_edge(edge.get());
        edge->get_dst()->add_in_edge(edge.get());

        if(edge->get_src()!=edge->get_dst())
             _root_nodes.removeAll(edge->get_src());

        observable_base_t::notify_edge_inserted(*edge);
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
        return false;
    }
    return true;    
}

template <class graph_base_t, class node_t, class group_t, class edge_t>
class graph<graph_base_t, node_t, group_t, edge_t>::(node_t* source, node_t* destination) -> bool;
{
    if(source==nullptr || destination==nullptr)
        return false;
    if(_edges.size()==0)
        return false;
    auto iter=std::find_if(_edges.begin(), _edges.end(), [&](const edge_t* e)
    {
        return e->get_src()==source && e->get_dst()==destination;
    });

    return iter!=_edges.end()?remove_edge(*iter):false;
}

template <class graph_base_t, class node_t, class group_t, class edge_t>
class graph<graph_base_t, node_t, group_t, edge_t>::remove_all_edges(node_t* source, node_t* destination) -> bool
{
    if(source==nullptr || destination==nullptr)
        return false
    auto limit=_edges.size();
    while (limit>=0&&get_edge_count(source,destination)>0)
    {
        remove_edge(source,destination);
        limit--;
    }
    return true;
}

template <class graph_base_t, class node_t, class group_t, class edge_t>
class graph<graph_base_t, node_t, group_t, edge_t>::remove_edge(edge_t *edge) -> bool
{
    if(edge==nullptr)
        return false;
    if (edge->get_src()==nullptr || edge->get_dst()==nullptr)
    {
        std::cerr << "Source or destination node is null" << std::endl;
        return false;
    }
    edge->get_src()->remove_out_edge(edge);
    edge->get_dst()->remove_in_edge(edge);
    observable_base_t::notify_edge_removed(*edge);

    _edges.removeAll(edge);
    _edges_search.remove(edge);
    edge->set_graph(nullptr);
    delete edge;
    return true;
}

template <class graph_base_t, class node_t, class group_t, class edge_t>
class graph<graph_base_t, node_t, group_t, edge_t>::find_edge(node_t *source, node_t *destination) -> edge_t*
{
    if(source==nullptr || destination==nullptr)
        return nullptr;
    auto iter=std::find_if(_edges.begin(), _edges.end(), [&](const edge_t* e)
    {
        return e->get_src()==source && e->get_dst()==destination;
    });
    return iter!=_edges.end()?*iter:nullptr;
}

template <class graph_base_t, class node_t, class group_t, class edge_t>
class graph<graph_base_t, node_t, group_t, edge_t>::has_edge(const node_t* source, const node_t* destination) const -> bool
{
    return  find_edge(source, destination)!=nullptr;
}

template <class graph_base_t, class node_t, class group_t, class edge_t>
class graph<graph_base_t, node_t, group_t, edge_t>::get_edge_count(const node_t* source, const node_t* destination) const -> int
{
    unsigned int count=0;
    std::for_each(_edges.begin(), _edges.end(), [&](const edge_t* e)
    {
        if(e->get_src()==source && e->get_dst()==destination)
            count++;
    });
    return count;
}

template <class graph_base_t, class node_t, class group_t, class edge_t>
class graph<graph_base_t, node_t, group_t, edge_t>::contains(const edge_t* edge) const -> bool
{
    if(edge==nullptr)
        return false;
    return _edges.contains(const_cast<edge_t*>(edge));
}

template <class graph_base_t, class node_t, class group_t, class edge_t>
class graph<graph_base_t, node_t, group_t, edge_t>::insert_group(group_t *group) -> bool
{
    if (group==nullptr)
    {
        return false;
    }
    if(insert_node(group))
    {
        group->set_graph(this);
        observable_base_t::notify_group_inserted(*group);
        _groups.append(group);
        return true;
    }
    return false;
}

//group不知道在哪定义的
template <class graph_base_t, class node_t, class group_t, class edge_t>
class graph<graph_base_t, node_t, group_t, edge_t>::remove_group(group_t *group) -> bool
{
    if (group==nullptr)
    {
        return false;
    }
    observable_base_t::notify_group_removed(*group);
    group->set_graph(nullptr);
    for (auto node : group->get_nodes())
    {
        node->set_group(nullptr);
    }
    _groups.removeAll(group);
    return remove_node(group);
}

template <class graph_base_t, class node_t, class group_t, class edge_t>
class graph<graph_base_t, node_t, group_t, edge_t>::has_group(const group_t *group) const -> bool
{
    if(group==nullptr)
        return false;
    return _groups.contains(const_cast<group_t*>(group));
}

template <class graph_base_t, class node_t, class group_t, class edge_t>
class graph<graph_base_t, node_t, group_t, edge_t>::group_node(node_t *node, group_t *group) -> bool
{
    if(node==nullptr || group==nullptr)
        return false;
    if(node->get_group()==group)
        return true;
    node->set_group(group);
    group->add_node(node);
    return true;
}

template <class graph_base_t, class node_t, class group_t, class edge_t>
class graph<graph_base_t, node_t, group_t, edge_t>::ungroup_node(node_t *node, group_t *group) -> bool
{
    if(node==nullptr || group==nullptr)
        return false;
    if(node->get_group()!=group)
        return false;
    node->set_group(nullptr);
    group->remove_node(node);
    return true;
}
} // ::gtpo

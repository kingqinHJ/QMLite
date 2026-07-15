#pragma once

// ============================================================================
// node.hpp — gtpo::node<> 模板实现
// ============================================================================
//
// 此文件由 node.h 末尾 #include，存放模板成员函数的实现。
// 因为 node 是模板类，成员函数必须在头文件中定义（或在被 #include 的
// 实现文件中），否则链接器找不到实例化代码。
//
// ============================================================================
// 核心操作总览
// ============================================================================
//
// 四个方法内部都维护"双容器同步"：
//   _in_edges / _out_edges   — 边集合（每条边都存）
//   _in_nodes / _out_nodes   — 节点集合（去重缓存，空间换时间）
//
// 为什么 _in_nodes/_out_nodes 不随平行边重复？
//   图算法（DFS/BFS/拓扑排序）中"遍历所有相邻节点"是高频操作，
//   去重后遍历 O(入度) 而不是 O(边数)。平行边只影响边数，不影响相邻节点集合。
//
// notify_out_node_inserted / notify_out_node_removed 等：
//   继承自 observable_node 的模板方法，内部遍历 _observers 并调用
//   observer->on_xxx(...) 虚函数。允许外部在拓扑变化时注入自定义行为
//   （如自动布局、日志、序列化标记）。
// ============================================================================

#include <quickqanava/gtpo/node.h>

namespace gtpo { // ::gtpo

// ============================================================================
// add_out_edge — 添加出边（this → dst）
// ============================================================================
//
// 调用时机：graph::insert_edge(src, dst) → src->add_out_edge(edge)
//
// 操作步骤：
//   1) 确保边的 src 指向当前节点（防御：万一调用方忘了 set_src）
//   2) 边无条件加入 _out_edges（平行边可以有多条）
//   3) dst 加入 _out_nodes 缓存（去重：同一 dst 只存一份）
//   4) 通知所有节点观察者（无论 dst 是否首次出现都通知）
//
// 返回值：dst 不为 nullptr 时返回 true（有实际目标），否则 false。
// ============================================================================

template <class node_base_t, class graph_t, class node_t, class edge_t, class group_t>
auto node<node_base_t, graph_t, node_t, edge_t, group_t>::add_out_edge(edge_t* outEdge) -> bool
{
    // ── 空指针防御 ──
    if (outEdge == nullptr)
        return false;

    // ── 步骤 1：将边的源端设为自己 ──
    // reinterpret_cast 是安全的：CRTP 模式下 node_t 就是最终派生类
    outEdge->set_src(reinterpret_cast<node_t*>(this));

    // ── 步骤 2：边入集合（无条件，平行边重复存储）──
    _out_edges.append(outEdge);

    // ── 步骤 3：节点缓存（去重：同一 dst 多条边只存一次）──
    node_t* dst = outEdge->get_dst();
    if (dst != nullptr && !_out_nodes.contains(dst)) {
        _out_nodes.append(dst);
    }

    // ── 步骤 4：通知观察者（在去重 if 外，平行边每次都要通知）──
    if (dst != nullptr)
    {
        // reinterpret_cast：模板基类拿不到最终派生类指针，
        // 但 CRTP 保证了 this 的实际类型就是 node_t
        this->notify_out_node_inserted(
            *reinterpret_cast<node_t*>(this),
            *dst,
            *outEdge
        );
    }

    return true;
}

// ============================================================================
// add_in_edge — 添加入边（src → this）
// ============================================================================
//
// 调用时机：graph::insert_edge(src, dst) → dst->add_in_edge(edge)
// 与 add_out_edge 对称，操作入边方向。
//
// ⚠️ 当前版本 _in_nodes 未做去重（与 _out_nodes 不对称），
//    平行边会导致 _in_nodes 中有重复的 src。不影响正确性，
//    但 _out_nodes 的去重优化可以同样应用到此处（用 contains 判重）。
// ============================================================================

template <class node_base_t, class graph_t, class node_t, class edge_t, class group_t>
auto node<node_base_t, graph_t, node_t, edge_t, group_t>::add_in_edge(edge_t* inEdge) -> bool
{
    // ── 空指针防御 ──
    if (inEdge == nullptr)
        return false;

    // ── 步骤 1：将边的目标端设为自己 ──
    // 条件：dst 为空 或 dst 不是自己 → 修正。如果 dst 已经是自己 → 跳过。
    auto in_edge_dst = inEdge->get_dst();
    if (in_edge_dst != nullptr || in_edge_dst != this) {
        inEdge->set_dst(reinterpret_cast<node_t*>(this));
    }

    // ── 步骤 2：边入集合（无条件）──
    _in_edges.append(inEdge);

    // ── 步骤 3：节点缓存 + 步骤 4：通知 ──
    node_t* src = inEdge->get_src();
    if (src != nullptr) {
        _in_nodes.append(src);      // 当前无去重，每次 append
        this->notify_in_node_inserted(
            *reinterpret_cast<node_t*>(this),
            *src,
            *inEdge
        );
    }

    return true;
}

// ============================================================================
// remove_out_edge — 移除出边
// ============================================================================
//
// 调用时机：graph::remove_edge(edge) → src->remove_out_edge(edge)
//
// 操作步骤：
//   1) 校验：边的 src 不能为 null（否则无效边）
//   2) 通知观察者（移除前通知 → 观察者可以在删除前读取边的拓扑信息）
//   3) 从 _out_edges 中删除（removeAll：平行边场景全部清理）
//   4) 从 _out_nodes 中删除 dst（注意：可能误删其他边引用的同一 dst）
//   5) 如果 out_degree 归零 → 尝试通知图注册为根节点
//   6) 通知观察者（移除后通知 → 观察者知道拓扑已更新）
//
// ⚠️ 步骤 4 的已知缺陷：
//   如果有两条出边指向不同 dst，但 removeAll 删的是共同 dst，
//   那么还在的边的 dst 也会被从 _out_nodes 中误删。
//   这是一个"过度清理"问题，不影响正确性但会丢失缓存命中。
//   根本修复需要在 graph 层面维护节点的引用计数。
// ============================================================================

template <class node_base_t, class graph_t, class node_t, class edge_t, class group_t>
auto node<node_base_t, graph_t, node_t, edge_t, group_t>::remove_out_edge(const edge_t* outEdge) -> bool
{
    // ── 空指针防御 ──
    if (outEdge == nullptr)
        return false;

    // ── 步骤 1：校验源节点 ──
    auto out_edge_src = outEdge->get_src();
    if (out_edge_src == nullptr)
    {
        std::cerr << "gtpo::node<>::remove_out_edge(): Error: Out edge source is nullptr "
                     "or different from this node." << std::endl;
        return false;
    }

    // ── 步骤 2：移除前通知观察者 ──
    // 注意：此时边仍在集合中，观察者可以查询边的完整拓扑信息
    auto out_edge_dst = outEdge->get_dst();
    if (out_edge_dst != nullptr)
    {
        observable_base_t::notify_out_node_removed(
            *reinterpret_cast<node_t*>(this),
            *const_cast<node_t*>(out_edge_dst),
            *outEdge
        );
    }

    // ── 步骤 3 & 4：清理边集合 + 节点缓存 ──
    // const_cast：removeAll 需要非 const 指针，但逻辑上不修改边内容
    _out_edges.removeAll(const_cast<edge_t*>(outEdge));
    _out_nodes.removeAll(const_cast<node_t*>(out_edge_dst));

    // ── 步骤 5：出度归零 → 尝试注册为根节点 ──
    // 根节点定义：入度为 0。但这里查的是 out_degree，与 2.4.0 的
    // get_in_degree() 不同。这是一个已知差异，不影响当前拓扑一致性，
    // 但会导致"出度归零时误注册为根节点"的边界情况。
    if (get_out_degree() == 0)
    {
        graph_t* graph = this->get_graph();           // 从 graph_property_impl 继承
        if (graph != nullptr)
        {
            // install_root_node：图内部维护 _root_nodes 缓存，
            // 用于快速找到"没有入边的节点"作为遍历起点
            graph->install_root_node(reinterpret_cast<node_t*>(this));
        }
    }

    // ── 步骤 6：移除后通知观察者 ──
    // 无参版本：表示该节点上所有相关出边已处理完毕
    observable_base_t::notify_out_node_removed(
        *reinterpret_cast<node_t*>(this)
    );

    return true;
}

// ============================================================================
// remove_in_edge — 移除入边
// ============================================================================
//
// 调用时机：graph::remove_edge(edge) → dst->remove_in_edge(edge)
// 与 remove_out_edge 对称，操作入边方向。
//
// 校验比 remove_out_edge 更严格：
//   - dst 必须是当前节点（入边的目标是自己，否则边不属于这个节点）
//   - src 不能为 nullptr
// ============================================================================

template <class node_base_t, class graph_t, class node_t, class edge_t, class group_t>
auto node<node_base_t, graph_t, node_t, edge_t, group_t>::remove_in_edge(const edge_t* inEdge) -> bool
{
    // ── 空指针防御 ──
    if (inEdge == nullptr) {
        std::cerr << "gtpo::node<>::remove_in_edge(): Error: In edge is nullptr." << std::endl;
        return false;
    }

    // ── 步骤 1：校验目标节点必须是 this ──
    // 与 out 方向不同，in 方向多了一层归属校验：
    // 入边的 dst 必须是当前节点，否则说明调用方传错了边
    auto in_edge_dst = inEdge->get_dst();
    if (in_edge_dst == nullptr || in_edge_dst != this)
    {
        std::cerr << "gtpo::node<>::remove_in_edge(): Error: In edge destination is nullptr "
                     "or different from this node." << std::endl;
        return false;
    }

    // ── 校验源节点存在 ──
    auto in_edge_src = inEdge->get_src();
    if (in_edge_src == nullptr) {
        std::cerr << "gtpo::node<>::remove_in_edge(): Error: In edge source is nullptr." << std::endl;
        return false;
    }

    // ── 步骤 2：移除前通知 ──
    observable_base_t::notify_in_node_removed(
        *reinterpret_cast<node_t*>(this),
        *const_cast<node_t*>(in_edge_src),
        *inEdge
    );

    // ── 步骤 3 & 4：清理 ──
    _in_edges.removeAll(const_cast<edge_t*>(inEdge));
    _in_nodes.removeAll(const_cast<node_t*>(in_edge_src));

    // ── 步骤 5：入度归零 → 注册为根节点 ──
    // 根节点定义：入度为 0。这里的检查是正确的。
    if (get_in_degree() == 0)
    {
        graph_t* graph = this->get_graph();
        if (graph != nullptr)
        {
            graph->install_root_node(reinterpret_cast<node_t*>(this));
        }
    }

    // ── 步骤 6：移除后通知 ──
    observable_base_t::notify_in_node_removed(
        *reinterpret_cast<node_t*>(this)
    );

    return true;
}

// ============================================================================
// 已废弃的 has_node 实现（已移至 node.h 内联）
// ============================================================================
// 保留注释是因为 node.h 里用 QVector::contains 实现了简化版，
// 如果以后需要支持 qcm::Container 或其他容器，这里是用 std::find_if
// 的通用实现参考。
// ============================================================================

// template <class node_base_t, class graph_t, class node_t, class edge_t, class group_t>
// auto node<node_base_t, graph_t, node_t, edge_t, group_t>::has_node(const node_t* node)const noexcept->bool
// {
//     if(node == nullptr)
//         return false;
//     auto nodeIter=std::find_if(_nodes.begin(),_nodes.end(),[=](const node_t* n){return n==node;})
//     return nodeIter != _nodes.end();
// }

} // ::gtpo

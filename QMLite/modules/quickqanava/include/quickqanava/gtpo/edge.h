#pragma once

// ============================================================================
// edge.h — gtpo::edge<> 有向边
// ============================================================================
//
// 继承链：
//   edge_base_t（如 QObject 或空 struct）
//     └── edge<edge_base_t, graph_t, node_t>
//           └── graph_property_impl<graph_t>  — "我属于哪张图"
//
// 职责：
//   1) 存储 src → dst 两个节点指针（非拥有）
//   2) 通过 graph_property_impl 记录所属图
//   3) 析构时检查是否已从图移除（防御性编程）
//
// 三五原则：
//   拷贝构造 = delete（边身份 = 指针地址，不可复制）
//   拷贝赋值 → 隐式 delete（基类 QObject 的拷贝赋值 = delete）
//   移动构造 → 隐式不生成（声明了拷贝构造，C++11 规则抑制移动）
//   移动赋值 → 同上
//   析构     → 虚析构，内部发出"未从图移除"警告
//
// 结果：四个拷贝/移动操作全部不可用，靠一个 = delete + 编译器规则
//       的组合拳达到，不是五个全手写。
// ============================================================================

#include <iostream>      // std::cerr
#include <cassert>
#include <iterator>

#include <quickqanava/gtpo/graph_property.h>

namespace gtpo { // ::gtpo

template <class edge_base_t, class graph_t, class node_t>
class edge : public edge_base_t,
             public graph_property_impl<graph_t>
{
public:
    // ── 类型别名 ──
    using edge_t = edge<edge_base_t, graph_t, node_t>;

    // ── 构造/析构 ──

    //! 默认构造（仅传 parent，src/dst 后续设置）
    explicit edge(edge_base_t* parent = nullptr) noexcept
        : edge_base_t{parent}
    {}

    //! 带 src/dst 的构造
    //  explicit：禁止 {src, dst} 隐式转换成 edge（创建边是有语义后果的操作）
    explicit edge(const node_t* src, const node_t* dst)
        : edge_base_t{}
        , _src{const_cast<node_t*>(src)}
        , _dst{const_cast<node_t*>(dst)}
    {}

    //! 虚析构：如果边未经 graph::remove_edge() 就被 delete，发出警告
    virtual ~edge()
    {
        if (graph_property_impl<graph_t>::_graph != nullptr) {
            // ⚠️ 边还注册在图中就被 delete 了 —— 图中 _edges 容器将出现悬空指针
            std::cerr << "gtpo::edge<>::~edge(): Warning: an edge has been "
                         "deleted before being removed from the graph."
                      << std::endl;
        }
    }

    // ── 拷贝/移动（全不可用）──
    edge(const edge_t&) = delete;           // 显式禁止拷贝构造
    edge& operator=(const edge_t&) = delete; // 显式禁止拷贝赋值
    // 移动构造/赋值未声明 → 因已声明拷贝构造，编译器不自动生成

    // ── 序列化标记 ──
public:
    inline auto get_serializable() const -> bool { return _serializable; }
    inline auto is_serializable() const -> bool { return get_serializable(); }
    inline auto set_serializable(bool serializable) -> void { _serializable = serializable; }

private:
    bool _serializable = true;   // 是否参与序列化（默认 true）

    // ── 源/目标节点访问 ──
public:
    //! 获取源节点（const 版本）
    inline auto get_src() const noexcept -> const node_t* { return _src; }
    //! 获取源节点（非 const 版本）
    inline auto get_src()       noexcept ->       node_t* { return _src; }

    //! 获取目标节点（const 版本）
    inline auto get_dst() const noexcept -> const node_t* { return _dst; }
    //! 获取目标节点（非 const 版本）
    inline auto get_dst()       noexcept ->       node_t* { return _dst; }

    //! 设置源节点（仅 graph 在建立拓扑时调用）
    inline auto set_src(node_t* src) noexcept -> void { _src = src; }
    //! 设置目标节点（仅 graph 在建立拓扑时调用）
    inline auto set_dst(node_t* dst) noexcept -> void { _dst = dst; }

private:
    node_t* _src = nullptr;    // 源节点（非拥有）
    node_t* _dst = nullptr;    // 目标节点（非拥有）
};

} // ::gtpo

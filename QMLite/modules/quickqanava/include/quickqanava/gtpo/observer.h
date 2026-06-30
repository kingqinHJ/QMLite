#pragma once

// ============================================================================
// observer.h — 观察者模式基类（Rule of Zero + 多态防切片）
// ============================================================================
//
// 继承体系：
//   observer<target_t>                          ← Rule of Zero（非拥有裸指针）
//     ├── node_observer<node_t, edge_t>          ← 多态基类，禁止拷贝/移动
//     └── graph_observer<graph_t, node_t, edge_t, group_t>
//
// 三五原则决策：
//   observer<>            → Rule of Zero：全 = default（_target 是裸指针，不拥有）
//   node_observer<>       → 多态防切片：拷贝/移动全 = delete
//   graph_observer<>      → 同上
//
// 为什么拷贝 = delete 是这里最重要的设计决策？
// 见文件末尾注释，或在阅读笔记里找"多态基类防切片"。
// ============================================================================

#include <cstddef>       // std::size_t
#include <functional>    // std::function
#include <vector>
#include <memory>
#include <string>        // std::string
#include <utility>       // std::move

// 前向声明 —— observable.h 里会用到 observer / node_observer / graph_observer
#include "./observable.h"

namespace gtpo { // ::gtpo

// ============================================================================
// 1. observer<> — 观察者基类（Rule of Zero）
// ============================================================================
//
// 成员：
//   _target   : target_t* 裸指针，不拥有目标对象，浅拷贝即正确
//   _name     : std::string 值对象，std::string 自己管资源
//   _enabled  : bool 平凡类型
//
// 三个成员全都不需要手动管理资源 → 五函数全 = default。
//
// 这是"非拥有型"三五原则的标准模板。
// ============================================================================

template <class target_t>
class observer
{
public:
    // ── 构造/析构 ──
    observer() noexcept {}
    virtual ~observer() noexcept {}

    // ── 拷贝/移动：Rule of Zero，全 = default ──
    observer(const observer&) = default;
    observer& operator=(const observer&) = default;
    observer(observer&&) noexcept = default;
    observer& operator=(observer&&) noexcept = default;

    // ── 目标对象（非拥有）──
public:
    auto get_target() const noexcept -> target_t* { return _target; }
    auto set_target(target_t* target) noexcept -> void { _target = target; }
protected:
    target_t* _target = nullptr;   // 非拥有裸指针

    // ── 名称 ──
public:
    auto get_name() const noexcept -> const std::string& { return _name; }
protected:
    auto set_name(const std::string& name) noexcept -> void { _name = name; }
private:
    std::string _name = "";

    // ── 启用/禁用开关 ──
public:
    auto enable() noexcept -> void { _enabled = true; }
    auto disable() noexcept -> void { _enabled = false; }
    auto is_enabled() const noexcept -> bool { return _enabled; }
protected:
    bool _enabled = true;
};

// ============================================================================
// 2. node_observer<> — 节点观察者（多态基类，禁止拷贝/移动）
// ============================================================================
//
// ⚠️ 拷贝/移动全 = delete 的原因：防止切片！
//
//   class MyObserver : public node_observer<Node, Edge> {
//       std::map<int, Data> _state;   // 派生类独有状态
//   };
//   node_observer<Node, Edge> copy = myObs;  // 切片！_state 被切掉，虚表混乱
//
// 结论：任何有虚函数、设计为继承使用的类，都必须在基类层禁止拷贝/移动。
// 这是"多态防切片"模式，三五原则的其中一种标准应用。
//
// 6 个虚函数含义：
//   on_in_node_inserted( target, source, edge ) — 某节点连入 target 时（target 是观察者所在节点）
//   on_in_node_removed( target, source, edge )  — 某节点断开 target 前
//   on_in_node_removed( target )                — target 上所有入边清空后
//   on_out_node_inserted / on_out_node_removed   — 出边版本（target → destination）
//   重载版本 on_out_node_removed( target )       — 所有出边清空后
// ============================================================================

template <class node_t, class edge_t>
class node_observer : public observer<node_t>
{
public:
    // observable_node 需要遍历 _observers 调用 notify —— 声明为友元
    template <class, class>
    friend class gtpo::observable_node;

    // ── 构造/析构 ──
    node_observer() noexcept : observer<node_t>() {}
    virtual ~node_observer() = default;

    // ── 禁止拷贝/移动（防切片）──
    node_observer(const node_observer&) = delete;
    node_observer& operator=(const node_observer&) = delete;
    node_observer(node_observer&&) noexcept = delete;
    node_observer& operator=(node_observer&&) noexcept = delete;

protected:
    // ── 入边通知 ──
    //! 当有边从 source → target（本节点）插入后调用
    virtual auto on_in_node_inserted(node_t& target, node_t& source, const edge_t& edge) noexcept -> void
    {
        static_cast<void>(target);
        static_cast<void>(source);
        static_cast<void>(edge);
    }

    //! 当有边从 source → target 移除前调用
    virtual auto on_in_node_removed(node_t& target, node_t& source, const edge_t& edge) noexcept -> void
    {
        static_cast<void>(target);
        static_cast<void>(source);
        static_cast<void>(edge);
    }

    //! 当 target 上所有入边清空后调用
    virtual auto on_in_node_removed(node_t& target) noexcept -> void
    {
        static_cast<void>(target);
    }

    // ── 出边通知 ──
    //! 当有边从 target（本节点）→ destination 插入后调用
    virtual auto on_out_node_inserted(node_t& target, node_t& destination, const edge_t& edge) noexcept -> void
    {
        static_cast<void>(target);
        static_cast<void>(destination);
        static_cast<void>(edge);
    }

    //! 当有边从 target → destination 移除前调用
    virtual auto on_out_node_removed(node_t& target, node_t& destination, const edge_t& edge) noexcept -> void
    {
        static_cast<void>(target);
        static_cast<void>(destination);
        static_cast<void>(edge);
    }

    //! 当 target 上所有出边清空后调用
    virtual auto on_out_node_removed(node_t& target) noexcept -> void
    {
        static_cast<void>(target);
    }
};

// ============================================================================
// 3. graph_observer<> — 图观察者（多态基类，禁止拷贝/移动）
// ============================================================================
//
// 与 node_observer 同理，拷贝/移动全 delete 是为了防止多态切片。
//
// 6 个虚函数含义：
//   on_node_inserted / on_node_removed       — 节点增删
//   on_edge_inserted / on_edge_removed       — 边增删
//   on_group_inserted / on_group_removed     — 组增删
// ============================================================================

template <class graph_t, class node_t, class edge_t, class group_t>
class graph_observer : public observer<graph_t>
{
public:
    template <class, class, class, class>
    friend class gtpo::observable_graph;

    using this_t = graph_observer<graph_t, node_t, edge_t, group_t>;

    // ── 构造/析构 ──
    graph_observer() noexcept : observer<graph_t>() {}
    virtual ~graph_observer() = default;

    // ── 禁止拷贝（防切片）──
    graph_observer(const this_t&) = delete;
    graph_observer& operator=(const this_t&) = delete;

protected:
    // ── 图级通知 ──
    virtual auto on_node_inserted(node_t& node) noexcept -> void { static_cast<void>(node); }
    virtual auto on_node_removed(node_t& node) noexcept   -> void { static_cast<void>(node); }

    virtual auto on_edge_inserted(edge_t& edge) noexcept -> void { static_cast<void>(edge); }
    virtual auto on_edge_removed(edge_t& edge) noexcept  -> void { static_cast<void>(edge); }

    virtual auto on_group_inserted(group_t& group) noexcept -> void { static_cast<void>(group); }
    virtual auto on_group_removed(group_t& group) noexcept -> void { static_cast<void>(group); }
};

} // ::gtpo

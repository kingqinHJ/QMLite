#pragma once

// ============================================================================
// observable.h — "可被观察"混入（Mixin）
// ============================================================================
//
// 继承体系：
//   abstract_observable                       ← 空接口，所有 observable<T> 的共同非模板基类
//     └── observable<observer_t>               ← 持有 vector<unique_ptr<observer_t>>
//           ├── observable_node<node_t, edge_t> ← 6 个节点级 notify 方法
//           └── observable_graph<graph_t, ...>  ← 6 个图级 notify 方法
//
// 三五原则决策：
//   abstract_observable → Rule of Zero（空壳，全 = default + virtual 析构）
//   observable<>        → 持有 vector<unique_ptr>，拷贝 = default（允许浅拷贝），
//                          析构清理观察者列表
//   observable_node<>   → 多态混入：拷贝 = delete（防止切片）
//   observable_graph<>  → 同上
//
// 设计意图：
//   "被观察者"不关心具体观察者是谁。它只管两件事：
//     1) 存储观察者（unique_ptr 独占所有权）
//     2) 在拓扑变化时遍历通知（调用 observer->on_xxx(...)
//
//   observable_node 和 observable_graph 作为混入被 node<> 和 graph<> 继承，
//   继承后 node/graph 天然拥有"添加观察者 + 通知"的能力。
// ============================================================================

#include <iostream>
#include <vector>
#include <memory>
#include <cstddef>
#include <functional>
#include <string>
#include <utility>

namespace gtpo { // ::gtpo

// ============================================================================
// 1. abstract_observable — 非模板基类接口
// ============================================================================
//
// 为什么需要这个空接口？
//   每个不同模板参数的 observable<T>（如 observable<node_observer>
//   和 observable<graph_observer>）都是不同的类型。
//   有了 abstract_observable，可以用 abstract_observable* 统一引用
//   任何 observable，或者用 dynamic_cast 判断某对象是否实现了
//   observable 概念。
//
// 三五原则：Rule of Zero。空类，编译器默认生成一切正确。
// ============================================================================

class abstract_observable
{
public:
    abstract_observable() = default;
    virtual ~abstract_observable() = default;

    abstract_observable(const abstract_observable&) = default;
    abstract_observable& operator=(const abstract_observable&) = default;
    abstract_observable(abstract_observable&&) = default;
    abstract_observable& operator=(abstract_observable&&) = default;
};

// ============================================================================
// 2. observable<> — 观察者容器
// ============================================================================
//
// 持有 vector<unique_ptr<observer_t>>，提供 add_observer / clear。
//
// 关键设计：add_observer 是 sink 函数 —— 调用方交出所有权：
//     node.add_observer(std::make_unique<MyObserver>());
//          ↑ unique_ptr 传入，所有权转移给 observable::_observers
//
// 拷贝行为：= default。为什么允许？
//   因为 observer_t 是多态基类（拷贝 = delete），unique_ptr 不可拷贝，
//   所以 observable 虽然声明拷贝 = default，但实际拷贝时会因为
//   vector<unique_ptr<...>> 不可拷贝而编译失败。
//   这正是预期行为 —— observable 的拷贝只在编译器自动处理时被
//   隐式抑制，不需要显式 = delete。
// ============================================================================

template <class observer_t>
class observable : public abstract_observable
{
public:
    // ── 构造/析构 ──
    observable() : abstract_observable() {}
    virtual ~observable() noexcept
    {
        _observers.clear();   // unique_ptr 自动 delete 各观察者
    }

    // ── 拷贝 = default（实际因 unique_ptr 不可拷贝而隐式禁止）──
    observable(const observable&) = default;
    observable& operator=(const observable&) = default;

    // ── 观察者容器操作 ──
public:
    //! 清空所有观察者（unique_ptr 自动释放）
    inline auto clear() -> void { _observers.clear(); }

    //! 添加观察者（sink：调用方交出所有权）
    inline auto add_observer(std::unique_ptr<observer_t> observer) -> void
    {
        _observers.emplace_back(std::move(observer));
    }

    //! 容器类型别名
    using observers_t = std::vector<std::unique_ptr<observer_t>>;

    //! 是否有已注册的观察者
    inline auto has_observers() const noexcept -> bool { return !_observers.empty(); }

    //! 获取观察者列表（只读）
    inline auto get_observers() const noexcept -> const observers_t& { return _observers; }

protected:
    //! 观察者列表（独占所有权：observable 销毁时自动清理所有观察者）
    observers_t _observers;
};

// ============================================================================
// 前向声明（observer.h 里有完整定义）
// ============================================================================
template <class target_t>
class observer;

template <class node_t, class edge_t>
class node_observer;

template <class graph_t, class node_t, class edge_t, class group_t>
class graph_observer;

// ============================================================================
// 3. observable_node<> — 节点级可观察混入
// ============================================================================
//
// 继承自 observable<node_observer<...>>，因此 _observers 存储的是
// vector<unique_ptr<node_observer<node_t, edge_t>>>。
//
// 提供 6 个 notify 方法，内部遍历 _observers 并调用 observer->on_xxx(...)。
//
// 拷贝 = delete：多态混入，防止切片。
// ============================================================================

template <class node_t, class edge_t>
class observable_node : public observable<gtpo::node_observer<node_t, edge_t>>
{
public:
    using node_observer_t = gtpo::node_observer<node_t, edge_t>;
    using super_t = observable<gtpo::node_observer<node_t, edge_t>>;

    // ── 构造/析构 ──
    observable_node() noexcept : super_t() {}
    ~observable_node() noexcept = default;

    // ── 禁止拷贝（多态混入，防切片）──
    observable_node(const observable_node&) = delete;
    observable_node& operator=(const observable_node&) = delete;

    // ── 添加观察者（便利方法，内部调用 super_t::add_observer）──
public:
    auto add_node_observer(std::unique_ptr<node_observer_t> observer) -> void
    {
        super_t::add_observer(std::move(observer));
    }

    // ── 6 个通知方法 ──
    // 每个方法遍历 _observers，逐一调用对应虚函数 on_xxx(...)。
    // 如果观察者被 disable()，它仍然在列表中，但 on_xxx 内的实现
    // 应自行检查 is_enabled() —— 这是观察者的责任。
public:
    auto notify_in_node_inserted(node_t& target, node_t& node, const edge_t& edge) noexcept -> void
    {
        for (auto& observer : super_t::_observers)
            if (observer)
                observer->on_in_node_inserted(target, node, edge);
    }

    auto notify_in_node_removed(node_t& target, node_t& node, const edge_t& edge) noexcept -> void
    {
        for (auto& observer : super_t::_observers)
            if (observer)
                observer->on_in_node_removed(target, node, edge);
    }

    auto notify_in_node_removed(node_t& target) noexcept -> void
    {
        for (auto& observer : super_t::_observers)
            if (observer)
                observer->on_in_node_removed(target);
    }

    auto notify_out_node_inserted(node_t& target, node_t& node, const edge_t& edge) noexcept -> void
    {
        for (auto& observer : super_t::_observers)
            if (observer)
                observer->on_out_node_inserted(target, node, edge);
    }

    auto notify_out_node_removed(node_t& target, node_t& node, const edge_t& edge) noexcept -> void
    {
        for (auto& observer : super_t::_observers)
            if (observer)
                observer->on_out_node_removed(target, node, edge);
    }

    auto notify_out_node_removed(node_t& target) noexcept -> void
    {
        for (auto& observer : super_t::_observers)
            if (observer)
                observer->on_out_node_removed(target);
    }
};

// ============================================================================
// 4. observable_graph<> — 图级可观察混入
// ============================================================================
//
// 与 observable_node 同理，提供 6 个图级 notify 方法。
// 拷贝 = delete：多态混入，防切片。
// ============================================================================

template <class graph_t, class node_t, class edge_t, class group_t>
class observable_graph : public observable<gtpo::graph_observer<graph_t, node_t, edge_t, group_t>>
{
public:
    using graph_observer_t = gtpo::graph_observer<graph_t, node_t, edge_t, group_t>;
    using super_t = observable<gtpo::graph_observer<graph_t, node_t, edge_t, group_t>>;

    // ── 构造/析构 ──
    observable_graph() noexcept : super_t() {}
    virtual ~observable_graph() noexcept = default;

    // ── 禁止拷贝（多态混入，防切片）──
    observable_graph(const observable_graph&) = delete;
    observable_graph& operator=(const observable_graph&) = delete;

    // ── 添加观察者 ──
public:
    inline auto add_graph_observer(std::unique_ptr<graph_observer_t> observer) -> void
    {
        super_t::add_observer(std::move(observer));
    }

    // ── 6 个通知方法 ──
public:
    auto notify_node_inserted(node_t& node) noexcept -> void
    {
        for (auto& observer : super_t::_observers)
            if (observer)
                observer->on_node_inserted(node);
    }

    auto notify_node_removed(node_t& node) noexcept -> void
    {
        for (auto& observer : super_t::_observers)
            if (observer)
                observer->on_node_removed(node);
    }

    auto notify_edge_inserted(edge_t& edge) noexcept -> void
    {
        for (auto& observer : super_t::_observers)
            if (observer)
                observer->on_edge_inserted(edge);
    }

    auto notify_edge_removed(edge_t& edge) noexcept -> void
    {
        for (auto& observer : super_t::_observers)
            if (observer)
                observer->on_edge_removed(edge);
    }

    auto notify_group_inserted(group_t& group) noexcept -> void
    {
        for (auto& observer : super_t::_observers)
            if (observer)
                observer->on_group_inserted(group);
    }

    auto notify_group_removed(group_t& group) noexcept -> void
    {
        for (auto& observer : super_t::_observers)
            if (observer)
                observer->on_group_removed(group);
    }
};

} // ::gtpo

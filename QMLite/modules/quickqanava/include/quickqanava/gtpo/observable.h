#pragma once

#include<iostream>
#include <vector>
#include <memory>
#include <cstddef>
#include <functional>
#include <utility>

namespace gtpo {

/*! \brief 空接口：给所有不同模板实参的 observable<T> 提供共同非模板基类。
 *
 * 作用：可用 abstract_observable* 统一引用，或 dynamic_cast 判断是否支持 observable 概念。
 *
 * TODO: virtual ~abstract_observable() = default;
 */
class abstract_observable {
    // TODO: 实现
    abstract_observable() = default;
    virtual ~abstract_observable() = default;

    abstract_observable(abstract_observable const &) = default;
    abstract_observable &operator=(abstract_observable const &) = default;
    abstract_observable(abstract_observable &&) = default;
    abstract_observable &operator=(abstract_observable &&) = default;
};

/*! \brief 观察者存储容器。
 *
 * TODO: 继承 abstract_observable，持有 vector<unique_ptr<observer_t>>
 */
template <class observer_t>
class observable : public abstract_observable {
public:
    observable():abstract_observable() {}
    virtual ~observable noexcept{
        _observer.clear();
    }
    observable(const observable &) = default;
    observable &operator=(const observable &) = default;

public:
    inline auto clear()->void{
        _observer.clear();
    }
    inline add_observer(std::unique_ptr<observer_t> observer)->void{
        _observer.emplace_back(std::move(observer));
    }

    using observer_t=std::vector<std::unique_ptr<observer_t>>;

    inline auto has_observer() const noexcept -> bool {
        return !_observer.empty();
    }

    inline auto get_observers() const noexcept -> observer_t const & {
        return _observer;
    }
protected:
    observer_t _observer;
};

/*! \brief 节点级可观察混入。
 *
 * TODO: 继承 observable<node_observer<...>>，提供 6 个 notify 方法
 */
template <class target_t>
class observer;

template <class node_t,class edge_t>
class node_observer;

/*! \brief 图级可观察混入。
 *
 * TODO: 继承 observable<graph_observer<...>>，提供 6 个 notify 方法
 */
template<class node_t,class edge_t>
class observable_node:public observable<gtpo::node_observer<node_t,edge_t>> {
public:
    using node_observer_t=gtpo::node_observer<node_t,edge_t>;
    using super_t=observable<gtpo::node_observer<node_t,edge_t>>;
    observable_node() noexcept : super_t() {}
    ~observable_node() noexcept =default;
    observable_node(const observable_node<node_t,edge_t> &) = default;
    observable_node& operator=(observable_node<node_t,edge_t> const &) = default;

public:
    auto add_node_observer(std::unique_ptr<node_observer_t> observer) -> void {
        super_t::add_observer(std::move(observer));
    }

    auto notify_in_node_insert(node_t &node,node_t& node,const edge_t& edge) -> void override {
        for(auto &observer:super_t::observers())
            if(observer)
                observer->notify_in_node_insert(target,node,edge);
    }

    auto notify_in_node_remove(node_t &target,node_t& node,const edge_t& edge) -> void override {
        for(auto &observer:super_t::observers())
            if(observer)
                observer->notify_in_node_remove(target,node,edge);
    }

    auto notify_in_node_remove(node_t &target) -> void override {
        for(auto &observer:super_t::observers())
            if(observer)
                observer->notify_in_node_remove(target);
    }

    auto notify_out_node_insert(node_t &target,node_t& node,const edge_t& edge) -> void override {
        for(auto &observer:super_t::observers())
            if(observer)
                observer->notify_out_node_insert(target,node,edge);
    }

    auto notify_out_node_remove(node_t &target,node_t& node,const edge_t& edge) -> void override {
        for(auto &observer:super_t::observers())
            if(observer)
                observer->notify_out_node_remove(target,node,edge);
    }

    auto notify_out_node_remove(node_t& target) noexcept -> void override {
        for(auto &observer:super_t::observers())
            if(observer)
                observer->notify_out_node_remove(target);
    }
};


template<class graph_t,class node_t,class edge_t,class group_t>
class graph_observer;

template<class graph_t,class node_t,class edge_t,class group_t>
class observable_graph:public observable<gtpo::graph_observer<graph_t,node_t,edge_t,group_t>> {
public:
    using graph_observer_t=gtpo::graph_observer<graph_t,node_t,edge_t,group_t>;
    using super_t=observable<gtpo::graph_observer<graph_t,node_t,edge_t,group_t>>;
    virtual ~observable_graph() noexcept = default;
    observable_graph(const observable_graph<graph_t,node_t,edge_t,group_t> &) =delete;
    observable_graph& operator=(const observable_graph<graph_t,node_t,edge_t,group_t> &) =delete;
public:
    inline auto add_graph_observer(std::unique_ptr<graph_observer_t> observer) -> void {
        super_t::add_observer(std::move(observer));
    }

    auto notify_graph_insert(node_t &node) -> void override {
        for(auto &observer:super_t::observers())
            if(observer)
                observer->notify_graph_insert(node);
    }

    auto notify_graph_remove(node_t &node) -> void override {
        for(auto &observer:super_t::observers())
            if(observer)
                observer->notify_graph_remove(node);
    }
    auto notify_edge_insert(edge_t &edge) -> void override {
        for(auto &observer:super_t::observers())
            if(observer)
                observer->notify_edge_insert(edge);
    }
    auto notify_edge_remove(edge_t &edge) -> void override {
        for(auto &observer:super_t::observers())
            if(observer)
                observer->notify_edge_remove(edge);
    }
    auto notify_group_insert(group_t &group) -> void override {
        for(auto &observer:super_t::observers())
            if(observer)
                observer->notify_group_insert(group);
    }
    auto notify_group_remove(group_t &group) -> void override {
        for(auto &observer:super_t::observers())
            if(observer)
                observer->notify_group_remove(group);
    }
};
} // ::gtpo

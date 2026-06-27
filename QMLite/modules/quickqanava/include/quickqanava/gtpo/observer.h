on_in_node_insert#pragma once

#include <cstddef>
#include <functional>
#include <vector>
#include <memory>
#include <utility>
#include "observable.h"

namespace gtpo {

/*! \brief 观察者基类。
 *
 * TODO: _target 指针 + enable/disable 开关
 */
template <class target_t>
class observer {
    public:
    observer() noexcept{}
    ~observer() noexcept{}

    observer(const observer &other) =default;
    observer(observer &&other) noexcept =default;
    observer &operator=(const observer &other) =default;
    observer &operator=(observer &&other) noexcept =default;

public:
    target_t* get_target() const noexcept { return _target; }
    void set_target(target_t* target) noexcept { _target = target; }

    inline auto getName() const noexcept -> std::string const & { return _name; }

    inline auto enable() noexcept -> void { _enabled = true; }
    inline auto disable() noexcept -> void { _enabled = false; }
    inline auto is_enabled() const noexcept -> bool { return _enabled; }
protected:
    void setName(std::string const &name) noexcept { _name = name; }
protected:
    target_t* _target = nullptr;
    bool _enabled = true;
private:
    std::string _name="";
};

/*! \brief 节点观察者接口。
 *
 * TODO: 6 个虚函数（出入边增删清空），默认空实现
 */
template <class node_t, class edge_t>
class node_observer :public observer<node_t> {
public:
    template<class,class>
    friend class gtpo::observable_node;

    node_observer() noexcept: observer<node_t>() {}
    virtual ~node_observer() =default;
    node_observer(const node_observer<node_t, edge_t> &other) =delete;
    node_observer(node_observer<node_t, edge_t> &&other) noexcept =delete;
    node_observer &operator=(const node_observer<node_t, edge_t> &other) =delete;
    node_observer &operator=(node_observer<node_t, edge_t> &&other) noexcept =delete;
protected:
    virtual void on_in_node_inserted(node_t &target,node_t &source, edge_t &edge) noexcept {
        static_cast<void>(target);static_cast<void>(source);static_cast<void>(edge);
    }
    virtual void on_in_node_removed(node_t &target,node_t &source, edge_t &edge) noexcept {
        static_cast<void>(target);static_cast<void>(source);static_cast<void>(edge);
    }
    virtual void on_in_node_removed(node_t &target) noexcept {
        static_cast<void>(target);
    }
    virtual void on_out_node_inserted(node_t &target,node_t &destination, edge_t &edge) noexcept {
        static_cast<void>(target);static_cast<void>(destination);static_cast<void>(edge);
    }
    virtual void on_out_node_removed(node_t &target,node_t &destination, edge_t &edge) noexcept {
        static_cast<void>(target);static_cast<void>(destination);static_cast<void>(edge);
    }
    virtual void on_out_node_removed(node_t &target) noexcept {
        static_cast<void>(target);
    }
};




/*! \brief 图观察者接口。
 *
 * TODO: 6 个虚函数（节点/边/组增删），默认空实现
 */
template <class graph_t, class node_t, class edge_t, class group_t>
class graph_observer:public observer<graph_t> {
public:
    template<class,class,class,class>
    friend class gtpo::observable_graph;

    using this_t = graph_observer<graph_t, node_t, edge_t, group_t>;
    graph_observer() noexcept: observer<graph_t>() {}
    virtual ~graph_observer() =default;
    graph_observer(const this_t &other) =delete;
    graph_observer &operator=(const this_t &other) =delete;
protected:
    virtual void on_node_inserted(node_t &node) noexcept {
        static_cast<void>(node);
    }
    virtual void on_node_removed(node_t &node) noexcept {
        static_cast<void>(node);
    }
    virtual void on_edge_inserted(edge_t &edge) noexcept {
        static_cast<void>(edge);
    }
    virtual void on_edge_removed(edge_t &edge) noexcept {
        static_cast<void>(edge);
    }
    virtual void on_group_inserted(node_t &group) noexcept {
        static_cast<void>(group);
    }
    virtual void on_group_removed(node_t &group) noexcept {
        static_cast<void>(group);
    }
};

} // ::gtpo

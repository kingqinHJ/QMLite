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
protected:
    observer_t _observer;
};

/*! \brief 节点级可观察混入。
 *
 * TODO: 继承 observable<node_observer<...>>，提供 6 个 notify 方法
 */
// template <class node_t, class edge_t>
// class observable_node { ... };

/*! \brief 图级可观察混入。
 *
 * TODO: 继承 observable<graph_observer<...>>，提供 6 个 notify 方法
 */
// template <class graph_t, class node_t, class edge_t, class group_t>
// class observable_graph { ... };

} // ::gtpo

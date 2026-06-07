#pragma once

#include "observable.h"

namespace gtpo {

/*! \brief 观察者基类。
 *
 * TODO: _target 指针 + enable/disable 开关
 */
template <class target_t>
class observer {
    // TODO: 实现
};

/*! \brief 节点观察者接口。
 *
 * TODO: 6 个虚函数（出入边增删清空），默认空实现
 */
// template <class node_t, class edge_t>
// class node_observer { ... };

/*! \brief 图观察者接口。
 *
 * TODO: 6 个虚函数（节点/边/组增删），默认空实现
 */
// template <class graph_t, class node_t, class edge_t, class group_t>
// class graph_observer { ... };

} // ::gtpo

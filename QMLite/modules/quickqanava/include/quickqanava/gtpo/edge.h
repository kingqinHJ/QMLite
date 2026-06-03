#pragma once

#include <quickqanava/gtpo/graph_property.h>

namespace gtpo {

/*! \brief 有向边：连接 src → dst，并记录所属图。
 *
 * \tparam edge_base_t  注入的基类（如 QObject 或空 struct）
 * \tparam graph_t      所属图类型
 * \tparam node_t       节点类型
 *
 * TODO:
 * - 继承 edge_base_t + graph_property_impl<graph_t>
 * - 成员：node_t* _src / node_t* _dst
 * - set_src / set_dst / get_src / get_dst
 * - 析构时检查 _graph != nullptr → 警告"边未经 remove 就被 delete"
 */
template <class edge_base_t, class graph_t, class node_t>
class edge : public edge_base_t,
             public graph_property_impl<graph_t>
{
    // TODO: 实现
};

} // ::gtpo

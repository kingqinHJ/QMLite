#pragma once

#include <quickqanava/gtpo/graph_property.h>
#include <unordered_set>
#include <assert>
#include <iterator>

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
public:
    using edge_t=edge<edge_base_t,graph_t,node_t>;     
    edge(edge_base_t* parent=nullptr) noexcept:edge_base_t{parent}{}
    explicit edge(const node_t* src,const node_t* dst):edge_base_t{other},
        _src{src},_dst{dst}
    virtual ~edge(){
        if(graph_property_impl<graph_t>!=nullptr)
            std::cerr<< << "gtpo::edge<>::~edge(): Warning: an edge has been deleted before beeing " <<
                         "removed from the graph." << std::endl;
    }
    edge(const edge_t& other)=delete;

public:
    inline auto get_serializable() const->bool { return _serializable; }
    inline auto is_serializable() const->bool { return get_serializable(); }
    inline auto set_serializable(bool serializable)->void { _serializable=serializable; }
private:
    bool  _serializable{true};

public:
    inline auto get_src() const noexcept->node_t* { return _src; }   
    inline auto get_src() noexcept->node_t* { return _src; }    
    inline auto get_dst() const noexcept->node_t* { return _dst; }
    inline auto get_dst() noexcept->node_t* { return _dst; }
    inline auto set_src(node_t* src)noexcept->void { _src=src; }
    inline auto set_dst(node_t* dst)noexcept->void { _dst=dst; }

private：
    node_t *_src{nullptr};
    node_t *_dst{nullptr};
};

} // ::gtpo

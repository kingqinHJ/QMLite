#pragma once

#include <cstddef>  // std::nullptr_t

namespace gtpo {

/*! \brief "我属于哪张图" 的类型安全回指混入。
 *
 * 被 node / edge / group 继承，每个拓扑元素都能拿到所属图的指针。
 * 使用裸指针：图一定比内部元素活得长，由 graph 统一管理生命周期。
 *
 * 【C++ 知识点：为什么显式声明拷贝/移动为 = default？】
 *
 * 1. "三五法则"（Rule of Three / Five）
 *    一旦类显式声明了析构函数（即使是 virtual ~X() = default），编译器
 *    就不会再自动生成【移动构造函数】和【移动赋值运算符】！
 *    参考：《C++ Primer 中文版（第5版）》13.6.2 节，P475 附近
 *
 * 2. = default 的含义
 *    显式要求编译器生成默认版本。这里的默认行为就是逐成员拷贝/移动。
 *    由于本类只有一个 graph_t* 裸指针成员，默认生成的行为完全符合需求。
 *    参考：《C++ Primer 中文版（第5版）》13.1.1 节，P389 附近
 *
 * 3. = delete 的含义（扩展）
 *    显式禁止某个操作。常见场景：
 *    - 禁止拷贝：Singleton、文件句柄等不可复制的资源
 *      struct NonCopyable { NonCopyable(const NonCopyable&) = delete; ... };
 *    - 禁止特定转换：
 *      void set_graph(void*) = delete;  // 禁止隐式 void* 转换
 *    参考：《C++ Primer 中文版（第5版）》13.1.6 节，P390 附近
 *
 * 4. 本类的设计决策
 *    - 声明了虚析构（会被继承，防止基类指针 delete 派生类时泄漏）
 *    - 因此必须显式 = default 移动操作，否则 node/edge 将失去移动语义
 *    - 拷贝操作也用 = default：裸指针拷贝是浅拷贝，符合预期（不拥有所有权）
 */
template <class graph_t>
class graph_property_impl {
    friend graph_t;   // 只有 graph 能调用 set_graph()

public:
    graph_property_impl() = default;

    // 虚析构：本类作为基类被继承，必须有虚析构防止内存泄漏
    virtual ~graph_property_impl() = default;

    // 拷贝操作：= default（逐成员拷贝裸指针，符合预期）
    graph_property_impl(const graph_property_impl&) = default;
    graph_property_impl& operator=(const graph_property_impl&) = default;

    // 移动操作：必须显式 = default（因声明了析构函数，编译器不再自动生成）
    graph_property_impl(graph_property_impl&&) noexcept = default;
    graph_property_impl& operator=(graph_property_impl&&) noexcept = default;

    // ── 公共接口（只读） ──

    //! 获取所属图（非 const 上下文）
    inline auto get_graph() noexcept -> graph_t* { return _graph; }

    //! 获取所属图（const 上下文）
    inline auto get_graph() const noexcept -> const graph_t* { return _graph; }

    //! 判断当前元素是否已绑定到某张图
    inline auto has_graph() const noexcept -> bool { return _graph != nullptr; }

protected:
    //! 设置所属图（仅 graph_t 友元可调用）
    inline auto set_graph(graph_t* graph) noexcept -> void { _graph = graph; }

    //! 解绑（置空）
    inline auto set_graph(std::nullptr_t) noexcept -> void { _graph = nullptr; }

private:
    graph_t* _graph = nullptr;
};

} // ::gtpo

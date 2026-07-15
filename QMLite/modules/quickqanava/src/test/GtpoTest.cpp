#include "GtpoTest.h"

#include <quickqanava/gtpo/graph.h>
#include <quickqanava/gtpo/node.h>
#include <quickqanava/gtpo/edge.h>

#include <sstream>
#include <string>

// ============================================================================
// GTpo 设计原则与设计模式总结
// ============================================================================
//
// GTpo（Generic Graph Topology）是一个纯模板有向图拓扑引擎，6 个文件、
// ~600 行代码，蕴含了丰富的面向对象设计思想。以下是逐类拆解：
//
// ────────────────────────────────────────────────────────────────────────────
// 一、SOLID 原则
// ────────────────────────────────────────────────────────────────────────────
//
// 【SRP — 单一职责】
//   graph_property_impl → 只管"我属于哪张图"（一个裸指针）
//   observable<>        → 只管"存储观察者 + 遍历通知"
//   observer<>          → 只管"持有 target 指针 + enable/disable"
//   node<>              → 只管"出入边集合 + 组语义"
//   graph<>             → 只管"容器 + 工厂 + 所有权"
//   container_adapter    → 只管"抹平不同容器的 API 差异"
//
// 【OCP — 开闭原则】
//   graph/node/edge 通过模板参数 graph_base_t / node_base_t / edge_base_t
//   注入基类：测试时注入 EmptyBase，QML 层注入 QQuickItem/QObject。
//   内部代码一行不改，纯靠模板参数实现"对扩展开放，对修改关闭"。
//
// 【LSP — 里氏替换】
//   Group 是 Node 的子类，任何接受 Node* 的地方都可以传 Group*。
//   graph::insert_node(group) 就是把组当成普通节点用——完全兼容。
//
// 【ISP — 接口隔离】
//   abstract_observable 只提供一个虚析构，给 dynamic_cast 判断用。
//   observable<> 单独提供观察者存储能力，node 想用就混入，不想用就不继承。
//   observer 体系完全独立于 topology 体系，互不绑架。
//
// 【DIP — 依赖倒置】
//   graph  不依赖具体的 Node/Edge → 依赖模板参数 node_t / edge_t
//   node   不依赖具体的 Graph     → 依赖 graph_property_impl<graph_t>
//   edge   同上
//   高层和低层都依赖抽象（模板参数），不依赖具体实现。
//
// ────────────────────────────────────────────────────────────────────────────
// 二、设计模式
// ────────────────────────────────────────────────────────────────────────────
//
// 【Observer — 观察者模式】
//   observable<>          → Subject（被观察者）
//   observable_node<>     → 节点级 Subject（6 个 notify 方法）
//   observable_graph<>    → 图级 Subject（6 个 notify 方法）
//   observer<>            → Observer 基类
//   node_observer<>       → 节点级观察者（6 个虚钩子）
//   graph_observer<>      → 图级观察者（6 个虚钩子）
//   双层观察者互不干扰，各自独立通知。
//
// 【Template Method — 模板方法模式】
//   node_observer 定义 6 个虚函数 + 默认空实现。
//   派生类只需覆盖关心的那个（如 on_in_node_inserted）。
//   observable_node 的 notify_* 遍历调用 observer->on_*，
//   算法框架在基类，具体行为在派生类。
//
// 【Mixin — 混入模式】
//   class node : public node_base_t,                     // 注入基类
//                public graph_property_impl<...>,         // 混入：回指能力
//                public observable_node<...>              // 混入：可被观察
//   功能通过多重继承横向组合，而不是通过深层继承竖向堆叠。
//
// 【Strategy — 策略模式（编译期）】
//   container_adapter 四个全特化：std::vector / QVector / QSet / unordered_set。
//   graph/node 调用 container_adapter<C>::insert(item, c)，
//   具体策略由模板参数在编译期选定，零运行时开销。
//
// 【Adapter — 适配器模式】
//   container_adapter 把 push_back / append / insert 统一成 insert() 接口。
//   抹平 STL 和 Qt 容器的 API 差异，不修改容器源码。
//
// 【Factory Method — 工厂方法】
//   graph::create_node()  → new node_t()
//   graph::insert_edge()  → new edge_t() + 双向注册
//   graph 是唯一的创建入口，外部永远不直接 new 后手动注册。
//
// 【Dependency Injection — 依赖注入】
//   graph_base_t / node_base_t / edge_base_t 三个模板参数就是注入点。
//   调用方决定基类是什么，graph 内部完全不感知。
//
// ────────────────────────────────────────────────────────────────────────────
// 三、三五原则（Rule of Three/Five）
// ────────────────────────────────────────────────────────────────────────────
//
//   graph_property_impl → Rule of Zero（全 = default，非拥有裸指针）
//   observer<>          → Rule of Zero
//   node_observer<>     → 多态防切片（拷贝/移动全 = delete）
//   graph_observer<>    → 同上
//   edge<>              → 拷贝 = delete，赋值/移动靠编译器规则隐式禁止
//   node<>              → 拷贝构造 = 空操作（身份语义），赋值 = delete
//   graph<>             → 全 = delete（所有权根，不可复制/移动）
//
// ============================================================================

// ============================================================================
// 测试类型定义
// ============================================================================
// 模板参数注入：graph_base_t / node_base_t / edge_base_t 用空 struct
// GTpo 不关心基类是什么，只提供拓扑能力

struct EmptyBase {};

// 前向声明（解决循环依赖：TestGraph ← TestNode ← TestGraph）
class TestNode;
class TestEdge;
class TestGroup;

// ── 图类型 ──
using TestGraph = gtpo::graph<EmptyBase, TestNode, TestGroup, TestEdge>;

// ── 节点类型 ──
class TestNode : public gtpo::node<EmptyBase, TestGraph, TestNode, TestEdge, TestGroup>
{
public:
    using gtpo::node<EmptyBase, TestGraph, TestNode, TestEdge, TestGroup>::node;
};

// ── 边类型 ──
class TestEdge : public gtpo::edge<EmptyBase, TestGraph, TestNode>
{
public:
    using gtpo::edge<EmptyBase, TestGraph, TestNode>::edge;
};

// ── 组类型：就是普通节点 + is_group=true ──
class TestGroup : public TestNode
{
public:
    explicit TestGroup(EmptyBase* parent = nullptr)
        : TestNode(parent)
    {
        this->set_is_group(true);
    }
};


// ============================================================================
// 测试实现
// ============================================================================

GtpoTest::GtpoTest(QObject* parent) : QObject(parent) {}

QString GtpoTest::runPhase1Test() const
{
    std::ostringstream out;
    int passed = 0;
    int total  = 0;

    auto test = [&](const char* name, bool result) {
        ++total;
        if (result) {
            ++passed;
            out << "  [PASS] " << name << "\n";
        } else {
            out << "  [FAIL] " << name << "\n";
        }
    };

    // ────────────────────────────────────────────────────
    // 测试 1: 创建图 → 插入节点 → 计数
    // ────────────────────────────────────────────────────
    {
        TestGraph g;

        auto n1 = new TestNode();
        auto n2 = new TestNode();
        auto n3 = new TestNode();
        g.insert_node(n1);
        g.insert_node(n2);
        g.insert_node(n3);

        test("insert_node — count == 3",       g.get_node_count() == 3);
        test("insert_node — contains(n1)",     g.contains(n1));
        test("insert_node — duplicate rejected", !g.insert_node(n1));  // 重复插入应拒绝
        test("root count (all in-degree=0)",    g.get_root_node_count() == 3);
    }

    // ────────────────────────────────────────────────────
    // 测试 2: 创建边 → 验证度 / has_edge / find_edge
    // ────────────────────────────────────────────────────
    {
        TestGraph g;
        auto n1 = new TestNode();  g.insert_node(n1);
        auto n2 = new TestNode();  g.insert_node(n2);
        auto n3 = new TestNode();  g.insert_node(n3);

        auto e12 = g.insert_edge(n1, n2);
        auto e23 = g.insert_edge(n2, n3);

        test("insert_edge — e12 not null",     e12 != nullptr);
        test("insert_edge — e23 not null",     e23 != nullptr);
        test("edge count == 2",                g.get_edge_count() == 2);
        test("has_edge(n1, n2)",               g.has_edge(n1, n2));
        test("!has_edge(n1, n3)",              !g.has_edge(n1, n3));
        test("find_edge(n2, n3) not null",     g.find_edge(n2, n3) != nullptr);
        test("n1 out_degree == 1",             n1->get_out_degree() == 1);
        test("n2 in_degree == 1",              n2->get_in_degree() == 1);
        test("n2 out_degree == 1",             n2->get_out_degree() == 1);
        test("n3 in_degree == 1",              n3->get_in_degree() == 1);
        test("root count (n1 only)",           g.get_root_node_count() == 1);
    }

    // ────────────────────────────────────────────────────
    // 测试 3: 移除边 → 度归零
    // ────────────────────────────────────────────────────
    {
        TestGraph g;
        auto n1 = new TestNode();  g.insert_node(n1);
        auto n2 = new TestNode();  g.insert_node(n2);

        auto e = g.insert_edge(n1, n2);
        test("before remove — edge count == 1", g.get_edge_count() == 1);
        test("before remove — n1 out_degree == 1", n1->get_out_degree() == 1);

        g.remove_edge(e);
        test("after remove — edge count == 0",  g.get_edge_count() == 0);
        test("after remove — n1 out_degree == 0", n1->get_out_degree() == 0);
        test("after remove — n2 in_degree == 0",  n2->get_in_degree() == 0);
        test("after remove — root count == 2",    g.get_root_node_count() == 2);
    }

    // ────────────────────────────────────────────────────
    // 测试 4: 组操作
    // ────────────────────────────────────────────────────
    {
        TestGraph g;
        auto n1 = new TestNode();  g.insert_node(n1);
        auto n2 = new TestNode();  g.insert_node(n2);
        auto grp = new TestGroup();  g.insert_group(grp);

        test("group inserted — node count includes group",
             g.get_node_count() == 3);
        test("group inserted — group count == 1",
             g.get_group_count() == 1);

        g.group_node(n1, grp);
        g.group_node(n2, grp);

        test("group_node — n1.group == grp",  n1->get_group() == grp);
        test("group_node — n2.group == grp",  n2->get_group() == grp);
        test("group_node — grp has n1",       grp->has_node(n1));
        test("group_node — grp has n2",       grp->has_node(n2));
        test("group count == 2",              grp->get_node_count() == 2);

        g.ungroup_node(n1, grp);
        test("ungroup_node — n1.group == nullptr", n1->get_group() == nullptr);
        test("ungroup_node — grp count == 1",       grp->get_node_count() == 1);
    }

    // ────────────────────────────────────────────────────
    // 测试 5: 移除节点 → 关联边自动删除
    // ────────────────────────────────────────────────────
    {
        TestGraph g;
        auto n1 = new TestNode();  g.insert_node(n1);
        auto n2 = new TestNode();  g.insert_node(n2);
        auto n3 = new TestNode();  g.insert_node(n3);

        g.insert_edge(n1, n2);
        g.insert_edge(n2, n3);

        test("before remove_node — edge count == 2", g.get_edge_count() == 2);
        test("before remove_node — node count == 3", g.get_node_count() == 3);

        g.remove_node(n2);  // n2 关联的 n1→n2 和 n2→n3 都应被删除

        test("after remove_node — node count == 2", g.get_node_count() == 2);
        test("after remove_node — edge count == 0", g.get_edge_count() == 0);
        test("n1 still exists",                     g.contains(n1));
        test("n3 still exists",                     g.contains(n3));
    }

    // ────────────────────────────────────────────────────
    // 汇总
    // ────────────────────────────────────────────────────
    out << "\n  Result: " << passed << "/" << total << " passed";
    if (passed == total)
        out << " ✅ ALL PASSED\n";
    else
        out << " ❌ " << (total - passed) << " FAILED\n";

    return QString::fromStdString(out.str());
}

#include "GtpoTest.h"

#include <quickqanava/gtpo/graph.h>
#include <quickqanava/gtpo/node.h>
#include <quickqanava/gtpo/edge.h>

#include <sstream>
#include <string>

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

#include "GtpoTest.h"

// TODO: 在此文件中实例化 gtpo 模板并运行测试
//
// 步骤：
// 1. 定义空基类 struct EmptyBase {};
// 2. 前向声明 TestNode / TestEdge / TestGroup
// 3. using TestGraph = gtpo::graph<EmptyBase, TestNode, TestGroup, TestEdge>;
// 4. 定义 TestNode / TestEdge / TestGroup（继承 gtpo::node / gtpo::edge）
// 5. 在 runPhase1Test() 中编写测试用例
//
// 参考 docs/gtpo-guide.md 第五步末尾的验证示例

GtpoTest::GtpoTest(QObject* parent) : QObject(parent) {}

QString GtpoTest::runPhase1Test() const
{
    // TODO: 实现测试，返回 "[PASS]/[FAIL]" 结果字符串
    return "TODO: 请实现 gtpo 后在此运行测试";
}

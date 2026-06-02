我把我们讨论过的所有 gtpo 相关知识整理成一份实现指南，你放到新工程的文档目录里，开新对话时直接引用即可。

```markdown
# GTPO 实现指南

> 从 QuickQanava 项目中分离出的泛型有向图拓扑库。纯头文件，零 Qt 依赖，模板混入式设计。

---

## 一、整体架构

```
abstract_observable                         空接口（类型擦除锚点）
  └── observable<observer_t>                观察者存储容器
        ├── observable_graph<G,N,E,Gr>      图级通知（6 个 notify）
        └── observable_node<N,E>            节点级通知（6 个 notify）

graph_property_impl<graph_t>                "我属于哪张图" 的类型安全指针

edge<edge_base_t, graph_t, node_t>          有向边：src/dst + 图回指
  └── 继承 edge_base_t + graph_property_impl

node<node_base_t, graph_t, node_t, edge_t, group_t>   节点：出入边集合 + 图回指
  └── 继承 node_base_t + graph_property_impl + observable_node

graph<graph_base_t, node_t, group_t, edge_t>          图：节点/边/组的容器 + 拓扑操作
  └── 继承 graph_base_t + observable_graph
```

## 二、核心设计思想

### 2.1 模板参数注入基类

`graph` / `node` / `edge` 全部接受一个"根基类"模板参数，通过继承注入：

```cpp
template <class graph_base_t, ...>
class graph : public graph_base_t { ... };

// 使用时：
class QanGraph : public graph<QQuickItem, ...> { ... };
// 展开后 QanGraph 就是一个 QQuickItem
```

**为什么：** 图拓扑逻辑与基类解耦。换 QObject / 纯 STL / 任何框架只需改一行模板实参。

### 2.2 容器适配器

不同容器（QVector / std::vector / QSet / std::unordered_set）API 不同，用模板全特化统一为 `insert` / `remove` / `contains`：

```cpp
template <typename T> struct container_adapter;  // 主模板空壳

template <> struct container_adapter<QVector<T>> {
    static void insert(T t, QVector<T>& c) { c.append(t); }
    static void remove(const T& t, QVector<T>& c) { c.removeAll(t); }
    static bool contains(const QVector<T>& c, const T& t) { return c.contains(t); }
};

template <> struct container_adapter<std::vector<T>> {
    static void insert(T t, std::vector<T>& c) { c.emplace_back(t); }
    static void remove(const T& t, std::vector<T>& c) {
        c.erase(std::remove(c.begin(), c.end(), t), c.end());
    }
    static bool contains(const std::vector<T>& c, const T& t) {
        return std::find(c.begin(), c.end(), t) != c.end();
    }
};
// QSet / std::unordered_set 同理
```

**调用时：** `container_adapter<nodes_t>::insert(node, _nodes);`
编译器根据 `nodes_t` 的实际类型自动选择正确特化版本，零运行时开销。

### 2.3 观察者模式

三层结构：

```
observable<observer_t>                // 第一层：只负责持有 vector<unique_ptr<observer_t>>
  └── observable_graph<G,N,E,Gr>      // 第二层：固定 observer_t 为 graph_observer，加 notify 方法
        └── graph<...>                // 第三层：在每个拓扑变更方法里调用 notify
```

通知调用链示例：
```
用户调用 insert_edge(src, dst)
  → graph::insert_edge 完成实际拓扑变更（设置 src/dst、更新入度出度、入容器）
  → observable_base_t::notify_edge_inserted(*edge)
    → for (obs : _observers) obs->on_edge_inserted(edge)
```

### 2.4 graph_property_impl — 回指指针混入

```cpp
template<class graph_t>
class graph_property_impl {
    friend graph_t;   // 只有 graph 能写，外部代码只能读
    graph_t* _graph = nullptr;   // 裸指针——图一定比节点/边活得长
};
```

node / edge / group 全部继承它，每个都能 `get_graph()` 拿到所属图的类型安全指针。

### 2.5 为什么 abstract_observable 一个纯虚函数都没有

它只有一个 `virtual ~abstract_observable() = default;`。作用是给所有不同模板实参的 `observable<T>` 提供一个共同的非模板基类，使得可以用 `abstract_observable*` 统一引用，或者 `dynamic_cast<abstract_observable*>(obj)` 判断某对象是否实现了 observable 概念。这是模板世界回到多态世界的出口。

---

## 三、渐进实现步骤

### 第一步：graph_property_impl

最简单的模板类，验证 friend 机制和类型安全指针。

- 单模板参数 `graph_t`
- `friend graph_t`
- `graph_t* _graph = nullptr` 裸指针
- `get_graph()` / `set_graph(graph_t*)` / `set_graph(std::nullptr_t)`

### 第二步：edge

- 三模板参数 `edge_base_t, graph_t, node_t`
- 继承 `edge_base_t` + `graph_property_impl<graph_t>`
- 成员：`node_t* _src` / `node_t* _dst`
- `set_src` / `set_dst` / `get_src` / `get_dst`
- 析构时检查 `_graph != nullptr` → 警告"边未经 remove 就被 delete"
- 此时 node 可用空占位类代替

### 第三步：node

- 五模板参数 `node_base_t, graph_t, node_t, edge_t, group_t`
- 继承 `node_base_t` + `graph_property_impl<graph_t>`
- 成员：
  - `edges_t _in_edges`  (QVector 或 std::vector，存 `const edge_t*`)
  - `edges_t _out_edges`
  - `nodes_t _in_nodes`   (存 `const node_t*`)
  - `nodes_t _out_nodes`
- 方法：
  - `add_in_edge(edge)` / `add_out_edge(edge)` — 同时更新 _in_nodes/_out_nodes
  - `remove_in_edge(edge)` / `remove_out_edge(edge)` — 同上
  - `get_in_degree()` / `get_out_degree()`
  - `get_in_edges()` / `get_out_edges()` — 返回 const 引用
- 此时用硬编码的容器操作（先不用 container_adapter）

### 第四步：container_adapter

把第三步里的容器操作抽出来：

- 主模板空壳
- 全特化 `std::vector<T>` — `insert`(emplace_back) / `remove`(erase-remove) / `contains`(std::find)
- 全特化 `std::unordered_set<T>` — `insert` / `remove`(erase)
- 回过头把第三步 node 里的操作改成 `container_adapter<edges_t>::insert(...)`
- 添加 `nodes_t` / `edges_t` / `nodes_search_t` 等类型别名

### 第五步：graph

- 四模板参数 `graph_base_t, node_t, group_t, edge_t`
- 继承 `graph_base_t`
- 使用容器别名：
  ```cpp
  using nodes_t        = std::vector<node_t*>;
  using nodes_search_t = std::unordered_set<node_t*>;
  using edges_t        = std::vector<edge_t*>;
  using edges_search_t = std::unordered_set<edge_t*>;
  using groups_t       = std::vector<group_t*>;
  ```
- 成员容器：`_nodes / _nodes_search / _edges / _edges_search / _groups`
- 根节点缓存：`_root_nodes`
- 方法：
  - `create_node()` → `make_unique<node_t>`
  - `insert_node(node)` → 设 graph → 入容器 → 入根节点缓存
  - `remove_node(node)` → 先解除 group → 移除所有关联边 → 出容器 → delete
  - `insert_edge(src, dst)` → 创建边 → 绑定 src/dst → 双向 add edge → 更新根节点缓存
  - `remove_edge(edge)` → 双向解绑 → 出容器 → delete
  - `insert_group(group)` → 作为特殊 node 插入 + 入 `_groups`
  - `remove_group(group)` → 解除 group 成员 → 出 `_groups` → remove_node
  - `group_node(node, group)` / `ungroup_node(node, group)`
  - `find_edge(src, dst)` → O(n) 线性搜索
  - `has_edge(src, dst)` / `contains(node)` / `contains(edge)`
- 析构：遍历 `_nodes` 和 `_edges` 逐个 delete
- 所有容器操作全部通过 `container_adapter` 调用

### 第六步（可选）：observable

- `abstract_observable` — 空接口 + virtual 析构
- `observable<observer_t>` — 继承 abstract_observable，持有 `vector<unique_ptr<observer_t>>`
  - `add_observer(unique_ptr)` — 转移所有权
  - `clear()`
  - `hasObservers()`
- `graph_observer<graph_t, node_t, edge_t, group_t>` — 继承 `observer<graph_t>`
  - 6 个虚函数，默认空实现：`on_node_inserted / on_node_removed / on_edge_inserted / on_edge_removed / on_group_inserted / on_group_removed`
  - `_enabled` 开关
- `observable_graph<graph_t, node_t, edge_t, group_t>` — 继承 `observable<graph_observer<...>>`
  - 6 个 `notify_xxx` 方法，遍历 `_observers` 调对应虚函数
- `node_observer<node_t, edge_t>` — 6 个虚函数（出入边的增删清空）
- `observable_node<node_t, edge_t>` — 继承 `observable<node_observer<...>>`，6 个 notify
- 修改第五步 graph：多重继承 `observable_graph`，每个 insert/remove 方法末尾加 `notify_xxx`
- 修改第三步 node：多重继承 `observable_node`，每个 add/remove edge 方法末尾加 `notify_xxx`

---

## 四、关键设计决策速查

| 问题 | 答案 |
|------|------|
| 为什么用裸指针不是 shared_ptr | 图一定比内部元素活得长，Qt/QML 有独立的所有权体系 |
| 为什么 edge 不需要 observable_node | edge 只有一对 src/dst，不需要独立的出入边追踪；边变化走 graph observer |
| 为什么 graph_property_impl 的 set_graph 要 friend | 只有 graph 能改归属，防止外部代码篡改 |
| 为什么 group_t 没有在 gtpo 里独立实现 | group 就是特殊 node（也可继承 node 模板），通过 graph 的 _groups 容器额外管理 |
| 为什么 graph.hpp 分出来不是直接写在 graph.h | 模板声明和实现分离是惯例；.h #include .hpp 对外部透明 |
| 为什么 observable 用 unique_ptr 存 observer | 所有权唯一且清晰；observer 不共享 |

---

## 五、文件清单（建议的目录结构）

```
gtpo/
  graph_property.h        ← 第一步
  edge.h                  ← 第二步
  node.h / node.hpp       ← 第三步
  container_adapter.h     ← 第四步
  graph.h / graph.hpp     ← 第五步
  observable.h            ← 第六步（可选）
  observer.h              ← 第六步（可选）
```

---

## 六、验证方式

每步写一个小 `main()`：

```cpp
// 第五步验证示例
struct MyGraphBase {};  // 空基类
struct MyNodeBase {};   // 空基类
struct MyGroup : public gtpo::node<MyNodeBase, MyGraph, MyGroup, MyEdge, MyGroup> {};

int main() {
    gtpo::graph<MyGraphBase, MyNode, MyGroup, MyEdge> g;
    auto* n1 = g.create_node();
    auto* n2 = g.create_node();
    g.insert_node(n1);
    g.insert_node(n2);
    auto* e = g.insert_edge(n1, n2);
    assert(g.has_edge(n1, n2));
    assert(n2->get_in_degree() == 1);
    g.remove_node(n1);  // 边应被级联删除
    assert(n2->get_in_degree() == 0);
    return 0;
}
```
```

把这个保存为 `docs/gtpo-guide.md`，新对话里我只需要一句"参考 gtpo-guide.md，从第 X 步开始"就能无缝接上。
#pragma once

#include <vector>
#include <unordered_set>
#include <algorithm>

#include<QObject>
#include<QList>
#include<QVector>
#include<QSet>

namespace gtpo {

/*! \brief 容器适配器：用模板全特化抹平不同容器的 API 差异。
 *
 * ============================================================================
 * 深入 C++ 核心知识点
 * ============================================================================
 *
 * 【1. 模板全特化（Explicit / Full Specialization）】
 *
 * ── 是什么 ──
 *
 * 模板全特化是对某个模板参数的【具体类型】，提供一份完全不同的实现。
 *
 *   主模板（Primary Template）：
 *     template <typename T>
 *     struct Foo { void bar() { ... } };
 *
 *   全特化（Explicit Specialization）：
 *     template <>
 *     struct Foo<int> { void bar() { ... } };
 *
 *   调用时，编译器优先选择特化版本：
 *     Foo<double> f1;  f1.bar();  // 走主模板
 *     Foo<int>    f2;  f2.bar();  // 走全特化版本
 *
 * ── 本文件中的应用 ──
 *
 *   template <typename container_t>
 *   struct container_adapter { };                       // 主模板：空壳
 *
 *   template <typename T>
 *   struct container_adapter<std::vector<T>> { ... };   // 全特化：vector 版
 *
 *   template <typename T>
 *   struct container_adapter<std::unordered_set<T>> { ... }; // 全特化：set 版
 *
 * 当调用 container_adapter<std::vector<node_t*>>::insert(...) 时：
 *   编译器匹配第二个特化 → 调用 push_back()
 * 当调用 container_adapter<std::unordered_set<node_t*>>::insert(...) 时：
 *   编译器匹配第三个特化 → 调用 insert()
 *
 * 【参考】《C++ Primer 第5版》§16.4 Template Specialization (P707-714)
 *
 * ============================================================================
 *
 * 【2. 全特化 vs 偏特化 vs 主模板】
 *
 * ┌───────────────────┬───────────────────────────────────────────────────┐
 * │ 形式              │ 含义                                             │
 * ├───────────────────┼───────────────────────────────────────────────────┤
 * │ 主模板            │ template <typename T>                            │
 * │ (Primary)         │ struct Foo { ... };                              │
 * │                   │ 处理一切未被特化的类型                           │
 * ├───────────────────┼───────────────────────────────────────────────────┤
 * │ 偏特化            │ template <typename T>                            │
 * │ (Partial)         │ struct Foo<T*> { ... };                          │
 * │                   │ 仍然有模板参数，只是受限了"形状"                 │
 * │                   │ 例：所有指针类型走偏特化                         │
 * ├───────────────────┼───────────────────────────────────────────────────┤
 * │ 全特化            │ template <>                                      │
 * │ (Explicit/Full)   │ struct Foo<int> { ... };                         │
 * │                   │ 一个具体的类型，模板参数全部锁死                 │
 * └───────────────────┴───────────────────────────────────────────────────┘
 *
 * 本文件全部是【全特化】：每种容器类型都有自己独立的实现。
 *
 * ── 为什么不用偏特化？──
 *
 * 偏特化适合"所有指针走同一份逻辑"/"所有引用走同一份逻辑"这种"形状级"分支。
 * 但这里不同容器（vector / set / QVector / QSet）的 API 互不兼容，必须每个类型
 * 单独写一份实现 → 四个全特化。
 *
 * 【参考】《C++ Primer 第5版》§16.4 Template Specialization (P712-714)
 *         §16.5 Partial Specialization (P714)
 *
 * ============================================================================
 *
 * 【3. 编译期多态（Compile-time Polymorphism）】
 *
 * ── 问题背景 ──
 *
 * 有两种方式能让"不同容器被同一种写法操作"：
 *
 * ┌───────────────────────┬───────────────────────────────────────────────┐
 * │ 方案 A：继承 + 虚函数 │ 方案 B：模板全特化（本文件）                  │
 * ├───────────────────────┼───────────────────────────────────────────────┤
 * │ 需要定义一个公共基类  │ 不需要基类，完全解耦                         │
 * │ 每个容器必须继承它    │ 容器（std::vector 等）无需任何改动           │
 * │ 运行时 dispatch       │ 编译期 dispatch（零开销）                    │
 * │ 有 vtable 空间开销    │ 零空间开销                                   │
 * │ 有虚函数调用开销      │ 零调用开销（直接内联）                       │
 * │ 适合"运行时换策略"    │ 适合"编译期就确定的容器类型"                 │
 * └───────────────────────┴───────────────────────────────────────────────┘
 *
 * ── 为什么选方案 B ──
 *
 * ① 不能改 std::vector / std::unordered_set：它们是标准库的，不可能让它们
 *    去继承某个"公共容器基类"。
 *
 * ② 零成本抽象（Zero-cost Abstraction）：全特化 + static inline 函数，编译后
 *    完全内联，等价于直接调用 push_back / insert，没有任何额外指令。
 *
 * ③ 编译期安全：如果你不小心把不识别的容器类型传给 container_adapter，
 *    编译器会报"implicit instantiation of undefined template"（因为主模板是
 *    空壳，没有 insert/remove 方法），让你在编译期就发现问题，而不是运行时
 *    崩溃。
 *
 * ── 如果你用 Java/C# 思维来看 ──
 *
 *  Java/C# 中通常会定义 IContainer 接口，然后写 ArrayList/SetAdapter 去实现它。
 *  这是【运行时多态】。C++ 中也可以这样做，但模板给了你更高效的选择：
 *  【编译期多态】。没有 interface、没有 vtable、没有 GC，编译完就是最直接的
 *  机器指令。
 *
 * 【参考】《C++ Primer 第5版》§15.1 OOP: An Overview (P591-605) ← 运行时多态
 *         §16.1 Defining a Template (P671-679)               ← 编译期多态
 *
 * ============================================================================
 *
 * 【4. 为什么用 struct 而不是 class】
 *
 * ── struct vs class 的唯一区别 ──
 *
 *    struct Foo {  ... };      // 默认访问权限：public
 *    class  Bar {  ... };      // 默认访问权限：private
 *
 * 除此之外，struct 和 class 在 C++ 中【完全等价】。struct 可以有虚函数、
 * 构造函数、析构函数、继承、模板、成员函数……和 class 一样。
 *
 * ── 为什么这里用 struct ──
 *
 * container_adapter 本质上是一个"静态工具函数集合"——它不管理状态，没有任何
 * 需要隐藏的数据。所有函数都应该是对外公开的（public）。用 struct 省去写
 * public: 标签，代码更清爽：
 *
 *   struct Foo {                          class Foo {
 *       static void bar();                   public: static int bar();
 *   };                                       };
 *   // 等价，左边少写一行
 *
 * 【参考】《C++ Primer 第5版》§7.2 Access Control and Encapsulation (P262-267)
 *
 * ============================================================================
 *
 * 【5. 为什么函数都是 static inline】
 *
 * ── static ──
 *
 * container_adapter 的所有函数不依赖实例状态（没有非静态成员变量），
 * 它们是纯函数——输入是参数，输出是副作用。它们根本不需要"对象"。
 *
 *   container_adapter<std::vector<int>>::insert(42, myVec);
 *   // 不创建 container_adapter 对象，直接用类名调用
 *
 * ── inline ──
 *
 * ① 提示编译器将函数体内联到调用点，消除函数调用开销。（现代编译器对
 *    模板函数默认会尝试内联，但显式 inline 强调了设计意图。）
 *
 * ② 模板定义通常放在头文件（.h）中，多个翻译单元包含同一个头文件时
 *    会产生重复定义。inline 告诉链接器"这些重复定义是允许的，合并它们"。
 *    static 函数也解决重复定义，但语义不同（每个翻译单元有自己的副本）。
 *
 * ── constexpr（无序集合特化中）──
 *
 *   inline static constexpr void insert(T t, std::unordered_set<T>& c, int i) {
 *       c.insert(i, t);
 *   }
 *
 *   该函数接受索引参数 i 但不使用它（unordered_set 没有"按位置插入"的概念），
 *   编译器在 constexpr 上下文中可以把它优化成 no-op 或发出警告。
 *   （实际工程中，这里写 constexpr 更多是一种"签名对齐"——和 vector 特化
 *   保持接口一致，让调用方不加 if/else 就能通过编译。）
 *
 * 【参考】《C++ Primer 第5版》§6.5.2 Inline Functions (P227-229)
 *         §6.5.3 constexpr Functions (P229-231)
 *
 * ============================================================================
 *
 * 【6. erase-remove 惯用法（Erase-Remove Idiom）】
 *
 * vector 特化中的 remove 函数：
 *
 *   c.erase(std::remove(c.begin(), c.end(), t), c.end());
 *
 * 看起来有点绕，实际上分两步：
 *
 *   ① std::remove(begin, end, t)：
 *       把不等于 t 的元素往前搬（移动赋值），返回一个新的"逻辑结束"迭代器。
 *       ⚠️ 不改变容器大小！只是重新排列元素位置。尾部元素仍然存在但值未定义。
 *
 *   ② c.erase(new_end, c.end())：
 *       真正删除尾部无效元素，调整容器 size。
 *
 * ── 为什么叫"Erase-Remove"而不是"Remove-Erase"？──
 *
 * 因为执行顺序是 remove 先跑（整理元素），erase 后跑（真正删除尾部）。
 * 但名字习惯叫 erase-remove，可能是因为最终效果是"erase"。
 *
 *   原始: [A, B, C, B, D]     要删除 B
 *   remove 后: [A, C, D, ?, ?]   new_end 指向第 4 个位置
 *            ──────────┬─────
 *                     ↑ 从这里
 *   erase:   [A, C, D]
 *
 * ── C++20 简化版 ──
 *
 *   std::erase(c, t);   // 一行搞定，不区分容器类型
 *
 * 但在 C++17 时代你需要这个惯用法。QuickQanava 的最低标准就是 C++17。
 *
 * ── 为什么 set 不用 erase-remove？──
 *
 * unordered_set 的元素顺序不固定，它用哈希表组织。"remove + 搬到队尾"对它
 * 毫无意义。set 的 erase 直接接收值或迭代器 → c.erase(t) 就够了。
 *
 * 【参考】《C++ Primer 第5版》§10.3.3 Reordering Algorithms (P404-405)
 *         10.4 Containers and Algorithms (P403-409)
 *
 * ============================================================================
 *
 * 【7. 四个特化之间的差距——"能省则省"的设计哲学】
 *
 *                  insert  insert(i)  remove  size  contains  reserve
 *  std::vector       ✅       ✅       ✅      ✅      ✅       ✅
 *  std::unord_set    ✅       ✅       ✅      —       —        —
 *  QVector           ✅       ✅       ✅      ✅      ✅       ✅
 *  QSet              ✅       ✅       ✅      ✅      ✅       ✅
 *
 * unordered_set 只实现了三个函数，因为：
 *
 *   ① size()：STL 容器统一是 .size()，graph/node 直接调，无需 adapter 包装。
 *   ② contains()：unordered_set 在这个项目里当"查重表"用，查重用 .count() > 0
 *      而非 .contains()（C++20 才有 std::unordered_set::contains）。
 *      调用路径不经过 adapter，就没必要写。
 *   ③ reserve()：unordered_set 确实有 reserve() 预分配桶，但本项目中搜索集合
 *      的容量由 vector 驱动，从不独立 reserve，所以不写。
 *
 * 设计原则：只写【实际会被调用的】接口。adapter 是胶水代码，多写多维护，
 * 少写少出错。
 *
 * 【参考】《C++ Primer 第5版》§11.4 The Unordered Containers (P443-445)
 *
 * ============================================================================
 *
 * 【8. 编译器如何选择特化版本（Overload Resolution for Templates）】
 *
 * 假设调用：
 *
 *   container_adapter<std::vector<int>>::insert(42, v);
 *
 * 编译器匹配过程：
 *
 *   ① 候选集收集
 *      - 主模板 container_adapter<container_t>（container_t = vector<int>）
 *      - 特化   container_adapter<vector<T>>   （T = int）
 *      - 特化   container_adapter<unordered_set<T>>（不匹配，跳过）
 *
 *   ② 择优（Partial Ordering）
 *      - 特化版本比主模板"更特化"（more specialized），优先选择。
 *      - container_adapter<vector<int>> 完全匹配，直接选中。
 *
 *   ③ 实例化（Instantiation）
 *      - T = int 代入特化版本，生成 insert(int, vector<int>&) 的代码。
 *
 *   ④ 实例化的代码被内联到调用点。
 *
 * 如果是没特化过的类型（如 std::list<T>）：
 *
 *   container_adapter<std::list<int>>::insert(42, l);
 *   → 啪！编译报错 "implicit instantiation of undefined template"
 *   因为主模板是空的，没有 insert() 成员。
 *
 * 【参考】《C++ Primer 第5版》§16.4 Template Specialization (P707-714)
 *
 * ============================================================================
 *
 * 【9. 模板特化的"所有权"问题】
 *
 * ── 你只能在全局/命名空间作用域定义特化 ──
 *
 *   namespace gtpo {
 *       template <> struct container_adapter<...> { ... };  // ✅ 命名空间内
 *   }
 *   template <> struct container_adapter<...> { ... };       // ✅ 全局
 *
 *   void func() {
 *       template <> struct container_adapter<...> { ... };   // ❌ 函数内不行！
 *   }
 *
 * 特化版本本质是"给编译器的一条补丁规则"，必须在"类定义的同一作用域"。
 *
 * ── 不能对同一个模板参数做两个全特化 ──
 *
 *   template <typename T> struct container_adapter<std::vector<T>> { ... };
 *   template <typename T> struct container_adapter<std::vector<T>> { ... };
 *   // ❌ 编译错误：duplicate explicit specialization
 *
 * 一个模板参数在全特化中只能出现一次。这和函数重载不同——函数可以有多个
 * 同名重载，但模板特化是"替换主模板的行为"，只能有一份。
 *
 * 【参考】《C++ Primer 第5版》§16.4 Template Specialization (P707-714)
 *
 * ============================================================================
 *
 * 【为什么这些知识点值得你掌握 —— 面试常考总结】
 *
 * ┌─────────────────────────────────────────────────────────────────────────┐
 * │ 面试题                                  │ 对应知识点                    │
 * ├─────────────────────────────────────────────────────────────────────────┤
 * │ "模板全特化和偏特化的区别？"            │ 全特化 vs 偏特化 (§16.4,     │
 * │                                          │ §16.5)                       │
 * │ "struct 和 class 有什么区别？"          │ struct vs class (§7.2)       │
 * │ "什么时候用编译期多态，什么时候用虚函数" │ 编译期多态 vs 运行时多态     │
 * │                                          │ (§15.1 vs §16.1)             │
 * │ "什么是 erase-remove 惯用法？为什么      │ Erase-Remove Idiom (§10.3.3, │
 * │  需要两步？"                             │ §10.4)                       │
 * │ "static 成员函数什么时候用？"            │ static 工具函数 (§7.2)       │
 * │ "inline 关键字在模板中有用吗？"           │ inline 语义 (§6.5.2)         │
 * │ "为什么要搞 adapter，直接调容器 API 不行"│ 适配器模式 / 零成本抽象       │
 * │ "std::erase (C++20) 和 erase-remove     │ STL 算法演进 (§10.4)          │
 * │  有什么区别？"                           │                              │
 * │ "模板特化能在函数内定义吗？"              │ 特化作用域规则 (§16.4)       │
 * └─────────────────────────────────────────────────────────────────────────┘
 *
 * 本文件虽然只 4 个特化 + 一个空壳，却覆盖了模板元编程的核心思想（编译期
 * 多态、特化优先级、零成本抽象、adapter 模式），是理解 C++ 模板在设计模式中
 * 角色的极佳案例。
 */
template <typename container_t>
struct container_adapter {
    // TODO: 主模板空壳
};

/*! \brief 全特化 std::vector<T>
 */
template <typename T>
struct container_adapter<std::vector<T>> {
    inline static void insert(T t,std::vector<T> &c) {
        c.push_back(t);
    }
    inline static void insert(T t,std::vector<T> &c,int i) {
        c.insert(i,t);
    }
    inline static bool remove(T t,std::vector<T> &c) {
        return c.erase(std::remove(c.begin(),c.end(),t),c.end());
    }
    inline static std::size_t size(const std::vector<T> &c) {
        return c.size();
    }
    inline static bool contains(T t,const std::vector<T> &c) {
        return std::find(c.begin(),c.end(),t) != c.end();
    }
    inline static void reserve(std::vector<T> &c,std::size_t n) {
        c.reserve(n);
    }
};

template <typename T>
struct container_adapter<std::unordered_set<T>> {
    inline static void insert(T t,std::unordered_set<T> &c) {
        c.insert(t);
    }
    inline static constexpr void insert(T t,std::unordered_set<T> &c,int i) {
        c.insert(i,t);
    }
    inline static void remove(T t,std::unordered_set<T> &c) {
        c.erase(t);
    }
 
};  
// TODO: 全特化 std::unordered_set<T>
template <typename T>
struct container_adapter<QVector<T>> {
    inline static void insert(T t,QVector<T> &c) {
        c.append(t);
    }
    inline static void insert(T t,QVector<T> &c,int i) {
        c.insert(i,t);
    }
    inline static bool remove(T t,QVector<T> &c) {
        c.removeAll(t);
    }
    inline static std::size_t size(const QVector<T> &c) {
        return c.size();
    }
    inline static bool contains(const QVector<T> &c,T t) {
        return c.contains(t);
    }
    inline static void reserve(QVector<T> &c,std::size_t n) {
        c.reserve(n);
    }
};

template <typename T>
struct container_adapter<QSet<T>> {
    inline static void insert(T t,QSet<T> &c) {
        c.insert(t);
    }
    inline static constexpr void insert(T t,QSet<T> &c,int i) {
        c.insert(i,t);
    }
    inline static void remove(T t,QSet<T> &c) {
        c.remove(t);
    }
    inline static std::size_t size(const QSet<T> &c) {
        return c.size();
    }
    inline static bool contains(T t,const QSet<T> &c) {
        return c.contains(t);
    }
    inline static void reserve(QSet<T> &c,std::size_t n) {
        c.reserve(n);
    }
 
};  

} // ::gtpo

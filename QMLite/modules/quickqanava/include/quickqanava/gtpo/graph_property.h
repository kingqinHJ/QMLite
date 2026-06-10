#pragma once

#include <cstddef>  // std::nullptr_t

namespace gtpo {

/*! \brief "我属于哪张图" 的类型安全回指混入。
 *
 * 被 node / edge / group 继承，每个拓扑元素都能拿到所属图的指针。
 * 使用裸指针：图一定比内部元素活得长，由 graph 统一管理生命周期。
 *
 * ============================================================================
 * 深入 C++ 核心知识点
 * ============================================================================
 *
 * 【1. 三五法则（Rule of Three / Rule of Five）】
 *
 * ── 是什么 ──
 *
 * C++98 时代提出 Rule of Three：
 *   如果一个类需要用户自定义以下三个函数中的【任意一个】，
 *   那么它通常【三个都需要】：
 *
 *   ① 析构函数（Destructor）                  ~T()
 *   ② 拷贝构造函数（Copy Constructor）         T(const T&)
 *   ③ 拷贝赋值运算符（Copy Assignment）        T& operator=(const T&)
 *
 * C++11 引入移动语义后，扩展到 Rule of Five，增加：
 *   ④ 移动构造函数（Move Constructor）         T(T&&)
 *   ⑤ 移动赋值运算符（Move Assignment）        T& operator=(T&&)
 *
 * ── 为什么叫"三五"？──
 *
 * "三" = 析构 + 拷贝构造 + 拷贝赋值（C++98 三个特殊成员函数）
 * "五" = 再加 移动构造 + 移动赋值（C++11 新增两个）
 * "零" = 什么都不需要声明（Rule of Zero，现代 C++ 推荐）
 *
 * 名字来源：这三个/五个函数是 C++ 中仅有的、编译器可以【隐式自动生成】的
 * 特殊成员函数，它们共同决定了对象的【生命周期管理】方式。
 *
 * ── 为什么需要这个规则 ──
 *
 * 当你手动管理资源（堆内存、文件句柄、互斥锁等）时，编译器默认生成的
 * 拷贝/移动操作做的是【逐成员浅拷贝（shallow copy）】。两个对象拷贝后
 * 指向同一块资源，析构时双方都会 delete → double-free（未定义行为），
 * 赋值时原对象持有的资源也没人释放 → 内存泄漏。
 *
 * 简单说：只要你的类需要自定义析构来释放资源，就一定需要配套的拷贝和移动。
 *
 * ── 反例（如果违反规则会怎样）──
 *
 *   class StringBuffer {
 *       char* _buf = new char[100];
 *       // 没有自定义析构 → 内存泄漏！
 *       // 没有自定义拷贝构造 → 两个 StringBuffer 指向同一块内存
 *   };
 *   StringBuffer a;
 *   StringBuffer b = a;   // 浅拷贝：a._buf == b._buf
 *   // 出作用域时，a 和 b 的默认析构都执行 delete[] _buf → double-free！
 *
 * ── 本类为什么可以用 = default ──
 *
 * graph_property_impl 只持有一个【非拥有裸指针】graph_t* _graph，
 * 它不负责销毁 _graph 所指的对象，所以：
 *   - 浅拷贝是正确的（拷贝后两个实例指向同一个 graph）
 *   - 浅移动也是正确的（移动后源实例被置为默认值）
 *   - 析构什么都不做也是正确的
 * 因此五个特殊成员函数全部 = default 即可。
 *
 * ── 决策流程：什么时候声明什么？──
 *
 * ┌────────────────────────────────────────────────────────────────────────────┐
 * │ 场景                         │ 需要声明什么                              │
 * ├────────────────────────────────────────────────────────────────────────────┤
 * │ 手动管理资源（裸指针、文件    │ Rule of Five：                            │
 * │ 句柄、互斥锁等）             │ 析构 + 拷贝构造 + 拷贝赋值 + 移动构造      │
 * │                              │ + 移动赋值（或 = delete 某些操作）         │
 * ├────────────────────────────────────────────────────────────────────────────┤
 * │ 不管理资源（成员都是智能指针  │ Rule of Zero：                            │
 * │ vector 等自动管理成员）       │ 什么都不声明，编译器默认生成的完全够用       │
 * ├────────────────────────────────────────────────────────────┤
 * │ 禁止拷贝（单例、资源句柄、    │ 拷贝构造 = delete                         │
 * │ 不可复制的对象）              │ 拷贝赋值 = delete                         │
 * │                              │ 移动也会自动被删除                        │
 * ├────────────────────────────────────────────────────────────┤
 * │ 裸指针但只是"观察/回指"，    │ 析构 = default（不 delete）                │
 * │ 不拥有指向的对象              │ 拷贝/移动 = default（浅拷贝就够了）         │
 * │                              │ ← 本类 graph_property_impl 属于这种情况    │
 * └────────────────────────────────────────────────────────────────────────────┘
 *
 * 核心判断标准：这个类是否【手动管理资源】？
 *   - 是 → Rule of Five（或部分 = delete）
 *   - 否 → Rule of Zero（什么都不写）
 *   - 不想被拷贝 → = delete 拷贝
 *
 * 【参考】《C++ Primer 第5版》§13.1.4 The Rule of Three/Five (P496-509)
 *
 * ============================================================================
 *
 * 【2. 编译器生成的五个特殊成员函数（Special Member Functions）】
 *
 * C++ 编译器在满足条件时会自动生成以下五个函数。理解它们的生成规则，
 * 是掌握三五法则的前提：
 *
 *   ┌─────────────────────────────┬─────────────────────────────────┐
 *   │ 特殊成员函数                │ 编译器自动生成的条件            │
 *   ├─────────────────────────────┼─────────────────────────────────┤
 *   │ 默认构造函数                │ 没有声明任何构造函数            │
 *   │ T()                         │                                 │
 *   ├─────────────────────────────┼─────────────────────────────────┤
 *   │ 析构函数                    │ 没有声明析构函数                │
 *   │ ~T()                        │                                 │
 *   ├─────────────────────────────┼─────────────────────────────────┤
 *   │ 拷贝构造函数                │ 没有声明拷贝构造函数            │
 *   │ T(const T&)                 │ 并且移动相关也没被声明           │
 *   ├─────────────────────────────┼─────────────────────────────────┤
 *   │ 拷贝赋值运算符              │ 没有声明拷贝赋值运算符          │
 *   │ T& operator=(const T&)      │ 并且移动相关也没被声明           │
 *   ├─────────────────────────────┼─────────────────────────────────┤
 *   │ 移动构造函数                │ 没有声明任何拷贝/移动/析构      │
 *   │ T(T&&)                      │ （即"三五大三无"条件）          │
 *   ├─────────────────────────────┼─────────────────────────────────┤
 *   │ 移动赋值运算符              │ 同移动构造函数                  │
 *   │ T& operator=(T&&)           │                                 │
 *   └─────────────────────────────┴─────────────────────────────────┘
 *
 * 关键结论：一旦你声明了【析构函数】（哪怕是 = default），移动构造和
 * 移动赋值就不会再自动生成了！这就是本类必须显式 = default 移动操作的
 * 原因。
 *
 * 【参考】《C++ Primer 第5版》§13.1.2 Copy Assignment Operator (P499)
 * 【参考】《C++ Primer 第5版》§13.6.2 Move Constructor and Move Assignment (P531)
 *
 * ============================================================================
 *
 * 【3. = default 的含义】
 *
 * 语法：  T(const T&) = default;
 *         T& operator=(T&&) = default;
 *
 * 作用：  显式要求编译器生成该函数的默认版本（即逐成员拷贝/移动）。
 *
 * 两种场景：
 *
 *   场景 A —— 内联默认（in-class default）：
 *       struct Widget {
 *           Widget(const Widget&) = default;  // 隐式 inline
 *       };
 *
 *   场景 B —— 外部默认（out-of-class default）：
 *       struct Widget {
 *           Widget(const Widget&);            // 只声明
 *       };
 *       Widget(const Widget&) = default;       // 在 .cpp 中定义
 *
 * 为什么需要显式写 = default 而不是让编译器隐式生成？
 *
 *   ① 意图文档化：告诉读者"我认真考虑过，编译器默认行为就是我想要的"
 *   ② 性能：有些情况下编译器隐式生成的是"弃置的（deleted）"版本，
 *       = default 可以强制编译器生成"非弃置"版本
 *   ③ 可移植性：某些编译器在复杂继承场景下可能不会隐式生成移动操作
 *
 * 本类：五个函数全部 = default，原因是：
 *   - 声明了虚析构 → 编译器不再隐式生成移动构造/移动赋值
 *   - 但我们的移动语义就是逐成员移动裸指针，完全正确
 *   - 所以显式 = default 让编译器生成它们
 *
 * 【参考】《C++ Primer 第5版》§13.1.5 Using = default (P496-509)
 *
 * ============================================================================
 *
 * 【4. = delete 的含义】
 *
 * 语法：  T(const T&) = delete;
 *
 * 作用：  显式【禁止】该函数的使用。任何调用该函数的代码都会导致
 *         编译错误。
 *
 * 常见场景：
 *
 *   ① 禁止拷贝（NonCopyable 惯用法）：
 *       struct Singleton {
 *           Singleton(const Singleton&) = delete;
 *           Singleton& operator=(const Singleton&) = delete;
 *       };
 *
 *   ② 禁止特定类型的隐式转换（类型安全）：
 *       void set_port(int port);              // 接受 int
 *       void set_port(short) = delete;        // 禁止 short → int 隐式转换
 *       set_port(80);      // 正确
 *       set_port(short{80}); // 编译错误！
 *
 *   ③ 禁止在模板特化中调用某些类型：
 *       template<typename T> void process(T);
 *       template<> void process<void>(void) = delete;  // 禁止 void
 *
 * = delete 与 = default 的对比：
 *
 *   = default  → "编译器，请生成默认版本"
 *   = delete   → "编译器，请拒绝任何调用"
 *
 * 二者都是在【声明处】告诉编译器如何处理特殊成员函数，而不是在定义处。
 *
 * 【参考】《C++ Primer 第5版》§13.1.6 Preventing Copies (P496-509)
 *
 * ============================================================================
 *
 * 【5. 虚析构函数（Virtual Destructor）】
 *
 * ── 为什么需要虚析构 ──
 *
 * C++ 允许通过【基类指针】来 delete 【派生类对象】：
 *
 *   class Base { ~Base(); };               // 非虚析构
 *   class Derived : public Base { ... };
 *
 *   Base* p = new Derived();
 *   delete p;  // 只调用了 Base::~Base()，没有调用 Derived::~Derived()！
 *              // 派生类的资源没有被释放 → 内存泄漏！
 *
 * 解决方式：把基类析构声明为 virtual：
 *
 *   class Base { virtual ~Base(); };        // 虚析构
 *
 *   Base* p = new Derived();
 *   delete p;  // 正确调用 Derived::~Derived() → 再调用 Base::~Base()
 *
 * ── 什么时候必须声明虚析构 ──
 *
 *   当类【作为基类被继承，并且通过基类指针删除派生类对象】时。
 *
 *   注意：并不是所有基类都需要虚析构。
 *   反例：std::enable_shared_from_this<T>、mixin 接口类如果不需要
 *   多态删除，也可以把析构设为 protected 而非 virtual。
 *
 * ── 本类的选择 ──
 *
 * graph_property_impl 声明了 virtual ~graph_property_impl() = default，
 * 因为：
 *   - 它会被 node/edge/group 继承
 *   - 外部代码可能通过 graph_property_impl* 指针操作派生类
 *   - virtual 析构保证了多态删除的安全性
 *
 * 但声明虚析构的【副作用】是：编译器不再自动生成移动构造和移动赋值。
 * 这就是为什么本类需要显式 = default 移动操作的根本原因。
 *
 * 【参考】《C++ Primer 第5版》§15.7.1 Virtual Destructors (P622)
 * 【参考】《C++ Primer 第5版》§13.1.3 The Destructor (P496-509)
 *
 * ============================================================================
 *
 * 【6. 移动语义（Move Semantics）】
 *
 * ── 为什么需要移动 ──
 *
 * C++11 之前，函数返回较大的对象时只能拷贝：
 *
 *   vector<int> makeBigVector() {
 *       vector<int> v(1000000);
 *       return v;           // 拷贝整个数组！很慢
 *   }
 *
 * C++11 引入移动语义后，可以"偷"走源对象的资源：
 *
 *   vector<int> makeBigVector() {
 *       vector<int> v(1000000);
 *       return v;           // C++11 起：移动！几乎零成本
 *   }
 *
 * ── 左值 vs 右值引用 ──
 *
 *   int  a = 42;     // a 是左值（有名字，可取地址）
 *   int  b = a;      // a 是左值
 *   int  c = 42;     // 42 是右值（字面量，没有名字）
 *
 *   int&  lr = a;    // 左值引用：只能绑定左值
 *   int&& rr = 42;   // 右值引用：只能绑定右值（C++11）
 *
 * 移动构造函数接受的是【右值引用】T&&：
 *
 *   class StringBuffer {
 *       StringBuffer(StringBuffer&& other) noexcept
 *           : _buf(other._buf)    // 偷走指针
 *       {
 *           other._buf = nullptr; // 源对象置为合法空状态
 *       }
 *   };
 *
 * ── noexcept 的重要性 ──
 *
 *   移动构造/赋值声明为 noexcept 有两个作用：
 *   ① 承诺不抛异常，标准库容器（如 std::vector）在 reallocate 时
 *      才会选择"移动"而不是"拷贝"（否则强异常安全保证要求拷贝）
 *   ② 编译器可以对 noexcept 函数做更好的优化
 *
 *   本类的移动操作全部 noexcept：
 *     graph_property_impl(graph_property_impl&&) noexcept = default;
 *     因为只移动一个裸指针，确实不会抛异常。
 *
 * 【参考】《C++ Primer 第5版》§13.6.1 Rvalue References (P531)
 * 【参考】《C++ Primer 第5版》§13.6.2 Move Constructor and Move Assignment (P531)
 * 【参考】《C++ Primer 第5版》§13.6.4 Move Operations and noexcept (P536)
 *
 * ============================================================================
 *
 * 【7. RAII（Resource Acquisition Is Initialization）】
 *
 * ── 核心思想 ──
 *
 *   ① 资源获取即初始化：在构造函数中获取资源
 *   ② 资源释放即销毁：在析构函数中释放资源
 *   ③ 对象生命周期结束时（离开作用域 / 异常栈展开），析构自动调用
 *
 * ── 一个完整例子 ──
 *
 *   class FileHandle {
 *       FILE* _fp;
 *   public:
 *       FileHandle(const char* path)
 *           : _fp(fopen(path, "r")) {          // 构造时打开文件
 *           if (!_fp) throw std::runtime_error("open failed");
 *       }
 *       ~FileHandle() {
 *           if (_fp) fclose(_fp);               // 析构时关闭文件
 *       }
 *       // 需要配套的拷贝/移动（三/五法则！）
 *       FileHandle(const FileHandle&) = delete;
 *       FileHandle& operator=(const FileHandle&) = delete;
 *   };
 *
 *   使用 FileHandle 时，绝不担心忘记 fclose()：
 *   {
 *       FileHandle fh("data.txt");   // 打开
 *       // ... 使用文件 ...
 *   }                                // 自动关闭 —— RAII 的威力
 *
 * ── C++ 为什么如此依赖 RAII？──
 *
 *   ① C++ 没有垃圾回收器（不像 Java/C#/Go）
 *   ② 异常安全：栈展开（stack unwinding）保证局部对象析构函数一定被调用
 *   ③ 异常安全的三种保证等级：
 *       - 基本保证：异常发生后程序处于合法状态
 *       - 强保证：异常发生后状态回滚到调用前（RAII + copy-and-swap）
 *       - 不抛保证：承诺绝不抛异常（noexcept，如移动操作）
 *
 * ── RAII 的四个经典例子 ──
 *
 *   std::unique_ptr<T>      : 构造时持有，析构时 delete
 *   std::shared_ptr<T>      : 构造时持有，引用计数归零时 delete
 *   std::lock_guard<Mutex>  : 构造时 lock()，析构时 unlock()
 *   std::fstream            : 构造时 open()，析构时 close()
 *
 * ── RAII 与三五法则的关系 ──
 *
 *   RAII 的类一定【管理某种资源】，而管理资源意味着需要自定义析构。
 *   需要自定义析构 → 需要配套拷贝/移动（三五法则）。
 *
 *   例外：如果管理资源的类【禁止拷贝和移动】（如 std::unique_ptr 禁止
 *   拷贝，但允许移动），则只需要：
 *     - 析构函数（释放资源）
 *     - 移动构造 / 移动赋值（转移资源所有权）
 *     - 拷贝 = delete（禁止拷贝）
 *
 * ── 本类为什么不涉及 RAII？──
 *
 *   graph_property_impl 的 _graph 是"回指指针"（back-pointer），
 *   它不代表所有权。图拥有节点，不是节点拥有图。
 *   因此：
 *     - 析构不需要 delete _graph
 *     - 拷贝 = default（浅拷贝就对了）
 *     - 移动 = default（裸指针移动就对了）
 *
 *   真正的 RAII 在 graph 类中：graph 的析构函数负责销毁所有节点和边。
 *
 * 【参考】《C++ Primer 第5版》§12.1 Dynamic Memory and Smart Pointers (P450)
 * 【参考】《C++ Primer 第5版》§13.1.4 The Rule of Three/Five (P496-509)
 *
 * ============================================================================
 *
 * 【8. noexcept 说明符】
 *
 * C++11 引入 noexcept，表示函数承诺不抛异常：
 *
 *   void func() noexcept;         // 不抛异常
 *   void func() noexcept(false);  // 可能抛异常
 *
 * 如果 noexcept 函数内部抛异常，程序会直接调用 std::terminate() 终止。
 *
 * 移动操作声明为 noexcept 的意图：
 *   ① 性能：std::vector 扩容时，如果 move constructor 是 noexcept，
 *      就敢放心地移动元素而不是拷贝（因为移动失败后无法回滚）
 *   ② 契约：承诺移动操作不会失败
 *
 * 本类：五个特殊成员函数全部 noexcept：
 *
 *   ~graph_property_impl() noexcept = default;
 *   graph_property_impl(graph_property_impl&&) noexcept = default;
 *   ...
 *
 * 因为只操作裸指针，确实不会抛异常，用 noexcept 给编译器更多优化空间。
 *
 * 注意：析构函数默认就是 noexcept，不写也是 noexcept。但移动构造和移动
 * 赋值默认不是 noexcept，需要显式声明。
 *
 * 【参考】《C++ Primer 第5版》§18.1.4 The noexcept Exception Specification (P774)
 *
 * ============================================================================
 *
 * 【9. 友元（friend）—— 控制访问权限】
 *
 *   friend graph_t;
 *
 * 含义：graph_t（即 gtpo::graph<...> 的实例化类型）可以访问本类的
 * 所有 private/protected 成员。
 *
 * 作用：只有 graph 能调用 set_graph() 来绑定/解绑节点与图的关系，
 * 外部代码无法绕过 graph 直接操作这个指针。
 *
 * 对比两种方案：
 *
 *   方案 A（公开 setter）：
 *       public: void set_graph(graph_t* g) { _graph = g; }
 *       → 外部代码可以随意把节点"塞进"不属于它的图，破坏拓扑一致性
 *
 *   方案 B（友元 + private）：
 *       friend graph_t;
 *   private: void set_graph(graph_t* g);
 *       → 只有 graph 内部代码能设置归属关系，安全得多
 *
 * 【参考】《C++ Primer 第5版》§7.2.1 Friends (P263)
 *
 * ============================================================================
 *
 * 【10. trailing return type（尾置返回类型）】
 *
 *   inline auto get_graph() noexcept -> graph_t*;
 *
 * C++11 允许把返回类型写在函数参数列表后面，用 -> 引导。
 *
 * 为什么用这种语法？
 *   ① 一致性：lambda 表达式就是 auto ... -> ReturnType 语法
 *   ② 模板编程中必要：返回类型依赖于模板参数时只能尾置
 *       template<typename T>
 *       auto make_container() -> std::vector<T> { ... }
 *   ③ 可读性：对于长返回类型，名字在前更容易阅读
 *
 * 本类：get_graph() 返回 graph_t*，用尾置返回主要是统一风格。
 *
 * 【参考】《C++ Primer 第5版》§6.3.3 Trailing Return Type (P225)
 *
 * ============================================================================
 *
 * 【为什么这些知识点值得你掌握 —— 面试常考总结】
 *
 * ┌─────────────────────────────────────────────────────────────────────────┐
 * │ 面试题                           │ 对应知识点                         │
 * ├─────────────────────────────────────────────────────────────────────────┤
 * │ "什么是 C++ 的三五法则？"         │ Rule of Three/Five (§13.1.4)       │
 * │ "= default 和 = delete 的区别？"  │ = default (§13.1.5) / = delete    │
 * │                                    │ (§13.1.6)                       │
 * │ "为什么移动操作要加 noexcept？"    │ noexcept (§18.1.4)               │
 * │ "什么时候必须声明虚析构？"         │ Virtual Destructor (§15.7.1)     │
 * │ "什么是 RAII？举个例子。"          │ RAII (§12.1)                     │
 * │ "移动语义和拷贝语义的区别？"       │ Move Semantics (§13.6)           │
 * │ "友元的优缺点？"                   │ friend (§7.2.1)                  │
 * │ "尾置返回类型有什么用？"           │ Trailing Return Type (§6.3.3)    │
 * └─────────────────────────────────────────────────────────────────────────┘
 *
 * 本文件虽然只有短短一个类，却覆盖了 C++ 核心（三五法则、移动语义、RAII、
 * 虚析构、noexcept、友元、尾置返回类型），是学习现代 C++ 的极佳切入点。
 */
template <class graph_t>
class graph_property_impl {
    friend graph_t;   // 只有 graph 能调用 set_graph()

public:
    graph_property_impl() = default;

    // 虚析构：本类作为基类被继承，必须有虚析构防止多态 delete 时内存泄漏
    // 【参考】《C++ Primer 第5版》§15.7.1 Virtual Destructors (P622)
    virtual ~graph_property_impl() = default;

    // 拷贝操作：= default（逐成员拷贝裸指针，不拥有所有权所以浅拷贝就够了）
    // 【参考】《C++ Primer 第5版》§13.1.5 Using = default (P496-509)
    graph_property_impl(const graph_property_impl&) = default;
    graph_property_impl& operator=(const graph_property_impl&) = default;

    // 移动操作：必须显式 = default
    // 原因：声明了虚析构（即使是 = default）后，编译器不再自动生成移动操作
    // 本类移动语义正确，所以显式要求编译器生成
    // noexcept：移动操作只操作裸指针，不会抛异常
    // 【参考】《C++ Primer 第5版》§13.6.2 Move Constructor and Move Assignment (P531)
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

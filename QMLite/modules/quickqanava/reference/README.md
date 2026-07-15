# reference/ — QuickQanava Qt 专版精简参考

## 文件清单

| 文件 | 类 | 职责 |
|------|-----|------|
| `Node.h` / `Node.cpp` | `qan::Node` | GTPO 拓扑节点 + QObject 属性（label/locked） |
| `Edge.h` / `Edge.cpp` | `qan::Edge` | GTPO 边 + QObject 属性（label/weight） |
| `Group.h` / `Group.cpp` | `qan::Group` | 继承 Node，is_group=true |
| `Graph.h` / `Graph.cpp` | `qan::Graph` | QML 顶层图容器，创建 NodeItem/EdgeItem |
| `NodeItem.h` / `NodeItem.cpp` | `qan::NodeItem` | 可视化节点：矩形 + 拖拽 + 标签文字 |
| `EdgeItem.h` / `EdgeItem.cpp` | `qan::EdgeItem` | 可视化边：直线/曲线 + 箭头 |
| `NodeStyle.h` | `qan::NodeStyle / EdgeStyle` | 样式空壳占位 |

## 不包含的功能

- qanStyle.h / qanStyleManager — 样式管理（Phase 3+）
- qanBehaviour.h — 节点行为观察者（Phase 3+）
- qanTableCell.h — 表格组（Phase 4）
- qanConnector.h — 可视化连线（Phase 3+）
- qanSelectable.h / qanDraggable — 选择/拖拽抽象（Phase 3+）
- qanNavigable — 导航/预览（Phase 4）

## 实现顺序建议

1. Node.h / Edge.h / Group.h / NodeStyle.h — 纯头文件，无 .cpp
2. Graph.h — 引入 QQmlParserStatus，需要 .cpp 实现 componentComplete
3. NodeItem.h — QQuickItem 绘制 + 拖拽，需要 paint, mousePressEvent
4. EdgeItem.h — QQuickItem 绘制连线，需要 updateItem()

## 与原版的差异

- 原版用 `QPointer<T>` + `qcm::Container` 暴露 QML 列表模型
  本版直接 Q_PROPERTY + QML_ELEMENT
- 原版有 Style + StyleManager + 样式继承链
  本版 NodeItem 内置样式属性，后续可挂接
- 原版支持 Connector（拖拽连线）、Port（节点端口）、Resizer
  本版暂不实现，Phase 2 只做基础可视化

#pragma once

// ============================================================================
// NodeStyle.h — 节点样式（精简版）
// ============================================================================
//
// 暂时定义 NodeStyle 和 EdgeStyle 为 QObject 子类的空壳，
// 不做实际样式管理。NodeItem/EdgeItem 的绘制属性已经直接内置了
// backColor/borderColor 等 Q_PROPERTY，暂时不依赖 Style 对象。
//
// 后续 Phase 3/4 可以实现完整的 Style + StyleManager。

#include <QObject>
#include <QQmlEngine>

namespace qan {

class NodeStyle : public QObject
{
    Q_OBJECT
    QML_ELEMENT
public:
    explicit NodeStyle(QObject* parent = nullptr) : QObject(parent) {}
};

class EdgeStyle : public QObject
{
    Q_OBJECT
    QML_ELEMENT
public:
    explicit EdgeStyle(QObject* parent = nullptr) : QObject(parent) {}
};

} // ::qan

QML_DECLARE_TYPE(qan::NodeStyle)
QML_DECLARE_TYPE(qan::EdgeStyle)

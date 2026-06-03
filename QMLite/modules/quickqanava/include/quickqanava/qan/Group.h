#pragma once

#include <quickqanava/qan/Node.h>

namespace qan {

// Phase 2+：Group 是一种特殊的 Node
//
// TODO:
// - class Group : public qan::Node
// - 附加属性：容纳子节点的能力
// - 视觉表现：一个可折叠的矩形容器

class Group : public Node
{
    Q_OBJECT
    // TODO: Phase 2 开始实现
};

} // ::qan

QML_DECLARE_TYPE(qan::Group)

#pragma once

#include <vector>
#include <unordered_set>
#include <algorithm>

namespace gtpo {

/*! \brief 容器适配器：统一 std::vector / std::unordered_set 的 insert/remove/contains 接口。
 *
 * TODO:
 * - 主模板空壳
 * - 全特化 std::vector<T>
 * - 全特化 std::unordered_set<T>
 */
template <typename container_t>
struct container_adapter {
    // TODO: 主模板空壳
};

// TODO: 全特化 std::vector<T>

// TODO: 全特化 std::unordered_set<T>

} // ::gtpo

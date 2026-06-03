cmake_minimum_required(VERSION 3.16)

# QuickQanava 模块化构建配置
#
# 使用方式（在主工程 CMakeLists.txt 中）：
#   add_subdirectory(modules/quickqanava)
#   target_link_libraries(YourTarget PRIVATE quickqanava)
#
# 构建模式切换（在主工程 add_subdirectory 之前设置）：
#   set(QUICKQANAVA_BUILD_STATIC ON)   # 编译为静态库
#   set(QUICKQANAVA_BUILD_SHARED ON)   # 编译为动态库
#   # 默认：OBJECT 库（源码直接合并到主工程目标中）

# ───────────────────────────────────────────────────────────────
# 收集文件
# ───────────────────────────────────────────────────────────────

file(GLOB_RECURSE QUICKQANAVA_GTPO_HEADERS CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/include/quickqanava/gtpo/*.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/include/quickqanava/gtpo/*.hpp"
)

file(GLOB_RECURSE QUICKQANAVA_QAN_HEADERS CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/include/quickqanava/qan/*.h"
)

file(GLOB_RECURSE QUICKQANAVA_SOURCES CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
)

# ───────────────────────────────────────────────────────────────
# 创建库目标（三种模式可选）
# ───────────────────────────────────────────────────────────────

# 当前为骨架阶段：qan 层头文件含 Q_OBJECT 但无实现，先跳过 MOC
# 待 Phase 2 填充 .cpp 后可移除此设置
set_source_files_properties(${QUICKQANAVA_QAN_HEADERS} PROPERTIES SKIP_AUTOMOC ON)

if(QUICKQANAVA_BUILD_SHARED)
    # 动态库模式
    add_library(quickqanava SHARED
        ${QUICKQANAVA_GTPO_HEADERS}
        ${QUICKQANAVA_QAN_HEADERS}
        ${QUICKQANAVA_SOURCES}
    )
    target_compile_definitions(quickqanava PRIVATE QUICKQANAVA_BUILD_DLL)
    target_compile_definitions(quickqanava INTERFACE QUICKQANAVA_USE_DLL)

elseif(QUICKQANAVA_BUILD_STATIC)
    # 静态库模式
    add_library(quickqanava STATIC
        ${QUICKQANAVA_GTPO_HEADERS}
        ${QUICKQANAVA_QAN_HEADERS}
        ${QUICKQANAVA_SOURCES}
    )
    target_compile_definitions(quickqanava PUBLIC QUICKQANAVA_STATIC)

else()
    # 默认：OBJECT 库（源码直接编译进链接了 quickqanava 的目标）
    # 适合开发阶段，头文件改动无需重新链接库
    add_library(quickqanava OBJECT
        ${QUICKQANAVA_GTPO_HEADERS}
        ${QUICKQANAVA_QAN_HEADERS}
        ${QUICKQANAVA_SOURCES}
    )
endif()

# ───────────────────────────────────────────────────────────────
# 配置 include 路径
# ───────────────────────────────────────────────────────────────

target_include_directories(quickqanava
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
)

# ───────────────────────────────────────────────────────────────
# 链接 Qt（由主工程已找到的 Qt 版本决定）
# ───────────────────────────────────────────────────────────────

if(NOT TARGET Qt${QT_VERSION_MAJOR}::Core)
    message(FATAL_ERROR "QuickQanava: Qt::Core target not found. Ensure find_package(Qt...) is called in the parent project before add_subdirectory(modules/quickqanava).")
endif()

target_link_libraries(quickqanava
    PUBLIC
        Qt${QT_VERSION_MAJOR}::Core
        Qt${QT_VERSION_MAJOR}::Quick
)

# ───────────────────────────────────────────────────────────────
# 分组显示（IDE 友好）
# ───────────────────────────────────────────────────────────────

source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}/include/quickqanava" PREFIX "Headers" FILES ${QUICKQANAVA_GTPO_HEADERS} ${QUICKQANAVA_QAN_HEADERS})
source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}/src" PREFIX "Sources" FILES ${QUICKQANAVA_SOURCES})

import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

Page {
    id: root
    title: "QuickQanava"

    RowLayout {
        anchors.fill: parent
        spacing: 0

        // 左侧 Phase 选择列表
        Rectangle {
            Layout.preferredWidth: 240
            Layout.fillHeight: true
            color: "#f0f0f0"

            ColumnLayout {
                anchors.fill: parent
                spacing: 0

                Label {
                    text: "QuickQanava"
                    font.pixelSize: 18
                    font.bold: true
                    Layout.margins: 16
                }

                Label {
                    text: "实现阶段"
                    font.pixelSize: 12
                    color: "#666666"
                    Layout.leftMargin: 16
                    Layout.bottomMargin: 8
                }

                ListView {
                    id: phaseList
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    currentIndex: 0
                    clip: true

                    model: ListModel {
                        ListElement { title: "Phase 1 — 拓扑引擎"; source: "qrc:/pages/GraphPhase1.qml"; desc: "纯模板有向图拓扑" }
                        ListElement { title: "Phase 2 — 基础可视化"; source: "qrc:/pages/GraphPhase2.qml"; desc: "节点/边图形项 + 拖拽" }
                        ListElement { title: "Phase 3 — 视图容器"; source: "qrc:/pages/GraphPhase3.qml"; desc: "缩放/平移/无限画布" }
                        ListElement { title: "Phase 4 — 完整功能"; source: "qrc:/pages/GraphPhase4.qml"; desc: "委托/贝塞尔/Group/Port" }
                    }

                    delegate: ItemDelegate {
                        width: phaseList.width
                        highlighted: ListView.isCurrentItem

                        contentItem: ColumnLayout {
                            spacing: 2
                            Label {
                                text: model.title
                                font.bold: highlighted
                                color: highlighted ? "#1976d2" : "#333333"
                                Layout.fillWidth: true
                            }
                            Label {
                                text: model.desc
                                font.pixelSize: 11
                                color: "#888888"
                                Layout.fillWidth: true
                            }
                        }

                        onClicked: {
                            phaseList.currentIndex = index
                            contentLoader.source = model.source
                        }
                    }

                    ScrollIndicator.vertical: ScrollIndicator {}
                }

                Rectangle {
                    Layout.fillWidth: true
                    Layout.preferredHeight: 1
                    color: "#d0d0d0"
                }

                Label {
                    text: "参考: docs/gtpo-guide.md"
                    font.pixelSize: 10
                    color: "#999999"
                    Layout.margins: 12
                }
            }
        }

        // 中间分割线
        Rectangle {
            Layout.preferredWidth: 1
            Layout.fillHeight: true
            color: "#d0d0d0"
        }

        // 右侧内容显示区
        Loader {
            id: contentLoader
            Layout.fillWidth: true
            Layout.fillHeight: true
            source: "qrc:/pages/GraphPhase1.qml"
        }
    }
}

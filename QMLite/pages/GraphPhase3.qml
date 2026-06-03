import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

Page {
    id: root
    title: "Phase 3: Graph View Container"

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 20
        spacing: 16

        Label {
            text: "Phase 3 — 视图容器（缩放 / 平移）"
            font.pixelSize: 20
            font.bold: true
            Layout.alignment: Qt.AlignHCenter
        }

        Label {
            text: "目标：GraphView 容器 + 无限画布 + 滚轮缩放 + 拖拽平移\n\n"
                  + "所有节点和边放在一个可导航的容器内，\n"
                  + "支持鼠标滚轮缩放、右键/中键平移。"
            horizontalAlignment: Text.AlignHCenter
            wrapMode: Text.Wrap
            Layout.fillWidth: true
        }

        Rectangle {
            Layout.fillWidth: true
            Layout.fillHeight: true
            color: "#f3e5f5"
            radius: 8
            border.color: "#ce93d8"
            border.width: 2

            Label {
                anchors.centerIn: parent
                text: "GraphView 占位\n\n"
                      + "实现后：支持滚轮缩放和拖拽平移的无限画布"
                horizontalAlignment: Text.AlignHCenter
            }
        }
    }
}

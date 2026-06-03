import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

Page {
    id: root
    title: "Phase 2: Basic Visual Nodes"

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 20
        spacing: 16

        Label {
            text: "Phase 2 — 基础可视化节点"
            font.pixelSize: 20
            font.bold: true
            Layout.alignment: Qt.AlignHCenter
        }

        Label {
            text: "目标：qan::NodeItem (QQuickItem) + 拖拽 + 简单连线\n\n"
                  + "GTPO 数据模型接上 Qt，每个节点对应一个可视矩形。\n"
                  + "支持鼠标拖拽移动位置。"
            horizontalAlignment: Text.AlignHCenter
            wrapMode: Text.Wrap
            Layout.fillWidth: true
        }

        Rectangle {
            Layout.fillWidth: true
            Layout.fillHeight: true
            color: "#e8f4f8"
            radius: 8
            border.color: "#90caf9"
            border.width: 2

            Label {
                anchors.centerIn: parent
                text: "可视化画布占位\n\n"
                      + "实现后：这里显示可拖拽的节点和直线边"
                horizontalAlignment: Text.AlignHCenter
            }
        }
    }
}

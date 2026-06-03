import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

Page {
    id: root
    title: "Phase 4: Full QuickQanava"

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 20
        spacing: 16

        Label {
            text: "Phase 4 — 完整功能"
            font.pixelSize: 20
            font.bold: true
            Layout.alignment: Qt.AlignHCenter
        }

        Label {
            text: "目标：QML 委托系统 + 贝塞尔曲线边 + Group + Port + Style + 选择框\n\n"
                  + "参考 QuickQanava 的完整能力，实现：\n"
                  + "• 节点/边/组的 QML delegate 自定义皮肤\n"
                  + "• 贝塞尔曲线和正交连线\n"
                  + "• 连接端口（Port/Dock）\n"
                  + "• 多选框 + 对齐工具\n"
                  + "• 样式系统"
            horizontalAlignment: Text.AlignHCenter
            wrapMode: Text.Wrap
            Layout.fillWidth: true
        }

        Rectangle {
            Layout.fillWidth: true
            Layout.fillHeight: true
            color: "#e8f5e9"
            radius: 8
            border.color: "#a5d6a7"
            border.width: 2

            Label {
                anchors.centerIn: parent
                text: "完整版画布占位\n\n"
                      + "实现后：功能对标 QuickQanava 的 GraphView"
                horizontalAlignment: Text.AlignHCenter
            }
        }
    }
}

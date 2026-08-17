import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import com.quickqanava 1.0

Page {
    id: root
    title: "Phase 2: Basic Visual Nodes"

    // 记录最近两个节点，用于"连接"操作
    property int nodeCount: 0
    property var lastNode: null
    property var prevNode: null

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
                  + "点击「添加节点」创建可拖拽的矩形节点，\n"
                  + "点击「连接最后两个节点」在它们之间画一条直线。"
            horizontalAlignment: Text.AlignHCenter
            wrapMode: Text.Wrap
            Layout.fillWidth: true
        }

        // ── 控制按钮 ──
        RowLayout {
            Layout.alignment: Qt.AlignHCenter
            spacing: 8

            Button {
                text: "添加节点"
                onClicked: {
                    var x = 40 + Math.random() * Math.max(100, canvas.width - 200)
                    var y = 40 + Math.random() * Math.max(100, canvas.height - 140)
                    prevNode = lastNode
                    lastNode = graph.insertNode(x, y, "Node " + (++nodeCount))
                }
            }

            Button {
                text: "连接最后两个节点"
                enabled: nodeCount >= 2
                onClicked: {
                    if (lastNode && prevNode)
                        graph.insertEdge(prevNode, lastNode)
                }
            }

            Button {
                text: "清空"
                onClicked: {
                    graph.clearGraph()
                    nodeCount = 0
                    lastNode = null
                    prevNode = null
                }
            }
        }

        // ── 可视化画布 ──
        Rectangle {
            id: canvas
            Layout.fillWidth: true
            Layout.fillHeight: true
            color: "#e8f4f8"
            radius: 8
            border.color: "#90caf9"
            border.width: 2
            clip: true

            // 图：qan::Graph（数据模型 + 图形项容器）
            Graph {
                id: graph
                anchors.fill: parent
                objectName: "graph"
            }
        }
    }
}

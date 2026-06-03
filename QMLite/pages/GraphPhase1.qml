import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import com.mycompany.gtpo 1.0

Page {
    id: root
    title: "Phase 1: GTPO Topology Engine"

    GtpoTest {
        id: gtpoTester
    }

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 20
        spacing: 16

        Label {
            text: "Phase 1 — 纯模板拓扑引擎（零 Qt 依赖）"
            font.pixelSize: 20
            font.bold: true
            Layout.alignment: Qt.AlignHCenter
        }

        Label {
            text: "实现内容：gtpo::graph / node / edge / container_adapter\n"
                  + "所有代码位于 modules/quickqanava/include/quickqanava/gtpo/\n\n"
                  + "请按 docs/gtpo-guide.md 的六步渐进指南逐步填充代码，\n"
                  + "完成后点击下方按钮运行测试。"
            horizontalAlignment: Text.AlignHCenter
            wrapMode: Text.Wrap
            Layout.fillWidth: true
        }

        Rectangle {
            Layout.fillWidth: true
            Layout.fillHeight: true
            color: "#f5f5f5"
            radius: 8

            ScrollView {
                anchors.fill: parent
                anchors.margins: 12

                TextArea {
                    id: resultArea
                    text: "等待实现..."
                    readOnly: true
                    wrapMode: Text.Wrap
                    background: null
                    font.family: "Consolas"
                    font.pixelSize: 13
                }
            }
        }

        Button {
            text: "运行拓扑测试"
            Layout.alignment: Qt.AlignHCenter
            onClicked: {
                resultArea.text = gtpoTester.runPhase1Test()
            }
        }
    }
}

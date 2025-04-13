import QtQuick 2.15
import com.mycompany.myParserStatus 1.0

Item {
    MyParserStatus{
        id:expensiveObject
    }

    // 显示昂贵操作的结果，初始为空，操作完成后更新为结果文本。对优化性能有用。
    Text {
        id: resultText
        text: expensiveObject.result !== "" ? expensiveObject.result : "等待昂贵操作完成..."
        anchors.top: expensiveObject.bottom
        anchors.horizontalCenter: parent.horizontalCenter
        font.pointSize: 14
        color: "black"
        anchors.topMargin: 20
    }
}

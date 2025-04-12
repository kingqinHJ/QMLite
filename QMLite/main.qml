import QtQuick 2.15
import QtQuick.Window 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

ApplicationWindow {
    id:window
    width: 500
    height: 400
    visible: true
    title: message ? message.name : "Default Title"

    Action {
        id: navigateBackAction
        icon.source: stackView.depth > 1 ? "icons/back.png" : "icons/drawer.png"
        onTriggered: {
            if (stackView.depth > 1) {
                stackView.pop()
                listView.currentIndex = -1
            } else {
                drawer.open()
            }
        }
    }

    header: ToolBar {
        id:header
        RowLayout {
            spacing: 20
            anchors.fill: parent

            ToolButton {
                action: navigateBackAction
            }

            Label {
                id: titleLabel
                text: listView.currentItem ? listView.currentItem.text : message.name
                font.pixelSize: 20
                elide: Label.ElideRight
                horizontalAlignment: Qt.AlignHCenter
                verticalAlignment: Qt.AlignVCenter
                Layout.fillWidth: true
            }
        }
    }

    //Drawer提供了一个基于滑动的侧板，类似于触摸界面中经常使用的那些，为导航提供了一个中心位置。
    Drawer {
        id: drawer
        width: Math.min(window.width, window.height) / 3 * 2
        height: window.height
        interactive: stackView.depth === 1

        ListView {
            id: listView

            focus: true
            currentIndex: -1
            anchors.fill: parent

            delegate: ItemDelegate {
                width: listView.width
                text: model.title
                highlighted: ListView.isCurrentItem
                onClicked: {
                    listView.currentIndex = index
                    stackView.push(model.source)
                    drawer.close()
                }
            }

            model: ListModel {
                ListElement { title: "CPlusPlusProperty"; source: "qrc:/CPlusPlusProperty.qml" }
                ListElement { title: "CPlusPlusPropertyList"; source: "qrc:/MessageList.qml" }
                ListElement { title: "PropertyValueSources"; source: "qrc:/PropertyValueSources.qml" }
            }

            ScrollIndicator.vertical: ScrollIndicator { }
        }
    }

    //QML的stackview和C++不同，stackview只能动态加载
    StackView {
        id: stackView
        anchors.fill: parent
        initialItem:  Pane {
            id: pane
            Label {
                text: qsTr("本项目用于演示QML所有模块，功能特性")
                anchors.margins: 20
                anchors.fill: parent
                horizontalAlignment: Label.AlignHCenter
                verticalAlignment: Label.AlignVCenter
                wrapMode: Label.Wrap
                font.pointSize:30
            }
        }
    }
}

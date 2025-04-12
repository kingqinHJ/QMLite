import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import com.mycompany.message 1.0
import com.mycompany.messagebody 1.0

Rectangle {
    color: "#f0f0f0"
    border.color: "#cccccc"
    border.width: 1

    Messagebody{
        id:tembody
    }

    ColumnLayout{
        anchors.fill: parent
        anchors.margins: 10
        spacing: 5

        RowLayout{
            Layout.fillWidth: true
            spacing:10
            TextField{
                id:messageInput
                Layout.fillWidth: true
                placeholderText: qsTr("请输入消息")
                onAccepted: {
                    if(text.trim()!="")
                    {
                        tembody.setContent(text)
                        message.appendMessagebody(tembody);
                        text=""
                    }
                }
            }
            Button{
                text:qsTr("添加")
                onClicked: {
                    if(messageInput.text.trim()!="")
                    {
                        tembody.setContent(text)
                        message.appendMessagebody(tembody);
                        messageInput.text=""
                    }
                }
            }
        }

        ListView{
            Layout.fillWidth: true
            Layout.fillHeight: true
            clip:true
            spacing:5
            model:message.bodyList

            delegate: Rectangle{
                width: parent.width
                height: 40
                color: "#ffffff"
                border.color: "#dddddd"
                border.width: 1
                radius: 4

                Text{
                    anchors.fill: parent
                    anchors.margins: 10
                    text:modelData.content
                    verticalAlignment: Text.AlignVCenter
                }
            }
        }
    }
}

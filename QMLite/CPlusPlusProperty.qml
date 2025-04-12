import QtQuick 2.15
import QtQuick.Layouts 1.15
import QtQuick.Controls 2.15
import com.mycompany.message 1.0
import com.mycompany.messagebody 1.0
import com.mycompany.messageboard 1.0

Item {
    Item {
        id: name
        width:400
        height:400
        Message{
            id:message
            property int index: 1
            body:Messagebody{
                content:qsTr("帅哥")
            }
            author.name:"MR QIN"
            author.email:"844926748@qq.com"
            onTestSignal:console.log("slot")
//            MessageBoard.onExpireChange:console.log("Message 的附加属性信号触发")             //附加属性
        }

        MessageBoard{
            id:attachBody
            MessageBoard.onExpireChange:
            {
                console.log("美女")
            }
        }

        Timer {
            interval: 100; running: true; repeat: true
            onTriggered: {
                 //对 MessageBoard 实例使用附加属性
//                attachBody.MessageBoard.expire = !attachBody.MessageBoard.expire  // 修改 expire 属性触发信号
                 //也可以尝试对 Message 使用附加属性
                message.MessageBoard.expire = !message.MessageBoard.expire  // 修改 expire 属性触发信号
            }
        }

        GridLayout {
            id: grid
            anchors.fill: parent
            rows:2
            columns: 2
            Layout.margins:10

            Button{
                Layout.row:0
                Layout.column:0
                onClicked:{
                    ++message.index;
                }
            }

            Text {
                Layout.row:0
                Layout.column:1
            }
        }
    }
}

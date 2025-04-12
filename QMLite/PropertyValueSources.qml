import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import com.mycompany.RandomNumberGenerator 1.0
import com.mycompany.messageQueue 1.0
import com.mycompany.message 1.0

Item {
    width: 300; height: 300

    Rectangle {
        RandomNumberGenerator on width { maxValue: 300 }

        height: 100
        color: "red"
    }

    MessageQueue
    {
        id:defaultProperty
        curMessage:Message{
            name:"unit state"
        }

        Message{
            name:"china"
        }

        Message{
            name:"japan"
        }

        Component.onCompleted: {
            console.log("Message count:", messagesCount) // 应输出 2
            console.log("First message:", message(0).text) // 应输出 "Hello"
            curMessage = message(1) // 设置 curMessage
        }
    }

}

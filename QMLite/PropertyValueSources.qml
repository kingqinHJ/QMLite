import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import com.mycompany.RandomNumberGenerator 1.0
import com.mycompany.messageQueue 1.0
import com.mycompany.message 1.0

//此处为附加属性
Item {
    width: 300; height: 300

    Rectangle {
        RandomNumberGenerator on width { maxValue: 300 }           //属性修饰符类型，C++属性变化反馈到QML

        height: 100
        color: "red"
    }

    MessageQueue                                 //默认属性，在父项中添加数据，如果不指定子对象，则自动赋值给默认属性
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
            // console.log("First message:", message(0).text) // 应输出 "Hello"
            // curMessage = message(1) // 设置 curMessage
        }
    }


}

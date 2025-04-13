#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include "message.h"
#include "messageauthor.h"
#include"messageboardattachtype.h"
#include "randomnumbergenerator.h"
#include "messagequeue.h"
#include "myparserstatus.h"

void RegisterType()
{
    qmlRegisterType<Message>("com.mycompany.message",1,0,"Message");
    qmlRegisterType<Messagebody>("com.mycompany.messagebody",1,0,"Messagebody");
    qmlRegisterType<MessageAuthor>("com.mycompany.messageauthor",1,0,"MessageAuthor");
    qmlRegisterType<MessageBoard>("com.mycompany.messageboard",1,0,"MessageBoard");
    qmlRegisterType<RandomNumberGenerator>("com.mycompany.RandomNumberGenerator",1,0,"RandomNumberGenerator");
    qmlRegisterType<MessageQueue>("com.mycompany.messageQueue",1,0,"MessageQueue");
    qmlRegisterType<MyParserStatus>("com.mycompany.myParserStatus",1,0,"MyParserStatus");
}

int main(int argc, char *argv[])
{
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
#endif
    QGuiApplication app(argc, argv);

    /*
    QML 上下文指的是 Qt 框架中的 QQmlContext 类，它定义了 QML 引擎内的上下文环境。简单来说，它是 QML 组件运行时的一个"工作空间"，允许将 C++ 的数据和对象暴露给 QML，使它们可以在 QML 代码中直接使用。
    主要作用
    数据暴露：通过上下文属性（context properties），QQmlContext 允许开发者将 C++ 数据绑定到 QML。例如，可以通过 setContextProperty 方法设置一个 C++ 对象，让 QML 组件直接访问它。这在需要动态更新 UI 时非常有用，比如显示实时数据。
    上下文层次结构：QQmlContext 形成一个层次结构，根上下文是 QML 引擎的根上下文，每个 QML 组件实例化时会创建自己的上下文。子上下文可以继承父上下文的属性，如果子上下文设置了相同的属性，会覆盖父上下文的属性。
    对象和绑定管理：上下文持有 QML 文档中通过 ID 标识的对象，并管理这些对象的绑定。如果上下文被销毁，依赖它的表达式和子上下文会失效，但对象本身不会被销毁（除非它们是上下文对象的子对象）

    替代方案：对于大型应用，建议使用 qmlRegisterType 注册 C++ 类型或使用单例模式，以减少全局状态的污染和性能开销。
    例如，博客文章 Qt/QML: Expose C++ classes to QML and why setContextProperty is not the best idea 指出，上下文属性在性能上不如注册类型，并且可能导致名称冲突。
    */

    QQmlApplicationEngine engine;
    const QUrl url(QStringLiteral("qrc:/main.qml"));

    // 创建 Message 对象并设置父对象为 engine,确保生命周期与应用程序一致
    Message* message = new Message(&engine);
    engine.rootContext()->setContextProperty("message", message);

    RegisterType();
    QObject::connect(
        &engine,
        &QQmlApplicationEngine::objectCreated,
        &app,
        [url](QObject *obj, const QUrl &objUrl) {
            if (!obj && url == objUrl)
                QCoreApplication::exit(-1);
        },
        Qt::QueuedConnection);
    engine.load(url);

    return app.exec();
}

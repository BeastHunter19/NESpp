#include <QtGui>
#include <QtQml>

int main(int argc, char** argv)
{
    QGuiApplication app(argc, argv);
    QQmlApplicationEngine engine(QUrl("qrc:/main.qml"));
    return app.exec();
}

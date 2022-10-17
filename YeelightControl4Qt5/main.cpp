#include "MainWindow.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    a.setApplicationName("Yeelight Control");
    a.setApplicationVersion("1.0");

    MainWindow w;
    w.setWindowTitle("Yeelight Control");
    w.show();

    return a.exec();
}

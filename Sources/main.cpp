#include "mainwindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    MainWindow mainWindow;
    app.setQuitOnLastWindowClosed(true);

    mainWindow.show();
    return app.exec();
}

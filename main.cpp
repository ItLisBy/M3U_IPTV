#include "mainwindow.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    if (!DB::initDB()) {
        return 1;
    }
    w.show();
    return a.exec();
}

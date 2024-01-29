#include "mainwindow.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;

    // Init GPIO
    if (gpioInitialise() < 0)
            exit(0);

    gpioSetMode(17, PI_INPUT);
    gpioSetPullUpDown(17, PI_PUD_UP);

    gpioSetMode(27, PI_INPUT);
    gpioSetPullUpDown(27, PI_PUD_UP);

    gpioSetMode(22, PI_INPUT);
    gpioSetPullUpDown(22, PI_PUD_UP);

    w.show();
    return a.exec();
}

#include "mainwindow.h"

#include <QApplication>
#include <QCursor>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;

    // Hide the cursor
    QCursor cursor(Qt::BlankCursor);
    QApplication::setOverrideCursor(cursor);
    QApplication::changeOverrideCursor(cursor);

    // Init GPIO
    if (gpioInitialise() < 0)
            exit(0);

    gpioSetMode(17, PI_INPUT);
    gpioSetPullUpDown(17, PI_PUD_UP);

    gpioSetMode(27, PI_INPUT);
    gpioSetPullUpDown(27, PI_PUD_UP);

    gpioSetMode(22, PI_INPUT);
    gpioSetPullUpDown(22, PI_PUD_UP);

    // Show main window and execute run loop
    w.show();
    return a.exec();
}

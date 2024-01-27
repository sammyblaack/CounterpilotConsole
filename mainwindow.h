#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "confighelper.h"

#include "mainview.h"
#include "technicianview.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT
    MainView *mainView;;
    TechnicianView *technicianView;;

public:
    ConfigHelper *config;

    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    Ui::MainWindow *ui;

public slots:
    void transitionToTechnicianView();
    void transitionToMainView();
};
#endif // MAINWINDOW_H

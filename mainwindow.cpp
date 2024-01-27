#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // Instantiate config class
    config = new ConfigHelper();

    // Instantiate main view and set central
    mainView = new MainView(config, this);
    connect(mainView, &MainView::transitionToTechnicanView, this, &MainWindow::transitionToTechnicianView);
    this->setCentralWidget(mainView);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::transitionToTechnicianView() {
    technicianView = new TechnicianView(config, this);
    connect(technicianView, &TechnicianView::transitionToMainView, this, &MainWindow::transitionToMainView);
    this->setCentralWidget(technicianView);

    delete mainView;
}

void MainWindow::transitionToMainView() {
    mainView = new MainView(config, this);
    connect(mainView, &MainView::transitionToTechnicanView, this, &MainWindow::transitionToTechnicianView);
    this->setCentralWidget(mainView);

    delete technicianView;
}

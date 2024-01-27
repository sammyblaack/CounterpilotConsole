#include "mainview.h"

MainView::MainView(ConfigHelper *config, QWidget *parent)
    : QWidget{parent}
{
    // Store config class ptr
    this->config = config;

    // Set focus policy so we can receive keyboard events
    this->setFocusPolicy(Qt::StrongFocus);

    // Setup technician mode three finger salute
    buttonTimer = new QTimer(this);
    connect(buttonTimer, &QTimer::timeout, parent, [=] () { emit transitionToTechnicanView(); });


    // Setup initial frame for NDI receiver
    // frame = new QImage(QSize(720, 720), QImage::Format_RGBA8888);
    // frame->fill(Qt::black);

    // // Setup NDI worker thread
    // worker = new NDIWorker();
    // worker->moveToThread(&workerThread);

    // // Connect signals and slots to start worker and receive frames
    // connect(this, &MainView::startWorker, worker, &NDIWorker::start);
    // connect(worker, &NDIWorker::frameUpdated, this, &MainView::updateFrame);

    // // Start worker thread
    // workerThread.start();
    // emit startWorker();

    // // Start run loop for OSC and button polling
    // loopTimer = new QTimer(this);
    // connect(loopTimer, &QTimer::timeout, this, &MainView::loop);
    // loopTimer->start(0);  // As fast as possible :)
}

// void MainView::updateFrame(const QImage &frame) {
//     // Get new frame from NDI worker and repaint the widget
//     this->frame = new QImage(frame);
//     repaint();
// }

// void MainView::paintEvent(QPaintEvent *event) {
//     QPainter painter(this);

//     // Resize frame to fill and center inside widget viewport
//     QSize size = frame->size().scaled(width(), height(), Qt::KeepAspectRatioByExpanding);
//     int x_offset = -(size.width()-width())/2;
//     int y_offset = -(size.height()-height())/2;

//     // Draw image inside widget viewport
//     painter.drawImage(QRect(x_offset, y_offset, size.width(), size.height()), *frame);
// }


// Input methods
// void MainView::loop() {
//     // This is a run loop for polling button presses and triggering OSC
// }

void MainView::keyPressEvent(QKeyEvent *event) {
    if (event->isAutoRepeat()) return;

    switch (event->nativeScanCode()) {
    case 331: {
        // Button H / Left
        hButtonDown = true;
        break;
    }

    case 336: {
        // Button C / Down
        cButtonDown = true;
        break;
    }

    case 333: {
        // Button B / Right
        bButtonDown = true;
        break;
    }
    }

    if (hButtonDown && cButtonDown && bButtonDown) {
        // Dont restart the timer if its already running
        if (timerStarted == true) return;

        buttonTimer->start(10000);
        this->timerStarted = true;
    }
}

void MainView::keyReleaseEvent(QKeyEvent *event) {
    if (event->isAutoRepeat()) return;

    switch (event->nativeScanCode()) {
    case 331: {
        // Button H / Left
        hButtonDown = false;

        if (timerStarted) {
            buttonTimer->stop();
            timerStarted = false;
        }

        break;
    }

    case 336: {
        // Button C / Down
        cButtonDown = false;

        if (timerStarted) {
            buttonTimer->stop();
            timerStarted = false;
        }

        break;
    }

    case 333: {
        // Button B / Right
        bButtonDown = false;

        if (timerStarted) {
            buttonTimer->stop();
            timerStarted = false;
        }

        break;
    }
    }
}

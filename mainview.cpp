#include "mainview.h"

MainView::MainView(ConfigHelper *config, QWidget *parent)
    : QWidget{parent}
{
    // Store config class ptr
    this->config = config;



    ndiSource = "";
    if (config->getParameter("ndi_stream").isString())
        ndiSource = config->getParameter("ndi_stream").toString();

    oscHostAddress = "";
    if (config->getParameter("osc_host").isString())
        oscHostAddress = config->getParameter("osc_host").toString();

    oscHostPort = 0;
    if (config->getParameter("osc_host_port").isDouble())
        oscHostPort = config->getParameter("osc_host_port").toInt();

    oscPort = 0;
    if (config->getParameter("osc_port").isDouble())
        oscPort = config->getParameter("osc_port").toInt();

    oscButtonHPath = "";
    if (config->getParameter("button_h_path").isString())
        oscButtonHPath = config->getParameter("button_h_path").toString();

    oscButtonCPath = "";
    if (config->getParameter("button_c_path").isString())
        oscButtonCPath = config->getParameter("button_c_path").toString();

    oscButtonBPath = "";
    if (config->getParameter("button_b_path").isString())
        oscButtonBPath = config->getParameter("button_b_path").toString();





    // Set focus policy so we can receive keyboard events
    this->setFocusPolicy(Qt::StrongFocus);

    // Setup technician mode three finger salute
    buttonTimer = new QTimer(this);
    connect(buttonTimer, &QTimer::timeout, parent, [=] () { emit transitionToTechnicanView(); });


    // Setup initial frame for NDI receiver
    frame = new QImage(QSize(720, 720), QImage::Format_RGBA8888);
    frame->fill(Qt::black);

    // Setup NDI worker thread
    workerThread = new QThread();
    worker = new NDIWorker();
    worker->moveToThread(workerThread);

    // Connect signals and slots to start worker and receive frames
    connect(workerThread, &QThread::started, worker, &NDIWorker::start);
    connect(worker, &NDIWorker::finished, workerThread, &QThread::quit);
    connect(worker, &NDIWorker::finished, workerThread, &QThread::deleteLater);
    connect(workerThread, &QThread::finished, workerThread, &QThread::deleteLater);

//    connect(this, &MainView::startWorker, worker, &NDIWorker::start);
//    connect(this, &MainView::stopWorker, worker, &NDIWorker::stop);

    connect(worker, &NDIWorker::frameUpdated, this, &MainView::updateFrame);

    // Start worker thread
    workerThread->start(ndiSource);
//    emit startWorker();


    // Start run loop for OSC and button polling
    loopTimer = new QTimer(this);
    connect(loopTimer, &QTimer::timeout, this, &MainView::loop);
    loopTimer->start(0);  // As fast as possible :)

    // Start OSC
    osc = new GoodOSC(QHostAddress::AnyIPv4, oscPort);
}

 MainView::~MainView() {
    buttonTimer->stop();
    delete buttonTimer;

    // NDI

    worker->stop();
    workerThread->quit();
    workerThread->wait();

    delete workerThread;
    delete worker;
    delete frame;

    // OSC & Buttons
    loopTimer->stop();
    delete loopTimer;
 }

 void MainView::updateFrame(const QImage &frame) {
     // Get new frame from NDI worker and repaint the widget
     this->frame = new QImage(frame);
     repaint();
 }

 void MainView::paintEvent(QPaintEvent *event) {
     QPainter painter(this);

     // Resize frame to fill and center inside widget viewport
     QSize size = frame->size().scaled(width(), height(), Qt::KeepAspectRatioByExpanding);
     int x_offset = -(size.width()-width())/2;
     int y_offset = -(size.height()-height())/2;

     // Draw image inside widget viewport
     painter.drawImage(QRect(x_offset, y_offset, size.width(), size.height()), *frame);
 }


// Input methods
 void MainView::loop() {
    // This is a run loop for polling button presses and triggering OSC
    static uint32_t hLastTick = 0;
    static uint32_t cLastTick = 0;
    static uint32_t bLastTick = 0;
    static int hState = 1;
    static int cState = 1;
    static int bState = 1;



    int hNewState = gpioRead(17);
    int cNewState = gpioRead(22);
    int bNewState = gpioRead(27);

    uint32_t tick = gpioTick();

    // Handle H Button
    if (hNewState != hState) {
        if (tick > hLastTick + 200000) { // uS
            hLastTick = tick;

            hState = hNewState;
            if (hState == false)
                handleButtonHPressed();
            else
                handleButtonHReleased();
        }
    }

    // Handle C Button
    if (cNewState != cState) {
        if (tick > cLastTick + 200000) { // uS
            cLastTick = tick;

            cState = cNewState;
            if (cState == false)
                handleButtonCPressed();
            else
                handleButtonCReleased();
        }
    }

    // Handle B Button
    if (bNewState != bState) {
        if (tick > bLastTick + 200000) { // uS
            bLastTick = tick;

            bState = bNewState;
            if (bState == false)
                handleButtonBPressed();
            else
                handleButtonBReleased();
        }
    }
 }

void MainView::keyPressEvent(QKeyEvent *event) {
    if (event->isAutoRepeat()) return;

    switch (event->nativeScanCode()) {
    case 331: {
        // Button H / Left
        // hButtonDown = true;
        handleButtonHPressed();
        break;
    }

    case 336: {
        // Button C / Down
        // cButtonDown = true;
        handleButtonCPressed();
        break;
    }

    case 333: {
        // Button B / Right
        // bButtonDown = true;
        handleButtonBPressed();
        break;
    }
    }

    // if (hButtonDown && cButtonDown && bButtonDown) {
    //     // Dont restart the timer if its already running
    //     if (timerStarted == true) return;

    //     buttonTimer->start(10000);
    //     this->timerStarted = true;
    // }
}

void MainView::keyReleaseEvent(QKeyEvent *event) {
    if (event->isAutoRepeat()) return;

    switch (event->nativeScanCode()) {
    case 331: {
        // Button H / Left
        handleButtonHReleased();
        break;
    }

    case 336: {
        // Button C / Down
        handleButtonCReleased();
        break;
    }

    case 333: {
        // Button B / Right
        handleButtonBReleased();
        break;
    }
    }
}

void MainView::handleButtonHPressed() {
    GoodOSCMessage *msg = new GoodOSCMessage(oscButtonHPath.toUtf8());
    osc->send(QHostAddress(oscHostAddress), oscHostPort, msg);
    delete msg;

    hButtonDown = true;
    globalButtonPressed();
}

void MainView::handleButtonCPressed() {
    GoodOSCMessage *msg = new GoodOSCMessage(oscButtonCPath.toUtf8());
    osc->send(QHostAddress(oscHostAddress), oscHostPort, msg);
    delete msg;

    cButtonDown = true;
    globalButtonPressed();
}

void MainView::handleButtonBPressed() {
    GoodOSCMessage *msg = new GoodOSCMessage(oscButtonBPath.toUtf8());
    osc->send(QHostAddress(oscHostAddress), oscHostPort, msg);
    delete msg;

    bButtonDown = true;
    globalButtonPressed();
}

void MainView::globalButtonPressed() {
    if (hButtonDown && cButtonDown && bButtonDown) {
        // Dont restart the timer if its already running
        if (timerStarted == true) return;

        buttonTimer->start(10000);
        this->timerStarted = true;
    }
}

void MainView::handleButtonHReleased() {
    hButtonDown = false;
    globalButtonReleased();
}

void MainView::handleButtonCReleased() {
    cButtonDown = false;
    globalButtonReleased();
}

void MainView::handleButtonBReleased() {
    bButtonDown = false;
    globalButtonReleased();
}

void MainView::globalButtonReleased() {
    if (timerStarted) {
        buttonTimer->stop();
        timerStarted = false;
    }
}

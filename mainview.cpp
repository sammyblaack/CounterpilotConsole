#include "mainview.h"

MainView::MainView(ConfigHelper *config, QWidget *parent)
    : QWidget{parent}
{

    QPalette pal = QPalette();
    pal.setColor(QPalette::Window, Qt::black);
    this->setAutoFillBackground(true);
    this->setPalette(pal);
    

    // Store config class ptr
    this->config = config;



    if (config->getParameter("ndi_stream").isString())
        this->sourceName = config->getParameter("ndi_stream").toString();

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


    // Start OSC
    osc = new GoodOSC(QHostAddress::AnyIPv4, oscPort);


    // Init NDI
    if (!NDIlib_initialize())
        exit(0);

    // Create finder instance
    pNDI_find = NDIlib_find_create_v2();
    if (!pNDI_find)
        exit(0);

    searchTimer = new QTimer(this);
    connect(searchTimer, &QTimer::timeout, this, &MainView::searchSource);
    searchTimer->start(0);


    // Start run loop for button polling and NDI
    loopTimer = new QTimer(this);
    connect(loopTimer, &QTimer::timeout, this, &MainView::loop);
    loopTimer->start(0);  // As fast as possible :)
}

 MainView::~MainView() {
    // NDI
    searchTimer->stop();
    delete searchTimer;

    if (pNDI_recv != NULL)
        NDIlib_recv_destroy(pNDI_recv);
    NDIlib_destroy();



    // OSC & Buttons
    loopTimer->stop();
    delete loopTimer;

    buttonTimer->stop();
    delete buttonTimer;
 }


void MainView::searchSource() {
    // Find sources
    const NDIlib_source_t *p_sources = NULL;

    if (source == -1) {
        NDIlib_find_wait_for_sources(pNDI_find, 1000);
        p_sources = NDIlib_find_get_current_sources(pNDI_find, &no_sources);

        for (uint32_t i = 0; i < no_sources; i++) {
            if (strcmp(p_sources[i].p_ndi_name, this->sourceName.toStdString().c_str()) == 0) {
                source = i;

                // Create receiver
                NDIlib_recv_create_v3_t opts;
                opts.color_format = NDIlib_recv_color_format_RGBX_RGBA;

                pNDI_recv = NDIlib_recv_create_v3(&opts);
                if (!pNDI_recv)
                    exit(0);

                // Connect receiver to video source and destroy finder
                NDIlib_recv_connect(pNDI_recv, p_sources + source);
                NDIlib_find_destroy(pNDI_find);

                // Stop timer
                searchTimer->stop();
                return;
            }
        }
    }
}

 void MainView::paintEvent(QPaintEvent *event) {
     QPainter painter(this);
    

     // Resize frame to fill and center inside widget viewport
     QSize size = frame->size().scaled(width(), height(), Qt::KeepAspectRatioByExpanding);
     int x_offset = -(size.width()-width())/2;
     int y_offset = -(size.height()-height())/2;

     painter.rotate(180);
     painter.translate(-size.width(), -size.height());

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
        if (tick - hLastTick > 200000) { // uS
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
        if (tick - cLastTick > 200000) { // uS
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
        if (tick - bLastTick > 200000) { // uS
            bLastTick = tick;

            bState = bNewState;
            if (bState == false)
                handleButtonBPressed();
            else
                handleButtonBReleased();
        }
    }

    // NDI
    if (source != -1) {
        NDIlib_video_frame_v2_t video_frame;
        NDIlib_audio_frame_v2_t audio_frame;

        switch (NDIlib_recv_capture_v2(pNDI_recv, &video_frame, &audio_frame, nullptr, 5000)) {

        case NDIlib_frame_type_none:
            break;

        case NDIlib_frame_type_video:
            delete this->frame;
            this->frame = new QImage(video_frame.p_data, video_frame.xres, video_frame.yres, QImage::Format_RGBA8888);
            repaint();

            NDIlib_recv_free_video_v2(pNDI_recv, &video_frame);
            break;

        case NDIlib_frame_type_audio:
            NDIlib_recv_free_audio_v2(pNDI_recv, &audio_frame);

        case NDIlib_frame_type_metadata:
        case NDIlib_frame_type_error:
        case NDIlib_frame_type_status_change:
        case NDIlib_frame_type_max:
            break;
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

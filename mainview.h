#ifndef MAINVIEW_H
#define MAINVIEW_H

#include <QWidget>
#include <QKeyEvent>
#include <QTimer>

#include "confighelper.h"

#include <Processing.NDI.Lib.h>
#include "goodosc.h"
#include <QtNetwork/QHostAddress>



#include <QThread>
#include <QPainter>

#include "pigpio.h"

#include <Processing.NDI.Lib.h>

class MainView : public QWidget
{
    Q_OBJECT
    ConfigHelper *config;

    // Button press logic
    void keyPressEvent(QKeyEvent *event) override;
    void keyReleaseEvent(QKeyEvent *event) override;
    bool hButtonDown = false;
    bool cButtonDown = false;
    bool bButtonDown = false;

    QTimer *buttonTimer;
    bool timerStarted = false;

    // NDI
    QImage *frame;
    NDIlib_find_instance_t pNDI_find = NULL;
    NDIlib_recv_instance_t pNDI_recv = NULL;
    
    int source = -1;
    uint32_t no_sources = 0;
    QString sourceName = "";

    QString ndiSource;
    QTimer *searchTimer;

    // OSC & Buttons
    GoodOSC *osc;
    QString oscHostAddress;
    int oscHostPort;
    int oscPort;

    QString oscButtonHPath;
    QString oscButtonCPath;
    QString oscButtonBPath;

    QTimer *loopTimer;
    void handleButtonHPressed();
    void handleButtonCPressed();
    void handleButtonBPressed();
    void globalButtonPressed();

    void handleButtonHReleased();
    void handleButtonCReleased();
    void handleButtonBReleased();
    void globalButtonReleased();



public:
    explicit MainView(ConfigHelper *config, QWidget *parent = nullptr);
    ~MainView();

 protected:
     void paintEvent(QPaintEvent *event) override;

signals:
    void transitionToTechnicanView();
    void startWorker();

public slots:
     void loop();
     void searchSource();
};

#endif // MAINVIEW_H

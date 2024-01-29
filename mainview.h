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
#include "ndiworker.h"

#include "pigpio.h"

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
    QThread *workerThread;
    NDIWorker *worker;
    QImage *frame;

    QString ndiSource;

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
     void updateFrame(const QImage &frame);
     void loop();
};

#endif // MAINVIEW_H

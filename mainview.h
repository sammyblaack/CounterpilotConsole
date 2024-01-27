#ifndef MAINVIEW_H
#define MAINVIEW_H

#include <QWidget>
#include <QKeyEvent>
#include <QTimer>

#include "confighelper.h"

#include <Processing.NDI.Lib.h>


// #include <QThread>
// #include <QPainter>
// #include "ndiworker.h"

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

    // // NDI
    // // NDIWorker *worker;
    // QThread workerThread;
    // QImage *frame;

    // // OSC & Buttons
    // QTimer *loopTimer;


public:
    explicit MainView(ConfigHelper *config, QWidget *parent = nullptr);

// protected:
//     void paintEvent(QPaintEvent *event) override;

signals:
    void transitionToTechnicanView();
    // void startWorker();

public slots:
    // void updateFrame(const QImage &frame);
    // void loop();
};

#endif // MAINVIEW_H

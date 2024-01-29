#ifndef TECHNICIANVIEW_H
#define TECHNICIANVIEW_H

#include <QWidget>
#include <QKeyEvent>

#include "confighelper.h"
#include <string.h>
#include <QTimer>

#include "pigpio.h"

namespace Ui {
class TechnicianView;
}

class TechnicianView : public QWidget
{
    Q_OBJECT
    ConfigHelper *config;

    void keyPressEvent(QKeyEvent *event) override;
    void populate();

    QTimer *loopTimer;
    void handleButtonHPressed();
    void handleButtonCPressed();
    void handleButtonBPressed();

public:
    explicit TechnicianView(ConfigHelper *config, QWidget *parent = nullptr);
    ~TechnicianView();

signals:
    void transitionToMainView();

public slots:
    void loop();

private:
    Ui::TechnicianView *ui;
};

#endif // TECHNICIANVIEW_H

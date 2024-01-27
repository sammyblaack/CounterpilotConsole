#ifndef TECHNICIANVIEW_H
#define TECHNICIANVIEW_H

#include <QWidget>
#include <QKeyEvent>

#include "confighelper.h"

namespace Ui {
class TechnicianView;
}

class TechnicianView : public QWidget
{
    Q_OBJECT
    ConfigHelper *config;

    void keyPressEvent(QKeyEvent *event) override;
    void populate();

public:
    explicit TechnicianView(ConfigHelper *config, QWidget *parent = nullptr);
    ~TechnicianView();

signals:
    void transitionToMainView();

private:
    Ui::TechnicianView *ui;
};

#endif // TECHNICIANVIEW_H

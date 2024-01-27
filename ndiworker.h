#ifndef NDIWORKER_H
#define NDIWORKER_H

#include <QObject>
#include <QImage>


#include <QDebug>
#include <Processing.NDI.Lib.h>


class NDIWorker : public QObject
{
    Q_OBJECT
    NDIlib_recv_instance_t pNDI_recv;

signals:
    void frameUpdated(const QImage &frame);

public slots:
    void start();
};

#endif // NDIWORKER_H

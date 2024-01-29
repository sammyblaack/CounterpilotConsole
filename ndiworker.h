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
    bool run = false;

signals:
    void frameUpdated(const QImage &frame);
    void finished();

public slots:
    void start(QString sourceName);
    void stop();
};

#endif // NDIWORKER_H

#ifndef ZRTPMESSAGE_H
#define ZRTPMESSAGE_H

#include <QDebug>
#include <QThread>
#include <QMutex>

class ZrtpMessage : public QThread
{
    Q_OBJECT
public:
    explicit ZrtpMessage();

private:

    void run();
};

#endif // ZRTPMESSAGE_H

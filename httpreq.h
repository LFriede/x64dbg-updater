#ifndef HTTPREQ_H
#define HTTPREQ_H

#include <QThread>
#include "Windows.h"

class HttpReq : public QThread
{
    Q_OBJECT
    void run() override;
signals:
    void resultReady(const unsigned long status, const QString &content);
public:
    explicit HttpReq(QObject *parent = nullptr, const char *url = nullptr);
private:
    QString url;
};

#endif // HTTPREQ_H

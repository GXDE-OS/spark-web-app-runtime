#ifndef WEBENGINEURLREQUESTINTERCEPTOR_H
#define WEBENGINEURLREQUESTINTERCEPTOR_H

#include <QWebEngineUrlRequestInterceptor>

class WebEngineUrlRequestInterceptor : public QWebEngineUrlRequestInterceptor
{
    Q_OBJECT

public:
    explicit WebEngineUrlRequestInterceptor(QObject *parent = nullptr);
    void interceptRequest(QWebEngineUrlRequestInfo &info) override;
};

#endif // WEBENGINEURLREQUESTINTERCEPTOR_H

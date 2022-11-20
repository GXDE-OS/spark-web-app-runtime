#ifndef WEBENGINEVIEW_H
#define WEBENGINEVIEW_H

#include <QWebEngineView>

// class WebEngineUrlRequestInterceptor;
class WebEngineView : public QWebEngineView
{
    Q_OBJECT

public:
    explicit WebEngineView(QWidget *parent = nullptr);

private:
    //    WebEngineUrlRequestInterceptor *interceptor = nullptr;
};

#endif // WEBENGINEVIEW_H

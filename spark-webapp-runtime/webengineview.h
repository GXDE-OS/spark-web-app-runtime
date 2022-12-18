#ifndef WEBENGINEVIEW_H
#define WEBENGINEVIEW_H

#include <QWebEngineView>
#include <QWebEngineNotification>

// class WebEngineUrlRequestInterceptor;
class WebEngineView : public QWebEngineView
{
    Q_OBJECT

public:
    explicit WebEngineView(QWidget *parent = nullptr);

    static void handleChromiumFlags();
    static void present(std::unique_ptr<QWebEngineNotification> &newNotification);

private:
    //    WebEngineUrlRequestInterceptor *interceptor = nullptr;
};

#endif // WEBENGINEVIEW_H

#ifndef WEBENGINEVIEW_H
#define WEBENGINEVIEW_H

#include <QWebEngineView>
#if (QT_VERSION >= QT_VERSION_CHECK(5, 13, 0))
#include <QWebEngineNotification>
#endif

// class WebEngineUrlRequestInterceptor;
class WebEngineView : public QWebEngineView
{
    Q_OBJECT

public:
    explicit WebEngineView(QWidget *parent = nullptr);

    static void handleChromiumFlags();
#if (QT_VERSION >= QT_VERSION_CHECK(5, 13, 0))
    static void present(std::unique_ptr<QWebEngineNotification> &newNotification);
#endif

private:
    //    WebEngineUrlRequestInterceptor *interceptor = nullptr;
};

#endif // WEBENGINEVIEW_H

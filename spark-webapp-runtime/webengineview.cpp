#include "webengineview.h"
//#include "webengineurlrequestinterceptor.h"

#include <QWebEngineSettings>
#include <QWebEngineProfile>
#include <QLocale>

WebEngineView::WebEngineView(QWidget *parent)
    : QWebEngineView(parent)
//    , interceptor(new WebEngineUrlRequestInterceptor(this))
{
    //    page()->profile()->setHttpUserAgent(page()->profile()->httpUserAgent() + " MediaFeature/prefers-color-scheme:dark");

    connect(this, &WebEngineView::urlChanged, this, [=]() {
        //        page()->setUrlRequestInterceptor(interceptor);
        //        page()->settings()->setAttribute(QWebEngineSettings::WebAttribute::LocalContentCanAccessRemoteUrls, true);
        page()->profile()->setHttpAcceptLanguage(QLocale::system().name());
        //        qInfo() << "User Agent:" << page()->profile()->httpUserAgent();
    });
}

#include "webengineurlrequestinterceptor.h"

#include <QLocale>

WebEngineUrlRequestInterceptor::WebEngineUrlRequestInterceptor(QObject *parent)
    : QWebEngineUrlRequestInterceptor(parent)
{
}

void WebEngineUrlRequestInterceptor::interceptRequest(QWebEngineUrlRequestInfo &info)
{
    info.setHttpHeader("Accept-Language", QLocale::system().name().toUtf8());
}

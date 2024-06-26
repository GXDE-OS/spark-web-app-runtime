#include "webenginepage.h"

#include <QFile>
#include <QWebEngineScript>
#include <QWebEngineScriptCollection>
#include <QDesktopServices>

WebEnginePage::WebEnginePage(QObject *parent)
    : QWebEnginePage(parent)
{
    initScrollBarStyle();

    connect(this, &QWebEnginePage::featurePermissionRequested, [&](const QUrl &origin, QWebEnginePage::Feature feature) {
        if (feature != QWebEnginePage::Notifications) {
            return;
        }

        setFeaturePermission(origin, feature, QWebEnginePage::PermissionGrantedByUser);
    });
}

WebEnginePage::~WebEnginePage()
{
}

void WebEnginePage::setUrl(const QUrl &url)
{
    if (m_currentUrl == url) {
        return;
    }

    m_currentUrl = url;
    QWebEnginePage::setUrl(url);
}

QWebEnginePage *WebEnginePage::createWindow(QWebEnginePage::WebWindowType type)
{
    qDebug() << Q_FUNC_INFO << type;

    WebEnginePage *page = new WebEnginePage(parent());
    connect(page, &WebEnginePage::urlChanged, this, &WebEnginePage::slotUrlChanged);
    return page;
}

void WebEnginePage::initScrollBarStyle()
{
    //自定义浏览器滚动条样式
    QFile file(":/css/webkit-scrollbar.css");
    if (file.open(QFile::ReadOnly)) {
        QString scrollbarStyleJS = QString("(function() {"
                                           "    css = document.createElement('style');"
                                           "    css.type = 'text/css';"
                                           "    css.id = '%1';"
                                           "    document.head.appendChild(css);"
                                           "    css.innerText = '%2';"
                                           "})()\n")
                                       .arg("scrollbarStyle")
                                       .arg(QString::fromUtf8(file.readAll()).simplified());
        file.close();

        QWebEngineScript script;
        script.setWorldId(QWebEngineScript::MainWorld);
        script.setSourceCode(scrollbarStyleJS);
        scripts().insert(script);
        runJavaScript(scrollbarStyleJS, QWebEngineScript::ApplicationWorld);
    }
}

void WebEnginePage::slotUrlChanged(const QUrl &url)
{
    if (m_currentUrl == url) {
        sender()->deleteLater();
        return;
    }

    qDebug() << Q_FUNC_INFO << m_currentUrl << url;

    QDesktopServices::openUrl(url);
    sender()->deleteLater();
}

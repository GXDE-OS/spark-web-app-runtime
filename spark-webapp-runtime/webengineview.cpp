#include "webengineview.h"
//#include "webengineurlrequestinterceptor.h"

#include <DGuiApplicationHelper>

#include <QWebEngineSettings>
#include <QWebEngineProfile>
#include <QLocale>

DGUI_USE_NAMESPACE

WebEngineView::WebEngineView(QWidget *parent)
    : QWebEngineView(parent)
//    , interceptor(new WebEngineUrlRequestInterceptor(this))
{
    connect(this, &WebEngineView::urlChanged, this, [=]() {
        //        page()->setUrlRequestInterceptor(interceptor);
        //        page()->settings()->setAttribute(QWebEngineSettings::WebAttribute::LocalContentCanAccessRemoteUrls, true);
        page()->profile()->setHttpAcceptLanguage(QLocale::system().name());
    });
}

void WebEngineView::handleChromiumFlags()
{
    DGuiApplicationHelper::ColorType themeType = DGuiApplicationHelper::instance()->themeType();

    QString env = qgetenv("QTWEBENGINE_CHROMIUM_FLAGS");
    QStringList flags = env.split(" ", QString::SkipEmptyParts);

    /**
     * --blink-settings=preferredColorScheme=0 强制 prefers-color-scheme=dark (>= 5.14)
     * --blink-settings=darkModeEnabled=true,darkModeInversionAlgorithm=4 强制反转网页颜色 (>= 5.14 && < 5.15)
     * --blink-settings=forceDarkModeEnabled=true,darkModeInversionAlgorithm=4 强制反转网页颜色 (>= 5.15)
     * --force-dark-mode 强制 prefers-color-scheme=dark (>= 5.14)
     */
    foreach (const QString &flag, flags) {
        if (flag == "--force-dark-mode") {
            flags.removeAll(flag);
        }
        if (flag.startsWith("--blink-settings")) {
            flags.removeAll(flag);
        }
    }

    if (themeType == DGuiApplicationHelper::DarkType) {
        flags.append("--blink-settings=preferredColorScheme=0");
    } else {
        flags.append("--blink-settings=preferredColorScheme=1");
    }
    qputenv("QTWEBENGINE_CHROMIUM_FLAGS", flags.join(" ").toUtf8());

    qDebug() << Q_FUNC_INFO << "QTWEBENGINE_CHROMIUM_FLAGS=" + qgetenv("QTWEBENGINE_CHROMIUM_FLAGS");
}

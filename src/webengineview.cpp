#include "webengineview.h"
//#include "webengineurlrequestinterceptor.h"
#include "application.h"

#include <DGuiApplicationHelper>
#include <DNotifySender>

#include <QWebEngineSettings>
#include <QWebEngineProfile>
#include <QLocale>
#include <QFileInfo>

DGUI_USE_NAMESPACE
DCORE_USE_NAMESPACE

WebEngineView::WebEngineView(QWidget *parent)
    : QWebEngineView(parent)
//    , interceptor(new WebEngineUrlRequestInterceptor(this))
{
    connect(this, &WebEngineView::urlChanged, this, [=]() {
        //        page()->setUrlRequestInterceptor(interceptor);
        //        page()->settings()->setAttribute(QWebEngineSettings::WebAttribute::LocalContentCanAccessRemoteUrls, true);
        page()->profile()->setHttpAcceptLanguage(QLocale::system().name());
#if (QT_VERSION >= QT_VERSION_CHECK(5, 13, 0))
        page()->profile()->setNotificationPresenter([&](std::unique_ptr<QWebEngineNotification> notification) {
            WebEngineView::present(notification);
        });
#endif
    });
}

void WebEngineView::handleChromiumFlags()
{
    DGuiApplicationHelper::ColorType themeType = DGuiApplicationHelper::instance()->themeType();

    QString env = qgetenv("QTWEBENGINE_CHROMIUM_FLAGS");
    QStringList flags = env.split(" ", Qt::SkipEmptyParts);

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

#if (QT_VERSION >= QT_VERSION_CHECK(5, 13, 0))
void WebEngineView::present(std::unique_ptr<QWebEngineNotification> &newNotification)
{
    qDebug() << Q_FUNC_INFO << "New notification received:" << newNotification->title() << newNotification->message();

    QImage image = newNotification->icon();
    QString tmpDir = qobject_cast<Application *>(qApp)->mainWindow()->tmpDir();
    QString appIcon = tmpDir + "/" + "icon.png";
    if (QFileInfo::exists(appIcon)) {
        QFile::remove(appIcon);
    }
    image.save(appIcon, "PNG");

    QString summary = newNotification->title();
    QString appName = qobject_cast<Application *>(qApp)->mainWindow()->title();
    QString appBody = newNotification->message();
    quint32 replaceId = 0;
    int timeOut = 3000;
    QStringList actions = QStringList() << "view" << QObject::tr("View");

    QString launchCmd = qobject_cast<Application *>(qApp)->launchParams().join(",");
    qDebug() << launchCmd;
    QVariantMap hints;
    hints.insert("x-deepin-action-view", launchCmd);

    DUtil::DNotifySender(summary)
        .appName(appName)
        .appIcon(appIcon)
        .appBody(appBody)
        .replaceId(replaceId)
        .timeOut(timeOut)
        .actions(actions)
        .hints(hints)
        .call();
}
#endif

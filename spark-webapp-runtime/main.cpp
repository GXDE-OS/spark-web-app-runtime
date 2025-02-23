/**
 * Spark WebApp Runtime
 * 星火网页应用运行环境
 */
#include "application.h"
#include "mainwindow.h"
#include "webengineview.h"
#include "httpd.h"

#include <DSysInfo>
#include <DApplicationSettings>

#include <QCommandLineParser>
#include <QCommandLineOption>
#include <QFileInfo>
#include <QSettings>

#include <unistd.h>

int main(int argc, char *argv[])
{
    // 龙芯机器配置,使得 DApplication 能正确加载 QTWEBENGINE
    qputenv("DTK_FORCE_RASTER_WIDGETS", "FALSE");

    // 开启 HiDPI 缩放支持
    DApplication::setAttribute(Qt::AA_EnableHighDpiScaling);

    // 强制使用 DTK 平台插件
    int fakeArgc = argc + 2;
    QVector<char *> fakeArgv(fakeArgc);
    fakeArgv[0] = argv[0];
    fakeArgv[1] = const_cast<char *>("-platformtheme");
    fakeArgv[2] = const_cast<char *>("deepin");
    for (int i = 1; i < argc; i++) {
        fakeArgv[i + 2] = argv[i];
    }
    Application a(fakeArgc, fakeArgv.data());

    // 解析命令行启动参数
    QCommandLineParser parser;

    parser.setApplicationDescription(QObject::tr("Description: %1").arg(DEFAULT_TITLE));
    parser.addHelpOption();
    parser.addVersionOption();

    QCommandLineOption optParser(QStringList() << "p"
                                               << "parser",
                                 QObject::tr("Enable CommandLineParser. Default is false."));
    parser.addOption(optParser);

    QCommandLineOption optTitle(QStringList() << "t"
                                              << "title",
                                QObject::tr("The Title of Application. Default is %1.").arg(DEFAULT_TITLE),
                                "title",
                                DEFAULT_TITLE);
    parser.addOption(optTitle);

    QCommandLineOption optUrl(QStringList() << "u"
                                            << "url",
                              QObject::tr("The target URL. Default is Blank."),
                              "url",
                              DEFAULT_URL);
    parser.addOption(optUrl);

    QCommandLineOption optWidth(QStringList() << "w"
                                              << "width",
                                QObject::tr("The Width of Application. Default is %1.").arg(DEFAULT_WIDTH),
                                "width",
                                QString::number(DEFAULT_WIDTH));
    parser.addOption(optWidth);

    QCommandLineOption optHeight(QStringList() << "H"
                                               << "height",
                                 QObject::tr("The Height of Application. Default is %1.").arg(DEFAULT_HEIGHT),
                                 "height",
                                 QString::number(DEFAULT_HEIGHT));
    parser.addOption(optHeight);

    QCommandLineOption optTray(QStringList() << "T"
                                             << "tray",
                               QObject::tr("Enable Tray Icon. Default is false."));
    parser.addOption(optTray);

    QCommandLineOption optFullScreen("full-screen",
                                     QObject::tr("Run in Fullscreen Mode. Default is false."));
    parser.addOption(optFullScreen);

    QCommandLineOption optFixSize("fix-size",
                                  QObject::tr("Fix Window Size. Default is false."));
    parser.addOption(optFixSize);

    QCommandLineOption optHideButtons("hide-buttons",
                                      QObject::tr("Hide Control Buttons. Default is false."));
    parser.addOption(optHideButtons);

    QCommandLineOption optIcon(QStringList() << "i"
                                             << "ico",
                               QObject::tr("The ICON of Application."),
                               "ico",
                               DEFAULT_ICON);
    parser.addOption(optIcon);

    QCommandLineOption optDesc(QStringList() << "d"
                                             << "desc",
                               QObject::tr("The Description of Application."),
                               "desc",
                               DEFAULT_DESC);
    parser.addOption(optDesc);

    QCommandLineOption optCfgFile(QStringList() << "c"
                                                << "cfg",
                                  QObject::tr("The Configuration file of Application."),
                                  "cfg",
                                  DEFAULT_CFG);
    parser.addOption(optCfgFile);

    QCommandLineOption optRootPath(QStringList() << "r"
                                                 << "root",
                                   QObject::tr("The root path of the program web service."),
                                   "root",
                                   DEFAULT_ROOT);
    parser.addOption(optRootPath);

    QCommandLineOption optPort(QStringList() << "P"
                                             << "port",
                               QObject::tr("The port number of the program web service."),
                               "port",
                               QString::number(DEFAULT_PORT));
    parser.addOption(optPort);

    QCommandLineOption useGPU(QStringList() << "G"
                                            << "GPU",
                              QObject::tr("To use GPU instead of CPU to decoding. Default True."),
                              "GPU",
                              QString::number(DEFAULT_GPU));
    parser.addOption(useGPU);

#if SSL_SERVER
    QCommandLineOption optSSLPort(QStringList() << "s"
                                                << "sslport",
                                  QObject::tr("The ssl port number of the program web service."),
                                  "sslport",
                                  DEFAULT_PORT);
    parser.addOption(optSSLPort);
#endif

    parser.process(a);

    QString szTitle = DEFAULT_TITLE;
    QString szUrl = DEFAULT_URL;
    int width = DEFAULT_WIDTH;
    int height = DEFAULT_HEIGHT;
    bool tray = false;
    bool fullScreen = false;
    bool fixSize = false;
    bool hideButtons = false;
    QString szIcon = DEFAULT_ICON;
    QString szDesc = DEFAULT_DESC;
    QString szRootPath = DEFAULT_ROOT;
    quint16 u16Port = DEFAULT_PORT;
    bool toUseGPU = DEFAULT_GPU;
#if SSL_SERVER
    quint16 u16sslPort = 0;
#endif
    QString szDefaultDesc = QObject::tr("Presented By Spark developers # HadesStudio");

    // 解析可能存在的配置文件
    QString szCfgFile = DEFAULT_CFG;
    if (parser.isSet(optCfgFile)) {
        szCfgFile = parser.value(optCfgFile);
        if (!szCfgFile.isEmpty()) {
            if (QFileInfo(szCfgFile).exists()) {
                QSettings settings(szCfgFile, QSettings::IniFormat);
                szTitle = settings.value("SparkWebAppRuntime/Title", DEFAULT_TITLE).toString();
                szUrl = settings.value("SparkWebAppRuntime/URL", DEFAULT_TITLE).toString();
                width = settings.value("SparkWebAppRuntime/Width", DEFAULT_WIDTH).toInt();
                height = settings.value("SparkWebAppRuntime/Height", DEFAULT_HEIGHT).toInt();
                tray = settings.value("SparkWebAppRunTime/Tray", false).toBool();
                fullScreen = settings.value("SparkWebAppRunTime/FullScreen", false).toBool();
                fixSize = settings.value("SparkWebAppRunTime/FixSize", false).toBool();
                hideButtons = settings.value("SparkWebAppRunTime/HideButtons", false).toBool();
                szIcon = settings.value("SparkWebAppRuntime/Ico", DEFAULT_ICON).toString();
                szDesc = QString("%1<br/><br/>%2")
                             .arg(settings.value("SparkWebAppRuntime/Desc", QString()).toString())
                             .arg(szDefaultDesc);
                szRootPath = settings.value("SparkWebAppRuntime/RootPath", QString()).toString();
                u16Port = static_cast<quint16>(settings.value("SparkWebAppRuntime/Port", 0).toUInt());
#if SSL_SERVER
                u16sslPort = settings.value("SparkWebAppRuntime/SSLPort", 0).toUInt();
#endif
            }
        }
    }

    // 命令行级别优先, 覆盖配置文件的设置
    if (parser.isSet(optTitle)) {
        szTitle = parser.value(optTitle);
    }
    if (parser.isSet(optUrl)) {
        szUrl = parser.value(optUrl);
    }
    if (parser.isSet(optWidth)) {
        width = parser.value(optWidth).toInt();
    }
    if (parser.isSet(optHeight)) {
        height = parser.value(optHeight).toInt();
    }

    if (parser.isSet(optTray)) {
        tray = true;
    }
    if (parser.isSet(optFullScreen)) {
        fullScreen = true;
    }
    if (parser.isSet(optFixSize)) {
        fixSize = true;
    }
    if (parser.isSet(optHideButtons)) {
        hideButtons = true;
    }

    if (parser.isSet(optIcon)) {
        szIcon = parser.value(optIcon);
    }
    if (parser.isSet(optDesc)) {
        szDesc = QString("%1<br/><br/>%2").arg(parser.value(optDesc)).arg(szDefaultDesc);
    }

    if (parser.isSet(optRootPath)) {
        szRootPath = parser.value(optRootPath);
    }

    if (parser.isSet(optPort)) {
        u16Port = static_cast<quint16>(parser.value(optPort).toUInt());
    }

    if (parser.isSet(useGPU)) {
        toUseGPU = parser.value(useGPU).toUInt();
    }

    QStringList chromium_flags = {"--disable-web-security"};
#if defined __sw_64__ || __loongarch__
    chromium_flags << "--no-sandbox";
#endif
    if (toUseGPU == true) {
        chromium_flags << "--ignore-gpu-blocklist"
                       << "--enable-gpu-rasterization"
                       << "--enable-native-gpu-memory-buffers"
                       << "--enable-accelerated-video-decode";
        qputenv("QTWEBENGINE_CHROMIUM_FLAGS", chromium_flags.join(" ").toUtf8());
        qDebug() << "Setting GPU to True.";
    }
    // 初始化 QtWebEngine 深色模式环境变量
    WebEngineView::handleChromiumFlags();

#if SSL_SERVER
    if (parser.isSet(optSSLPort)) {
        u16sslPort = parser.value(optSSLPort).toUInt();
    }
#endif

    // 没设置 -p 并且参数个数 > 1 并且第一个参数不是 - 开始的
    if (!parser.isSet(optParser) && argc > 1 && !QString(argv[1]).startsWith("-")) {
        // 按照固定顺序级别最优先
        if (argc > 1) {
            szTitle = argv[1];
        }
        if (argc > 2) {
            szUrl = argv[2];
        }
        if (argc > 3) {
            width = QString(argv[3]).toInt();
        }
        if (argc > 4) {
            height = QString(argv[4]).toInt();
        }

        if (argc > 5) {
            tray = true;
        }
        if (argc > 6) {
            fullScreen = true;
        }
        if (argc > 7) {
            fixSize = true;
        }
        if (argc > 8) {
            hideButtons = true;
        }

        if (argc > 9) {
            szIcon = QString(argv[9]);
        }
        if (argc > 10) {
            szDesc = QString("%1<br/><br/>%2").arg(QString(argv[10])).arg(szDefaultDesc);
        }
        if (argc > 11) {
            szRootPath = QString(argv[11]);
        }
        if (argc > 12) {
            u16Port = static_cast<quint16>(QString(argv[12]).toUInt());
        }
#if SSL_SERVER
        if (argc > 13) {
            u16sslPort = QString(argv[13]).toUInt();
        }
#endif
    }

    if (fixSize) {
        fullScreen = false; // 固定窗口大小时禁用全屏模式，避免标题栏按钮 BUG
    }

    // DApplication 单例运行（标题名称_当前登录用户 id）
    if (!a.setSingleInstance(szTitle + "_" + QString::number(getuid()))) {
        qInfo() << "Another instance has already started, now exit...";
        exit(0);
    }
    a.setQuitOnLastWindowClosed(!tray); // 启用托盘时，退出程序后服务不终止

#if SSL_SERVER
    if (!szRootPath.isEmpty() && u16Port > 0 && u16sslPort > 0) {
        HttpD httpd(szRootPath, u16Port, u16sslPort);
        httpd.start();
    }
#else
    if (!szRootPath.isEmpty() && u16Port > 0) {
        static HttpD httpd(szRootPath, u16Port);
        QObject::connect(&a, &Application::sigQuit, &httpd, &HttpD::stop);
        httpd.start();
    }
#endif

    MainWindow w(szTitle, szUrl, width, height, tray, fullScreen, fixSize, hideButtons);
    a.setMainWindow(&w);
    QObject::connect(&a, &Application::newInstanceStarted, &w, &MainWindow::slotNewInstanceStarted);
    QObject::connect(&w, &MainWindow::sigClose, &a, &Application::slotMainWindowClose);

    if (!szIcon.isEmpty()) {
        w.setIcon(szIcon);
    }
    if (!szDesc.isEmpty()) {
        w.setDescription(szDesc);
    }

    w.show();

    return a.exec();
}

#include "mainwindow.h"
#include "application.h"
#include "webengineview.h"
#include "webenginepage.h"

#include <DPlatformWindowHandle>
#include <DLog>
#include <DWidgetUtil>
#include <DTitlebar>
#include <DMessageManager>
#include <DDesktopServices>

#include <QKeyEvent>
#include <QWebEngineProfile>
#include <QFileInfo>
#include <QFileDialog>
#include <QDir>
#include <QStandardPaths>

#include <unistd.h>

DCORE_USE_NAMESPACE

MainWindow::MainWindow(QString szTitle,
                       QString szUrl,
                       int nWidth,
                       int nHeight,
                       bool nTray,
                       bool nFullScreen,
                       bool nFixSize,
                       bool nHideButtons,
                       QWidget *parent)
    : DMainWindow(parent)
    , m_title(szTitle)
    , m_url(szUrl)
    , m_width(nWidth)
    , m_height(nHeight)
    , m_isTrayEnabled(nTray)
    , m_isFullScreen(nFullScreen)
    , m_isFixedSize(nFixSize)
    , m_isHideButton(nHideButtons)
    , m_widget(new Widget(m_url, this, m_title))
    , m_tray(new QSystemTrayIcon(this))
    , btnBack(new DToolButton(titlebar()))
    , btnForward(new DToolButton(titlebar()))
    , btnRefresh(new DToolButton(titlebar()))
    , m_menu(new QMenu(titlebar()))
    , m_fullScreen(new QAction(QObject::tr("Full Screen"), this))
    , m_fixSize(new QAction(QObject::tr("Fix Size"), this))
    , m_hideButtons(new QAction(QObject::tr("Hide Buttons"), this))
    , m_clearCache(new QAction(QObject::tr("Clear Cache"), this))
    , t_menu(new QMenu(this))
    , t_show(new QAction(QObject::tr("Show MainWindow"), this))
    , t_about(new QAction(qApp->translate("TitleBarMenu", QString("About").toUtf8().data()), this))
    , t_exit(new QAction(qApp->translate("TitleBarMenu", QString("Exit").toUtf8().data()), this))
    , downloadMessage(new DFloatingMessage(DFloatingMessage::ResidentType, this))
    , downloadProgressWidget(new QWidget(downloadMessage))
    , progressBarLayout(new QHBoxLayout(downloadProgressWidget))
    , downloadProgressBar(new DProgressBar(downloadProgressWidget))
    , btnPause(new DPushButton(QObject::tr("Pause"), downloadProgressWidget))
    , btnResume(new DPushButton(QObject::tr("Resume"), downloadProgressWidget))
    , btnCancel(new DPushButton(QObject::tr("Cancel"), downloadProgressWidget))
    , isCanceled(false)
{
    initTmpDir();
    initLog();

    initUI();
    initTrayIcon();
    initConnections();
}

MainWindow::~MainWindow()
{
}

bool MainWindow::event(QEvent *event)
{
    /**
     * @bug QWebEngineView in Qt6 will recreate window handle
     * @ref https://github.com/linuxdeepin/deepin-deepinid-client/commit/2a305926a9047c699cf4d12e3e64aae17e8c367b
     */
    if (event->type() == QEvent::WinIdChange) {
        DPlatformWindowHandle handle(this);
    }

    return DMainWindow::event(event);
}

void MainWindow::setIcon(QString szIconPath)
{
    if (!QFile::exists(szIconPath)) {
        return;
    }

    titlebar()->setIcon(QIcon(szIconPath));
    setWindowIcon(QIcon(szIconPath));
    m_tray->setIcon(QIcon(szIconPath));

    qApp->setWindowIcon(QIcon::fromTheme(szIconPath));
    qApp->setProductIcon(QIcon::fromTheme(szIconPath));
}

void MainWindow::setDescription(const QString &desc)
{
    qApp->setApplicationDescription(desc);
}

QString MainWindow::title() const
{
    return m_title;
}

QString MainWindow::tmpDir() const
{
    QString orgName = qobject_cast<DApplication *>(qApp)->organizationName();
    QString appName = qobject_cast<DApplication *>(qApp)->applicationName();
    return QStandardPaths::writableLocation(QStandardPaths::TempLocation) + "/" + orgName + "/" + appName + "/" + m_title + "/" + QString::number(getuid());
}

void MainWindow::keyPressEvent(QKeyEvent *event)
{
    if (!m_fixSize->isChecked()) // 固定窗口大小时禁止全屏
    {
        if (event->key() == Qt::Key_F11) // 绑定键盘快捷键 F11
        {
            m_fullScreen->trigger();
            m_menu->update();
        }
    }

    DMainWindow::keyPressEvent(event);
}

void MainWindow::resizeEvent(QResizeEvent *event)
{
    if (this->isFullScreen()) {
        m_fullScreen->setChecked(true);
    } else {
        m_fullScreen->setChecked(false);
        if (!m_isFixedSize) {
            m_fixSize->setEnabled(true); // 命令行参数没有固定窗口大小时，窗口模式下允许手动选择固定窗口大小
        }
    }

    DMainWindow::resizeEvent(event);
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    if (!m_isTrayEnabled) {
        emit sigClose(); // 不启用托盘时，关闭主窗口则关闭关于窗口
    }

    DMainWindow::closeEvent(event);
}

void MainWindow::initLog()
{
    if (!QDir(tmpDir()).exists()) {
        return;
    }

    DLogManager::setlogFilePath(tmpDir() + "/" + "log");
    DLogManager::registerFileAppender();
    DLogManager::registerConsoleAppender();
    DLogManager::registerJournalAppender();
}

void MainWindow::initTmpDir()
{
    QDir dir(tmpDir());
    dir.removeRecursively();
    dir.mkpath(dir.path());
    if (!dir.exists()) {
        qCritical() << Q_FUNC_INFO << dir.path() << "not exists";
        return;
    }
    qDebug() << Q_FUNC_INFO << dir.path() << "created";
}

void MainWindow::initUI()
{
    // 初始化 MainWindow
    setCentralWidget(m_widget);
    centralWidget()->layout()->setContentsMargins(0, 0, 0, 0);

    resize(m_width, m_height);

    moveToCenter(this);

    setWindowIcon(QIcon(":/images/spark-webapp-runtime.svg"));

    initTitleBar();
    initDownloadProgressBar();

    fixSize();
    fullScreen();
    hideButtons(); // 修复指定hidebuttons之后没有生效
}

void MainWindow::initTitleBar()
{
    titlebar()->setTitle(m_title);
    titlebar()->setIcon(QIcon(":/images/spark-webapp-runtime.svg"));

    btnBack->setIcon(QIcon(":/images/go-previous-24.svg"));
    btnBack->setIconSize(QSize(36, 36));
    btnForward->setIcon(QIcon(":/images/go-next-24.svg"));
    btnForward->setIconSize(QSize(36, 36));
    btnRefresh->setIcon(QIcon(":/images/view-refresh.svg"));
    btnRefresh->setIconSize(QSize(36, 36));

    titlebar()->addWidget(btnBack, Qt::AlignLeft);
    titlebar()->addWidget(btnForward, Qt::AlignLeft);
    titlebar()->addWidget(btnRefresh, Qt::AlignLeft);

    m_fullScreen->setCheckable(true);
    m_fullScreen->setChecked(m_isFullScreen);
    m_fullScreen->setDisabled(m_isFixedSize); // NOTE: 固定窗口大小时禁用全屏模式，避免标题栏按钮显示问题
    m_fixSize->setCheckable(true);
    m_fixSize->setChecked(m_isFixedSize);
    m_fixSize->setDisabled(m_isFixedSize);
    m_hideButtons->setCheckable(true);
    m_hideButtons->setChecked(m_isHideButton);
    m_hideButtons->setDisabled(m_isHideButton);

    // 命令行设置参数后 GUI 中隐藏对应选项
    if (!m_isFixedSize) {
        m_menu->addAction(m_fullScreen);
        m_menu->addAction(m_fixSize);
    }

    if (!m_isHideButton) {
        m_menu->addAction(m_hideButtons);
    }

    if (m_menu->actions().size() > 0) {
        m_menu->addSeparator();
        m_menu->addAction(m_clearCache);
    }

    titlebar()->setMenu(m_menu);
    titlebar()->setAutoHideOnFullscreen(true);
}

void MainWindow::initDownloadProgressBar()
{
    // 初始化 DownloadProgressBar
    downloadProgressBar->setFixedSize(250, 8);
    btnPause->setFixedSize(80, 32);
    btnResume->setFixedSize(80, 32);
    btnCancel->setFixedSize(80, 32);

    progressBarLayout->setContentsMargins(0, 0, 0, 0);
    progressBarLayout->setSpacing(0);
    progressBarLayout->setAlignment(Qt::AlignCenter);
    progressBarLayout->addWidget(downloadProgressBar);
    progressBarLayout->addSpacing(10);
    progressBarLayout->addWidget(btnPause);
    progressBarLayout->addWidget(btnResume);
    progressBarLayout->addWidget(btnCancel);

    downloadMessage->setIcon(QIcon::fromTheme("deepin-download"));
    downloadMessage->setWidget(downloadProgressWidget);
    downloadMessage->hide();
}

void MainWindow::initTrayIcon()
{
    // 初始化 TrayIcon
    t_menu->addAction(t_show);
    t_menu->addAction(t_about);
    t_menu->addAction(t_exit);
    m_tray->setContextMenu(t_menu);
    m_tray->setToolTip(m_title);
    m_tray->setIcon(QIcon(":/images/spark-webapp-runtime.svg"));

    if (m_isTrayEnabled) {
        m_tray->show(); // 启用托盘时显示
    }
}

void MainWindow::initConnections()
{
    connect(btnBack, &DToolButton::clicked, this, [&]() {
        m_widget->goBack();
    });
    connect(btnForward, &DToolButton::clicked, this, [&]() {
        m_widget->goForward();
    });
    connect(btnRefresh, &DToolButton::clicked, this, [&]() {
        m_widget->refresh();
    });

    connect(m_fullScreen, &QAction::triggered, this, [=]() {
        fullScreen();
    });
    connect(m_fixSize, &QAction::triggered, this, [=]() {
        fixSize();
    });
    connect(m_hideButtons, &QAction::triggered, this, [=]() {
        hideButtons();
    });
    connect(m_clearCache, &QAction::triggered, this, [=]() {
        clearCache();
    });

    connect(t_show, &QAction::triggered, this, [=]() {
        this->activateWindow();
        fixSize();
    });
    connect(t_about, &QAction::triggered, this, [=]() {
        qobject_cast<Application *>(qApp)->triggerAboutAction();
    });
    connect(t_exit, &QAction::triggered, this, [=]() {
        exit(0);
    });
    connect(m_tray, &QSystemTrayIcon::activated, this, &MainWindow::on_trayIconActivated);

    connect(m_widget, &Widget::sigLoadErrorOccurred, this, &MainWindow::slotLoadErrorOccurred);
    connect(m_widget->getPage()->profile(), &QWebEngineProfile::downloadRequested, this, &MainWindow::on_downloadStart);
    connect(m_widget->getPage(), &QWebEnginePage::windowCloseRequested, this, [=]() {
        this->close();
    });
}

void MainWindow::fullScreen()
{
    if (m_fullScreen->isChecked()) {
        m_fixSize->setChecked(false);
        m_fixSize->setDisabled(true);
        m_menu->update();
        showFullScreen();
        // DMessageManager::instance()->sendMessage(this, QIcon::fromTheme("dialog-information"), QString(QObject::tr("%1Fullscreen Mode")).arg("    "));
    } else {
        if (!m_isFixedSize) {
            m_fixSize->setDisabled(false); // 命令行参数没有固定窗口大小时，窗口模式下允许手动选择固定窗口大小
        }
        m_menu->update();
        showNormal();
        // DMessageManager::instance()->sendMessage(this, QIcon::fromTheme("dialog-information"), QString(QObject::tr("%1Windowed Mode")).arg("    "));
    }
}

void MainWindow::fixSize()
{
    if (m_fixSize->isChecked()) {
        m_fullScreen->setChecked(false);
        m_fullScreen->setDisabled(true);
        m_menu->update();
        setFixedSize(this->size());
        // BUG: 启用托盘图标后，若手动选择固定窗口大小，并且关闭窗口，再次打开时会丢失最大化按钮，且无法恢复。
    } else {
        m_fullScreen->setDisabled(false);
        m_menu->update();
        setMinimumSize(m_width, m_height);
        setMaximumSize(QSize(QWIDGETSIZE_MAX, QWIDGETSIZE_MAX));
    }

    fullScreen();
}

void MainWindow::hideButtons()
{
    if (m_hideButtons->isChecked()) {
        btnBack->hide();
        btnForward->hide();
        btnRefresh->hide();
    } else {
        btnBack->show();
        btnForward->show();
        btnRefresh->show();
    }
}

void MainWindow::clearCache()
{
    // 清除缓存文件夹并刷新页面
    QDir dir(QStandardPaths::writableLocation(QStandardPaths::AppLocalDataLocation));
    if (dir.exists()) {
        dir.removeRecursively();
    }

    emit btnRefresh->clicked();
}

QString MainWindow::saveAs(QString fileName)
{
    QString saveFile = QFileDialog::getSaveFileName(this,
                                                    QObject::tr("Save As"),
                                                    QStandardPaths::writableLocation(QStandardPaths::DownloadLocation) + "/" + fileName);
    if (!saveFile.isEmpty()) {
        // 判断上层目录是否可写入
        if (QFileInfo(QFileInfo(saveFile).absoluteDir().canonicalPath()).isWritable()) {
            return saveFile;
        } else {
            return saveAs(fileName);
        }
    }
    return nullptr;
}

void MainWindow::slotNewInstanceStarted()
{
    this->setWindowState(Qt::WindowActive);
    this->activateWindow();
    this->show();
}

void MainWindow::on_trayIconActivated(QSystemTrayIcon::ActivationReason reason)
{
    switch (reason) {
    /* 响应托盘点击事件 */
    case QSystemTrayIcon::Trigger:
        this->setWindowState(Qt::WindowActive);
        this->activateWindow();
        fixSize();
        break;
    default:
        break;
    }
}

void MainWindow::on_downloadStart(QWebEngineDownloadRequest *request)
{
    // 尝试加锁互斥量，禁止同时下载多个文件
    if (mutex.tryLock()) {
        QString fileName = request->suggestedFileName();
        QString filePath = saveAs(fileName);
        if (filePath.isEmpty()) {
            mutex.unlock();
            return;
        }

        QFileInfo fileInfo(filePath);
        fileName = fileInfo.fileName();
        QString dirPath = fileInfo.absoluteDir().canonicalPath();
        filePath = QDir(dirPath).absoluteFilePath(fileName);

        request->setDownloadDirectory(dirPath);
        request->setDownloadFileName(fileName);

        connect(request, &QWebEngineDownloadRequest::receivedBytesChanged, this, &MainWindow::on_receivedBytesChanged);
        connect(request, &QWebEngineDownloadRequest::isFinishedChanged, this, [=]() {
            on_downloadFinish(filePath);
        });

        connect(btnPause, &DPushButton::clicked, this, [=]() {
            on_downloadPause(request);
        });
        connect(btnResume, &DPushButton::clicked, this, [=]() {
            on_downloadResume(request);
        });
        connect(btnCancel, &DPushButton::clicked, this, [=]() {
            on_downloadCancel(request);
        });

        DFloatingMessage *message = new DFloatingMessage(DFloatingMessage::TransientType);
        message->setIcon(QIcon::fromTheme("dialog-information"));
        message->setMessage(QObject::tr("%1Start downloading %2").arg("    ", fileName));
        DMessageManager::instance()->sendMessage(this, message);

        request->accept();

        // 重置 DownloadProgressBar 状态
        isCanceled = false;
        btnResume->hide();
        btnPause->show();
        this->downloadMessage->show(); // 上一次下载完成后隐藏了进度条，这里要重新显示
    } else {
        DMessageManager::instance()->sendMessage(this, QIcon::fromTheme("dialog-cancel"), QString(QObject::tr("%1Wait for previous download to complete!")).arg("    "));
    }
}

void MainWindow::on_receivedBytesChanged()
{
    QWebEngineDownloadRequest *request = qobject_cast<QWebEngineDownloadRequest *>(sender());

    int value = int(double(request->receivedBytes()) / double(request->totalBytes()) * 100.0);
    downloadProgressBar->setValue(qBound(downloadProgressBar->value(), value, 100));

    downloadMessage->setMessage("    " + QString::number(value) + "%");

    DMessageManager::instance()->sendMessage(this, downloadMessage);
}

void MainWindow::on_downloadFinish(QString filePath)
{
    QWebEngineDownloadRequest *request = qobject_cast<QWebEngineDownloadRequest *>(sender());
    if (!request->isFinished()) {
        return;
    }

    mutex.unlock(); // 解锁互斥量，允许下载新文件

    downloadMessage->hide();

    // 下载完成显示提示信息
    if (!isCanceled) {
        DPushButton *button = new DPushButton(QObject::tr("Open"));

        DFloatingMessage *message = new DFloatingMessage(DFloatingMessage::ResidentType);
        message->setIcon(QIcon::fromTheme("dialog-ok"));
        message->setMessage(QString("    %1 %2 %3").arg(QFileInfo(filePath).fileName(), QObject::tr("download finished."), QObject::tr("Show in file manager?")));
        message->setWidget(button);
        DMessageManager::instance()->sendMessage(this, message);

        connect(button, &DPushButton::clicked, this, [=]() {
            DDesktopServices::showFileItem(filePath);
            message->hide();
            message->deleteLater();
        });
    }
}

void MainWindow::on_downloadPause(QWebEngineDownloadRequest *request)
{
    request->pause();

    downloadMessage->setIcon(QIcon::fromTheme("package-download-failed"));
    btnResume->show();
    btnPause->hide();
}

void MainWindow::on_downloadResume(QWebEngineDownloadRequest *request)
{
    request->resume();

    downloadMessage->setIcon(QIcon::fromTheme("deepin-download"));
    btnResume->hide();
    btnPause->show();
}

void MainWindow::on_downloadCancel(QWebEngineDownloadRequest *request)
{
    isCanceled = true; // 取消下载
    request->cancel();

    mutex.unlock();

    downloadMessage->hide();
    DMessageManager::instance()->sendMessage(this, QIcon::fromTheme("dialog-error"), QString(QObject::tr("%1Download canceled!")).arg("    "));
}

void MainWindow::slotLoadErrorOccurred()
{
    DMessageManager::instance()->sendMessage(this, QIcon::fromTheme("dialog-warning"), QString(QObject::tr("%1Load error occurred!")).arg("    "));
}

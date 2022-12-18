#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <DMainWindow>
#include <DAboutDialog>
#include <DToolButton>
#include <DProgressBar>
#include <DPushButton>
#include <DFloatingMessage>

#include <QSystemTrayIcon>
#include <QProcess>
#include <QMutex>

#include "widget.h"
#include "globaldefine.h"

DWIDGET_USE_NAMESPACE

class MainWindow : public DMainWindow
{
    Q_OBJECT

public:
    MainWindow(QString szTitle = DEFAULT_TITLE,
               QString szUrl = DEFAULT_URL,
               int nWidth = DEFAULT_WIDTH,
               int nHeight = DEFAULT_HEIGHT,
               bool nTray = false,
               bool nFullScreen = false,
               bool nFixSize = false,
               bool nHideButtons = false,
               QWidget *parent = nullptr);
    ~MainWindow();

    void setIcon(QString szIconPath);
    void setDescription(const QString &desc);

    QString title() const;
    QString tmpDir() const;

protected:
    void keyPressEvent(QKeyEvent *event);
    void resizeEvent(QResizeEvent *event);
    void closeEvent(QCloseEvent *event);

private:
    void initLog();
    void initTmpDir();

    void initUI();
    void initTitleBar();
    void initDownloadProgressBar();
    void initTrayIcon();
    void initConnections();

    void fullScreen();
    void fixSize();
    void hideButtons();
    void clearCache();

    QString saveAs(QString fileName);

signals:
    void sigClose();

public slots:
    void slotNewInstanceStarted();

private slots:
    void on_trayIconActivated(QSystemTrayIcon::ActivationReason reason);

    void on_downloadStart(QWebEngineDownloadItem *item);
    void on_downloadProgress(qint64 bytesReceived, qint64 bytesTotal);
    void on_downloadFinish(QString filePath);
    void on_downloadPause(QWebEngineDownloadItem *item);
    void on_downloadResume(QWebEngineDownloadItem *item);
    void on_downloadCancel(QWebEngineDownloadItem *item);

    void slotLoadErrorOccurred();

private:
    QString m_title, m_url;
    int m_width, m_height;
    bool m_isTrayEnabled, m_isFullScreen, m_isFixedSize, m_isHideButton;

    Widget *m_widget;
    QSystemTrayIcon *m_tray;

    DToolButton *btnBack;
    DToolButton *btnForward;
    DToolButton *btnRefresh;

    QMenu *m_menu;
    QAction *m_fullScreen;
    QAction *m_fixSize;
    QAction *m_hideButtons;
    QAction *m_clearCache;

    QMenu *t_menu;
    QAction *t_show;
    QAction *t_about;
    QAction *t_exit;

    DFloatingMessage *downloadMessage;
    QWidget *downloadProgressWidget;
    QHBoxLayout *progressBarLayout;
    DProgressBar *downloadProgressBar;
    DPushButton *btnPause;
    DPushButton *btnResume;
    DPushButton *btnCancel;

    QMutex mutex; // 通过 Mutex 互斥量禁止同时下载多个文件（使用简单的 bool 变量应该也可以实现该功能？）
    bool isCanceled; // 判断是否为取消下载
};

#endif // MAINWINDOW_H

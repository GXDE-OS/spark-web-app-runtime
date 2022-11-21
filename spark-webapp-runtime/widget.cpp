#include "widget.h"
#include "webengineview.h"
#include "webenginepage.h"

#include <DApplication>

DWIDGET_USE_NAMESPACE

Widget::Widget(QString szUrl, QWidget *parent)
    : QWidget(parent)
    , m_webEngineView(new WebEngineView(this))
    , m_spinner(new DSpinner(this))
    , mainLayout(new QStackedLayout(this))
    , m_szUrl(szUrl)
{
    initUI();
    initConnections();
}

Widget::~Widget()
{
}

QWebEnginePage *Widget::getPage()
{
    return this->m_webEngineView->page();
}

void Widget::goBack()
{
    m_webEngineView->back();
}

void Widget::goForward()
{
    m_webEngineView->forward();
}

void Widget::refresh()
{
    m_webEngineView->reload();
}

void Widget::initUI()
{
    m_spinner->setFixedSize(96, 96);

    DApplication *dApp = qobject_cast<DApplication *>(qApp);
    m_webEngineView->setZoomFactor(dApp->devicePixelRatio());

    WebEnginePage *page = new WebEnginePage(m_webEngineView);
    m_webEngineView->setPage(page);

    page->setUrl(QUrl());
    if (!m_szUrl.isEmpty()) {
        page->setUrl(QUrl(m_szUrl));
    }

    QWidget *spinnerWidget = new QWidget(this);
    QHBoxLayout *spinnerLayout = new QHBoxLayout(spinnerWidget);
    spinnerLayout->setMargin(0);
    spinnerLayout->setSpacing(0);
    spinnerLayout->setAlignment(Qt::AlignCenter);
    spinnerLayout->addStretch();
    spinnerLayout->addWidget(m_spinner);
    spinnerLayout->addStretch();

    mainLayout->addWidget(spinnerWidget);
    mainLayout->addWidget(m_webEngineView);
}

void Widget::initConnections()
{
    connect(m_webEngineView, &QWebEngineView::loadStarted, this, &Widget::slotLoadStarted, Qt::UniqueConnection);
    connect(m_webEngineView, &QWebEngineView::loadProgress, this, &Widget::slotLoadProgress, Qt::UniqueConnection);
    connect(m_webEngineView, &QWebEngineView::loadFinished, this, &Widget::slotLoadFinished, Qt::UniqueConnection);

    // FIXME: DTK 主题切换时，动态修改 QtWebEngine prefers-color-scheme
    //    connect(DGuiApplicationHelper::instance(), &DGuiApplicationHelper::paletteTypeChanged, this, &Widget::slotPaletteTypeChanged, Qt::UniqueConnection);
}

void Widget::updateLayout()
{
    slotLoadStarted();

    mainLayout->removeWidget(m_webEngineView);
    QUrl url = m_webEngineView->url();
    m_webEngineView->deleteLater();

    m_webEngineView = new WebEngineView(this);
    mainLayout->addWidget(m_webEngineView);
    initConnections();

    DApplication *dApp = qobject_cast<DApplication *>(qApp);
    m_webEngineView->setZoomFactor(dApp->devicePixelRatio());

    WebEnginePage *page = new WebEnginePage(m_webEngineView);
    m_webEngineView->setPage(page);
    page->setUrl(url);
}

void Widget::slotLoadStarted()
{
    mainLayout->setCurrentIndex(0);
    m_spinner->start();
}

void Widget::slotLoadProgress(int value)
{
    if (value == 100) {
        slotLoadFinished(-1);
    }
}

void Widget::slotLoadFinished(int status)
{
    m_spinner->stop();
    mainLayout->setCurrentIndex(1);

    if (status < 0) {
        qDebug() << Q_FUNC_INFO << "Load progress: 100%";
        return;
    }

    if (!status) {
        qWarning() << Q_FUNC_INFO << "Load finished, error occurred!";
        emit sigLoadErrorOccurred();
    }
}

void Widget::slotPaletteTypeChanged(DGuiApplicationHelper::ColorType paletteType)
{
    Q_UNUSED(paletteType)

    WebEngineView::handleChromiumFlags();
    if (m_webEngineView) {
        updateLayout();
    }
}

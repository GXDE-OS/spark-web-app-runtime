#ifndef WIDGET_H
#define WIDGET_H

#include <DSpinner>
#include <DGuiApplicationHelper>

#include <QWidget>
#include <QWebEnginePage>
#include <QStackedLayout>

DWIDGET_USE_NAMESPACE
DGUI_USE_NAMESPACE

class WebEngineView;
class Widget : public QWidget
{
    Q_OBJECT

public:
    explicit Widget(QString szUrl = nullptr, QWidget *parent = nullptr);
    ~Widget();

    QWebEnginePage *getPage();
    void goBack();
    void goForward();
    void refresh();

private:
    void initUI();
    void initConnections();
    void updateLayout();

signals:
    void sigLoadErrorOccurred();

private slots:
    void slotLoadStarted();
    void slotLoadProgress(int value);
    void slotLoadFinished(int status);

    void slotPaletteTypeChanged(DGuiApplicationHelper::ColorType paletteType);

private:
    WebEngineView *m_webEngineView = nullptr;
    DSpinner *m_spinner = nullptr;
    QStackedLayout *mainLayout = nullptr;

    QString m_szUrl;
};

#endif // WIDGET_H

#ifndef WIDGET_H
#define WIDGET_H

#include <DSpinner>

#include <QWidget>
#include <QWebEnginePage>
#include <QStackedLayout>

DWIDGET_USE_NAMESPACE

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

private slots:
    void on_loadStarted();
    void on_loadFinished();

private:
    WebEngineView *m_webEngineView = nullptr;
    DSpinner *m_spinner = nullptr;
    QStackedLayout *mainLayout = nullptr;

    QString m_szUrl;
};

#endif // WIDGET_H

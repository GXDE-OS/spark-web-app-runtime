#ifndef APPLICATION_H
#define APPLICATION_H

#include "mainwindow.h"

#include <DApplication>

DWIDGET_USE_NAMESPACE

class Application : public DApplication
{
    Q_OBJECT

public:
    Application(int &argc, char **argv);
    void triggerAboutAction();

    QStringList launchParams() const;

    void setMainWindow(MainWindow *window);
    MainWindow *mainWindow();

protected:
    void handleAboutAction() override;

private:
    void saveLaunchParams(int &argc, char **argv);

signals:
    void sigQuit();

public slots:
    void slotMainWindowClose();

private:
    MainWindow *m_mainWindow = nullptr;

    int m_argc;
    QStringList m_argv;
};

#endif // APPLICATION_H

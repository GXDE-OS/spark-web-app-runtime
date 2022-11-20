#ifndef APPLICATION_H
#define APPLICATION_H

#include <DApplication>

DWIDGET_USE_NAMESPACE

class Application : public DApplication
{
    Q_OBJECT

public:
    Application(int &argc, char **argv);
    void handleAboutAction() override;

private:
    void initAboutDialog();

signals:
    void sigQuit();

public slots:
    void slotMainWindowClose();
};

#endif // APPLICATION_H

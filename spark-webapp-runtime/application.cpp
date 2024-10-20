#include "application.h"
#include "globaldefine.h"

#include <DPlatformWindowHandle>
#include <DAboutDialog>

#include <QStandardPaths>

#include <unistd.h>

const QString websiteLinkTemplate = "<a href='%1' style='text-decoration: none; color: rgba(0,129,255,0.9);'>%2</a>";

Application::Application(int &argc, char **argv)
    : DApplication(argc, argv)
{
    saveLaunchParams(argc, argv);

    loadTranslator();

    setAttribute(Qt::AA_UseHighDpiPixmaps);
    if (!DPlatformWindowHandle::pluginVersion().isEmpty()) {
        setAttribute(Qt::AA_DontCreateNativeWidgetSiblings, true);
    }

    setOrganizationName(ORGANIZATION_NAME); // 添加组织名称，和商店主体的文件夹同在 ~/.local/share/spark-union 文件夹下
    setApplicationName(PROJECT_NAME); // 这里不要翻译，否则 ~/.local/share 中文件夹名也会被翻译
    setApplicationVersion(APP_VERSION);
    setApplicationDisplayName(DEFAULT_TITLE);
    setWindowIcon(QIcon(":/images/spark-webapp-runtime.svg"));

    setProductIcon(QIcon(":/images/spark-webapp-runtime.svg"));
    setProductName(websiteLinkTemplate.arg("https://gitee.com/deepin-community-store/spark-web-app-runtime", DEFAULT_TITLE));
    setApplicationDescription(QObject::tr("Presented By Spark developers # HadesStudio"));
    setApplicationLicense(websiteLinkTemplate.arg("https://gitee.com/spark-store-project/spark-web-app-runtime/blob/master/LICENSE", "GPLv3"));
}

void Application::triggerAboutAction()
{
    handleAboutAction();
}

QStringList Application::launchParams() const
{
    return m_argv;
}

void Application::setMainWindow(MainWindow *window)
{
    m_mainWindow = window;
}

MainWindow *Application::mainWindow()
{
    return m_mainWindow;
}

void Application::handleAboutAction()
{
    DApplication::handleAboutAction();

    DAboutDialog *dialog = aboutDialog();
    if (dialog) {
        // CompanyLogo
        dialog->setCompanyLogo(QPixmap(":/images/Logo-Spark.png"));
        // WebsiteName
        dialog->setWebsiteName("Spark Project");
        // WebsiteLink
        dialog->setWebsiteLink("https://gitee.com/deepin-community-store/");
    }
}

void Application::saveLaunchParams(int &argc, char **argv)
{
    m_argc = argc;

    m_argv.clear();
    for (int i = 0; i < m_argc; i++) {
        m_argv.append(argv[i]);
    }

    qDebug() << Q_FUNC_INFO << m_argc << m_argv;
}

void Application::slotMainWindowClose()
{
    if (aboutDialog()) {
        aboutDialog()->close();
    }
}

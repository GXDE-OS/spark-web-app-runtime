#include "application.h"
#include "globaldefine.h"

#include <DPlatformWindowHandle>
#include <DAboutDialog>

#include <QStandardPaths>

#include <unistd.h>

Application::Application(int &argc, char **argv)
    : DApplication(argc, argv)
{
    saveLaunchParams(argc, argv);

    loadTranslator();

    setAttribute(Qt::AA_UseHighDpiPixmaps);
    if (!DPlatformWindowHandle::pluginVersion().isEmpty()) {
        setAttribute(Qt::AA_DontCreateNativeWidgetSiblings, true);
    }

    setApplicationVersion(QString(CURRENT_VER));
    setOrganizationName(ORGANIZATION_NAME); // 添加组织名称，和商店主体的文件夹同在 ~/.local/share/spark-union 文件夹下
    setApplicationName(APPLICATION_NAME); // 这里不要翻译，否则 ~/.local/share 中文件夹名也会被翻译
    setProductName(DEFAULT_TITLE);
    setApplicationDisplayName(DEFAULT_TITLE);
    setApplicationLicense(" <a href='https://www.gnu.org/licenses/gpl-3.0.html'>GPLv3</a> ");

    initAboutDialog();
}

void Application::handleAboutAction()
{
    if (aboutDialog()) {
        DApplication::handleAboutAction();
        return;
    }

    initAboutDialog();
    DApplication::handleAboutAction();
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

void Application::saveLaunchParams(int &argc, char **argv)
{
    m_argc = argc;

    m_argv.clear();
    for (int i = 0; i < m_argc; i++) {
        m_argv.append(argv[i]);
    }

    qDebug() << Q_FUNC_INFO << m_argc << m_argv;
}

void Application::initAboutDialog()
{
    // Customized DAboutDialog
    DAboutDialog *dialog = new DAboutDialog(activeWindow());
    // WindowIcon
    dialog->setWindowIcon(QIcon(":/images/spark-webapp-runtime.svg"));
    // ProductIcon
    dialog->setProductIcon(QIcon(":/images/spark-webapp-runtime.svg"));
    // ProductName
    dialog->setProductName(productName());
    // Version
    dialog->setVersion(translate("DAboutDialog", "Version: %1").arg(applicationVersion()));
    // CompanyLogo
    dialog->setCompanyLogo(QPixmap(":/images/Logo-Spark.png"));
    // Description

    QString szDefaultDesc = QString("<a href='https://gitee.com/deepin-community-store/spark-web-app-runtime'><span style='font-size:12pt;font-weight:500;'>%1</span></a><br/>"
                                    "<span style='font-size:12pt;'>%2</span>")
                                .arg(DEFAULT_TITLE)
                                .arg(QObject::tr("Presented By Spark developers # HadesStudio"));

    dialog->setDescription(szDefaultDesc);
    // WebsiteName
    dialog->setWebsiteName("Spark Project");
    // WebsiteLink
    dialog->setWebsiteLink("https://gitee.com/deepin-community-store/");
    // License
    dialog->setLicense(translate("DAboutDialog", "%1 is released under %2").arg(productName()).arg(applicationLicense()));

    setAboutDialog(dialog);
    connect(aboutDialog(), &DAboutDialog::destroyed, this, [=] {
        setAboutDialog(nullptr);
    });

    dialog->hide();
}

void Application::slotMainWindowClose()
{
    if (aboutDialog()) {
        aboutDialog()->close();
        aboutDialog()->deleteLater();
    }
}

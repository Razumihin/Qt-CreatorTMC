#pragma once
#include <ui_loginscreen.h>
#include "testmycode_global.h"
#include <extensionsystem/iplugin.h>
#include <QWidget>

#include <projectexplorer/project.h>
#include <projectexplorer/projectexplorer.h>
#include <projectexplorer/session.h>
#include <projectexplorer/target.h>

#include <extensionsystem/iplugin.h>
#include <utils/hostosinfo.h>
#include <utils/fileutils.h>

#include <QAction>
#include <QMessageBox>
#include <QMainWindow>
#include <QMenu>
#include <QObject>
#include <QString>
#include <QCoreApplication>
#include <QProcessEnvironment>
#include <QFileInfo>

namespace TestMyCodePlugin {
namespace Internal {

class TestMyCode : public ExtensionSystem::IPlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.qt-project.Qt.QtCreatorPlugin" FILE "TestMyCode.json")

public:
    TestMyCode();
    ~TestMyCode();

    bool initialize(const QStringList &arguments, QString *errorString);
    void extensionsInitialized();
    ShutdownFlag aboutToShutdown();

private slots:
    void on_cancelbutton_clicked();
    void on_loginbutton_clicked();

private:
    // Loginform
    Ui::loginform *login;
    QWidget *loginWidget;
    void showLoginWidget();
    void runOnActiveProject();

    void triggerAction();
    void launchTmcCLI(const QString &workingDirectory);
    bool tryLauchingTmcCLI(const QProcessEnvironment &env,
                                    const QString &workingDirectory);
    Utils::FileName tmcBinary() const;
    QProcessEnvironment processEnvironment() const;
    QStringList searchPathList() const;
    void onCurrentProjectChanged(ProjectExplorer::Project *project);
    QString readTMCOutput(const QString &testOutput);
};

} // namespace Internal
} // namespace TestMyCodePlugin
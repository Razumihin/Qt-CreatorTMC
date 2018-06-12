#include "settingswidget.h"
#include "testmycodeconstants.h"

#include <QSettings>
#include <QFileDialog>
#include <QMessageBox>
#include <QStandardPaths>

#include <algorithm>

SettingsWidget::SettingsWidget(TmcClient *client, QWidget *parent) :
    QWidget(parent),
    m_client(client)
{

    QSettings settings("TestMyQt", "TMC");
    m_client->setAccessToken(settings.value("accessToken", "").toString());
    m_client->setClientId(settings.value("clientId", "").toString());
    m_client->setClientSecret(settings.value("clientSecret", "").toString());
    m_client->setServerAddress(settings.value("server", "").toString());

    connect(m_client, &TmcClient::authorizationFinished, this, &SettingsWidget::handleAuthResponse);
    connect(m_client, &TmcClient::authenticationFinished, this, &SettingsWidget::handleLoginResponse);

    connect(m_client, &TmcClient::organizationListReady, this, &SettingsWidget::handleOrganizationList);
    connect(m_client, &TmcClient::courseListReady, this, &SettingsWidget::handleCourseList);

    settingsWindow = new Ui::settingsForm;
    settingsWindow->setupUi(this);

    // Initialize login window
    loginWidget = new LoginWidget(m_client);

    m_orgComboBox = settingsWindow->orgComboBox;
    m_courseComboBox = settingsWindow->courseComboBox;
    m_workingDir = settingsWindow->workingDir;
    m_cliLocation = settingsWindow->cliLocation;
    m_autoUpdateInterval = settingsWindow->updateInterval;
    m_userLoggedInLabel = settingsWindow->userLoggedInLabel;

    m_activeOrganization = Organization::fromQSettings(&settings);
    m_activeCourse = Course::fromQSettings(&settings);
    m_activeCourse.exerciseListFromQSettings(&settings);

    tmcCliLocation = settings.value("tmcCliLocation", "").toString();
    workingDirectory = settings.value("workingDir", "").toString();
    m_workingDir->setText(workingDirectory);
    m_interval = settings.value("autoupdateInterval", 60).toInt();
    m_autoUpdateInterval->setValue(m_interval);
    m_cliLocation->setText(tmcCliLocation);
    m_userLoggedInLabel->setText("Logged in as <strong>" + settings.value("username").toString() + "</strong>");

    settings.deleteLater();

    connect(settingsWindow->logoutButton, &QPushButton::clicked, this, [=](){
        clearCredentials();
        close();
    });
    connect(settingsWindow->cancelButton, &QPushButton::clicked, this, [=](){
        setComboboxIndex(m_orgComboBox, m_activeOrganization.getName());
        setComboboxIndex(m_courseComboBox, m_activeCourse.getName());
        close();
    });

    connect(settingsWindow->okButton, &QPushButton::clicked, this, &SettingsWidget::onSettingsOkClicked);
    connect(settingsWindow->browseButton, &QPushButton::clicked, this, &SettingsWidget::onBrowseClicked);
    connect(settingsWindow->cliBrowseButton, &QPushButton::clicked, this, &SettingsWidget::onCliBrowseClicked);
    connect(m_orgComboBox, static_cast<void(QComboBox::*)(int)>(&QComboBox::activated), [=](int index){
        Organization selectedOrg = m_orgComboBox->itemData(index).value<Organization>();
        if (!selectedOrg.getName().isEmpty()) {
            m_client->getCourseList(selectedOrg);
        } else {
            m_courseComboBox->clear();
        }
    });
}

void SettingsWidget::setUpdateInterval(int interval)
{
    m_interval = interval;
    emit autoUpdateIntervalChanged(m_interval);
}

void SettingsWidget::display()
{
    if (!m_client->isAuthenticated()) {
        showLoginWidget();
        return;
    }

    m_client->getOrganizationList();
    if (!m_activeOrganization.getName().isEmpty()) {
        m_client->getCourseList(m_activeOrganization);
    }
    m_workingDir->setText(workingDirectory);
    m_cliLocation->setText(tmcCliLocation);
    show();
}

void SettingsWidget::showLoginWidget()
{
    loginWidget->loadQSettings();
    loginWidget->show();
}

QString SettingsWidget::getWorkingDirectory()
{
    return workingDirectory;
}

Course* SettingsWidget::getActiveCourse()
{
    return &m_activeCourse;
}

void SettingsWidget::setComboboxIndex(QComboBox *box, QString value)
{
    int index = box->findText(value);
    if (index != -1) {
        box->setCurrentIndex(index);
    }
}

int SettingsWidget::getAutoupdateInterval()
{
    return m_interval;
}

QString SettingsWidget::getTmcCliLocation()
{
    return tmcCliLocation;
}

bool SettingsWidget::haveTmcCli()
{
    return !tmcCliLocation.isEmpty();
}

void SettingsWidget::onBrowseClicked()
{
    QString dir = askSaveLocation();
    m_workingDir->setText(dir);
}

void SettingsWidget::onCliBrowseClicked()
{
    QString jar = QFileDialog::getOpenFileName(this, tr("Choose TMC CLI .jar file"),
                                               "", tr("Jar files (*.jar)"));
    m_cliLocation->setText(jar);
}

void SettingsWidget::clearCredentials()
{
    QSettings settings("TestMyQt", "TMC");
    settings.setValue("username", "");
    settings.setValue("accessToken", "");
    settings.deleteLater();
    m_client->setAccessToken("");
    emit enableDownloadSubmit(false);
}

QString SettingsWidget::askSaveLocation()
{
    QFileDialog dialog(this);
    dialog.setFileMode(QFileDialog::Directory);
    dialog.setOption(QFileDialog::ShowDirsOnly, true);

    if (!dialog.exec())
        return QString();

    QString directory = dialog.selectedFiles().at(0);
    return directory;
}

void SettingsWidget::handleLoginResponse(QString accessToken)
{
    QSettings settings("TestMyQt", "TMC");
    if (accessToken == "") {
        settings.setValue("username", "");
    } else {
        settings.setValue("accessToken", accessToken);
        emit enableDownloadSubmit(true);
        display();
    }
    settings.deleteLater();
}

void SettingsWidget::handleAuthResponse(QString clientId, QString clientSecret)
{
    QSettings settings("TestMyQt", "TMC");
    settings.setValue("clientId", clientId);
    settings.setValue("clientSecret", clientSecret);
    settings.deleteLater();
}

void SettingsWidget::handleCourseList(Organization org)
{
    m_courseComboBox->clear();
    QList<Course> courses = org.getCourses();
    std::sort(courses.begin(), courses.end(), [](const Course& lhs, const Course& rhs)
    {
        return lhs.getTitle() < rhs.getTitle();
    });

    foreach (Course c, courses) {
        m_courseComboBox->addItem(c.getTitle(), QVariant::fromValue(c));
    }
    setComboboxIndex(m_courseComboBox, m_activeCourse.getTitle());
}

void SettingsWidget::handleOrganizationList(QList<Organization> orgs)
{
    m_organizations = orgs;

    std::sort(m_organizations.begin(), m_organizations.end(), [](const Organization& lhs, const Organization& rhs)
    {
        return lhs.getName() < rhs.getName();
    });

    m_orgComboBox->clear();

    bool hasActiveOrg = !!m_activeOrganization;
    // Add "empty" org if we have none selected
    if (!hasActiveOrg) {
        m_orgComboBox->addItem(QString("Select an organization"),
                               QVariant::fromValue(Organization()));
    }

    foreach (Organization org, m_organizations) {
        m_orgComboBox->addItem(org.getName(), QVariant::fromValue(org));
    }

    if (hasActiveOrg) {
        setComboboxIndex(m_orgComboBox, m_activeOrganization.getName());
    } else {
        m_orgComboBox->setCurrentIndex(0);
        m_courseComboBox->clear();
    }
}

void SettingsWidget::onSettingsOkClicked()
{
    if (m_workingDir->text() == "") {
        QMessageBox::critical(this, "TMC", "Please set the working directory!", QMessageBox::Ok);
        return;
    }

    QSettings settings("TestMyQt", "TMC");
    QString setDir = m_workingDir->text();
    if (setDir != workingDirectory) {
        workingDirectory = setDir;
        settings.setValue("workingDir", workingDirectory);
        emit workingDirectoryChanged(workingDirectory);
    }

    QString setCli = m_cliLocation->text();
    if (setCli != tmcCliLocation) {
        tmcCliLocation = setCli;
        settings.setValue("tmcCliLocation", tmcCliLocation);
        emit tmcCliLocationChanged(tmcCliLocation);
    }

    int setInterval = m_autoUpdateInterval->value();
    if (setInterval != m_interval) {
        m_interval = setInterval;
        settings.setValue("autoupdateInterval", m_interval);
        emit autoUpdateIntervalChanged(m_interval);
    }

    Organization setOrg = m_orgComboBox->currentData().value<Organization>();
    if (m_activeOrganization != setOrg) {
        m_activeOrganization = setOrg;
        Organization::toQSettings(&settings, setOrg);
        emit organizationChanged(setOrg);
    }

    Course setCourse = m_courseComboBox->currentData().value<Course>();
    if (!!setCourse && m_activeCourse != setCourse) {
        m_activeCourse = setCourse;
        Course::toQSettings(&settings, setCourse);
        emit activeCourseChanged(&m_activeCourse);
    }
    settings.deleteLater();
    close();
}

/*!
    \class Organization
    \inmodule lib/tmcclient
    \inheaderfile organization.h
    \brief Class \l Organization objects represent the educational organizations
        that have TMC courses available.

    Each \l Organization object contains a collection of \l Course objects that
    correspond to the courses the organization is offering online. Other important
    fields of \l Organization objects include the organization name and slug.

    \note The organization slug is a short string that is meant to uniquely
    identify the organization in a technical context. For example, the organization
    slug of the Aalto University School of Business is \e aalto-biz. An example of
    its use is in the URL \c {https://tmc.mooc.fi/api/v8/core/org/aalto-biz/courses}.
*/

#include "organization.h"

Organization::Organization()
{
    m_name = "";
    m_slug = "";
}

Organization::Organization(QString name, QString slug) :
    m_name(name),
    m_slug(slug)
{

}

/*!
    Returns \c true if the \l Organization object is equal to the parameter \a other;
    otherwise returns \c false. The overloaded operator expression \c {o1 == o2}
    evaluates to \c true when both the \l {Organization::getName()} {name} and
    \l {Organization::getSlug()} {slug} fields of the \l Organization objects
    \c o1 and \c o2 are equal; otherwise the expression evaluates to \c false.
*/
bool Organization::operator==(const Organization &other) const
{
    return other.getName() == m_name &&
            other.getSlug() == m_slug;
}

/*!
    Returns \c true if the \l Organization object is distinct from parameter \a other;
    otherwise returns \c false. The overloaded operator expression \c {o1 != o2}
    evaluates to \c true when either the \l {Organization::getName()} {name} or
    \l {Organization::getSlug()} {slug} fields of the operands are not equal;
    otherwise the expression evaluates to \c false.
*/
bool Organization::operator!=(const Organization &other) const
{
    return !(*this == other);
}

bool Organization::operator!() const
{
    return m_slug.isEmpty();
}

/*!
    Getter function for the name field of the \l Organization object.
*/
QString Organization::getName() const
{
    return m_name;
}

/*!
    Getter function for the slug field of the \l Organization object.
*/
QString Organization::getSlug() const
{
    return m_slug;
}

/*!
    Adds the \l Course parameter \a c to the collection of the organization's courses.
*/
void Organization::addCourse(Course c)
{
    m_courses.append(c);
}

/*!
    Getter function for the \l Organization object's collection of \l Course objects.
*/
QList<Course> Organization::getCourses()
{
    return m_courses;
}

/*!
    Creates and returns an \l Organization object from the values extracted
    from the \l {http://doc.qt.io/qt-5/qsettings.html} {QSettings} parameter
    \a settings. The values in \a settings are the organization
    \l {Organization::getName()} {name} and \l {Organization::getSlug()} {slug}.
    The main purpose of the function is to preserve the user-selected organization
    between sessions.
*/
Organization Organization::fromQSettings(QSettings *settings)
{
    return Organization(settings->value("orgName", "").toString(),
                        settings->value("orgSlug", "").toString());
}

/*!
    The function does the inverse of \l {Organization::} {fromQSettings()} in
    that it stores the name and slug fields of the \l Organization parameter
    \a org in the \l {http://doc.qt.io/qt-5/qsettings.html} {QSettings}
    parameter \a settings.
*/
void Organization::toQSettings(QSettings *settings, Organization org)
{
    settings->setValue("orgName", org.getName());
    settings->setValue("orgSlug", org.getSlug());
}

/*!
    Uses the \l {http://doc.qt.io/qt-5/qjsonobject.html} {QJsonObject}
    parameter \a jsonOrg to initialize and return a new \l Organization object.
    The \l {Organization::getName()} {name} and \l {Organization::getSlug()} {slug}
    fields of the \l Organization object are set to the values extracted from
    \a jsonOrg.
*/
Organization Organization::fromJson(const QJsonObject jsonOrg)
{
    Organization fromJson = Organization(
                jsonOrg["name"].toString(),
                jsonOrg["slug"].toString());

    return fromJson;
}

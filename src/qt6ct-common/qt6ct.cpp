/*
 * Copyright (c) 2020-2024, Ilya Kotov <forkotov02@ya.ru>
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include <QDir>
#include <QLocale>
#include <QLatin1String>
#include <QStandardPaths>
#include <QRegularExpression>
#include <QFile>
#include <QSettings>
#include <QtDebug>
#include "qt6ct.h"

#ifndef QT6CT_DATADIR
#define QT6CT_DATADIR "/usr/share"
#endif

QSet<Qt6CT::StyleInstance*> Qt6CT::styleInstances;

void Qt6CT::initConfig()
{
    if(QFile::exists(configFile()))
        return;

    QString globalConfig = QStandardPaths::locate(QStandardPaths::GenericConfigLocation, "qt6ct/qt6ct.conf");
    if(globalConfig.isEmpty())
        return;

    QDir("/").mkpath(configPath());
    QFile::copy(globalConfig, configFile());
}

QString Qt6CT::configPath()
{
    return QStandardPaths::writableLocation(QStandardPaths::ConfigLocation) + QLatin1String("/qt6ct");
}

QString Qt6CT::configFile()
{
    return configPath() + QLatin1String("/qt6ct.conf");
}

QStringList Qt6CT::iconPaths()
{
    QStringList paths = { QDir::homePath() + QLatin1String("/.icons") };

    for(const QString &p : QStandardPaths::standardLocations(QStandardPaths::GenericDataLocation))
    {
        paths << (p + QLatin1String("/icons"));
    }
    paths.removeDuplicates();

    //remove invalid
    QStringList::iterator it = paths.begin();
    while(it != paths.end())
    {
        if(QDir(*it).exists())
            ++it ;
        else
            it = paths.erase(it);
    }

    return paths;
}

QString Qt6CT::userStyleSheetPath()
{
    return configPath() + QLatin1String("/qss");
}

QStringList Qt6CT::sharedStyleSheetPaths()
{
    QStringList paths;
    for(const QString &p : QStandardPaths::standardLocations(QStandardPaths::GenericDataLocation))
    {
        paths << (p + QLatin1String("/qt6ct/qss"));
    }
    paths << QLatin1String(QT6CT_DATADIR"/qt6ct/qss");
    paths.removeDuplicates();
    return paths;
}

QString Qt6CT::userColorSchemePath()
{
    return configPath() + QLatin1String("/colors");
}

QStringList Qt6CT::sharedColorSchemePaths()
{
    QStringList paths;
    for(const QString &p : QStandardPaths::standardLocations(QStandardPaths::GenericDataLocation))
    {
        paths << (p + QLatin1String("/qt6ct/colors"));
    }
    paths << QLatin1String(QT6CT_DATADIR"/qt6ct/colors");
    paths.removeDuplicates();
    return paths;
}

QString Qt6CT::resolvePath(const QString &path)
{
    QString tmp = path;
    tmp.replace("~", QStandardPaths::writableLocation(QStandardPaths::HomeLocation));
    if(!tmp.contains("$"))
        return tmp;

    //find environment variables
    static const QRegularExpression regexp("\\$([A-Z_]+)\\/");
    QRegularExpressionMatchIterator it = regexp.globalMatch(tmp);

    while (it.hasNext())
    {
        QRegularExpressionMatch match = it.next();
        QString captured = match.captured(1);
        tmp.replace(QLatin1String("$") + captured, qgetenv(captured.toLatin1().constData()) );
    }

    return tmp;
}

QPalette Qt6CT::loadColorScheme(const QString &filePath, const QPalette &fallback)
{
    QPalette customPalette;
    QSettings settings(filePath, QSettings::IniFormat);
    settings.beginGroup("ColorScheme");
    QStringList activeColors = settings.value("active_colors").toStringList();
    QStringList inactiveColors = settings.value("inactive_colors").toStringList();
    QStringList disabledColors = settings.value("disabled_colors").toStringList();
    settings.endGroup();


#if (QT_VERSION >= QT_VERSION_CHECK(6,6,0))
    if(activeColors.count() == QPalette::Accent)
        activeColors << activeColors.at(QPalette::Highlight);
    if(inactiveColors.count() == QPalette::Accent)
        inactiveColors << inactiveColors.at(QPalette::Highlight);
    if(disabledColors.count() == QPalette::Accent)
        disabledColors << disabledColors.at(QPalette::Highlight);
#endif


    if(activeColors.count() >= QPalette::NColorRoles &&
            inactiveColors.count() >= QPalette::NColorRoles &&
            disabledColors.count() >= QPalette::NColorRoles)
    {
        for (int i = 0; i < QPalette::NColorRoles; i++)
        {
            QPalette::ColorRole role = QPalette::ColorRole(i);
            customPalette.setColor(QPalette::Active, role, QColor(activeColors.at(i)));
            customPalette.setColor(QPalette::Inactive, role, QColor(inactiveColors.at(i)));
            customPalette.setColor(QPalette::Disabled, role, QColor(disabledColors.at(i)));
        }
    }
    else
    {
        customPalette = fallback; //load fallback palette
    }

    return customPalette;
}

void Qt6CT::registerStyleInstance(Qt6CT::StyleInstance *instance)
{
    styleInstances.insert(instance);
}

void Qt6CT::unregisterStyleInstance(Qt6CT::StyleInstance *instance)
{
    styleInstances.remove(instance);
}

void Qt6CT::reloadStyleInstanceSettings()
{
    for(auto instance : qAsConst(styleInstances))
        instance->reloadSettings();
}

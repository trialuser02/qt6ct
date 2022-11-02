/*
 * Copyright (c) 2020-2022, Ilya Kotov <forkotov02@ya.ru>
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
#include <QtDebug>
#include "qt6ct.h"

#ifndef QT6CT_DATADIR
#define QT6CT_DATADIR "/usr/share"
#endif


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

QString Qt6CT::systemLanguageID()
{
#ifdef Q_OS_UNIX
    QByteArray v = qgetenv ("LC_ALL");
    if (v.isEmpty())
        v = qgetenv ("LC_MESSAGES");
    if (v.isEmpty())
        v = qgetenv ("LANG");
    if (!v.isEmpty())
        return QLocale (QString::fromLatin1(v)).name();
#endif
    return  QLocale::system().name();
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

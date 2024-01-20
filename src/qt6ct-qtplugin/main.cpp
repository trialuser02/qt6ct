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

#include <QLibraryInfo>
#include <qpa/qplatformthemeplugin.h>
#include "qt6ctplatformtheme.h"

QT_BEGIN_NAMESPACE

class Qt6CTPlatformThemePlugin: public QPlatformThemePlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.qt-project.Qt.QPA.QPlatformThemeFactoryInterface.5.1" FILE "qt6ct.json")
public:
    QPlatformTheme *create(const QString &key, const QStringList &params) override;
};

QPlatformTheme *Qt6CTPlatformThemePlugin::create(const QString &key, const QStringList &params)
{
    Q_UNUSED(params);
    QVersionNumber v = QLibraryInfo::version();
    if(v.majorVersion() != QT_VERSION_MAJOR || v.minorVersion() != QT_VERSION_MINOR)
    {
        qCCritical(lqt6ct) << "qt6ct is compiled against incompatible Qt version (" QT_VERSION_STR ").";
        return nullptr;
    }

    if(key.toLower() == QLatin1String("qt6ct") || key.toLower() == QLatin1String("qt5ct"))
        return new Qt6CTPlatformTheme();
    return nullptr;
}

QT_END_NAMESPACE

#include "main.moc"

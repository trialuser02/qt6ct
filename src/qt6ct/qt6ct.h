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

#ifndef QT6CT_H
#define QT6CT_H

#define QT6CT_VERSION_MAJOR 0
#define QT6CT_VERSION_MINOR 6

#define QT6CT_TOSTRING(s) #s
#define QT6CT_STRINGIFY(s)         QT6CT_TOSTRING(s)

#define QT6CT_VERSION_INT (QT6CT_VERSION_MAJOR<<8 | QT6CT_VERSION_MINOR)
#define QT6CT_VERSION_STR QT6CT_STRINGIFY(QT6CT_VERSION_MAJOR.QT6CT_VERSION_MINOR)

#include <QString>
#include <QStringList>

class Qt6CT
{
public:
    static void initConfig();
    static QString configPath();
    static QString configFile();
    static QStringList iconPaths();
    static QString userStyleSheetPath();
    static QStringList sharedStyleSheetPaths();
    static QString userColorSchemePath();
    static QStringList sharedColorSchemePaths();
    static QString systemLanguageID();
    static QString resolvePath(const QString &path);

private:
    Qt6CT() {}
};

#endif // QT6CT_H

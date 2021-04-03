/*
 * Copyright (c) 2020-2021, Ilya Kotov <forkotov02@ya.ru>
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

#include <QVariant>
#include <QSettings>
#include <QGuiApplication>
#include <QScreen>
#include <QFont>
#include <QPalette>
#include <QTimer>
#include <QIcon>
#include <QRegularExpression>
#include <QMimeDatabase>
#ifdef QT_WIDGETS_LIB
#include <QStyle>
#include <QStyleFactory>
#include <QApplication>
#include <QWidget>
#endif
#include <QFile>
#include <QFileSystemWatcher>
#include <private/qiconloader_p.h>

#include <qt6ct/qt6ct.h>
#include "qt6ctplatformtheme.h"

#include <QStringList>
#include <qpa/qplatformthemefactory_p.h>

Q_LOGGING_CATEGORY(lqt6ct, "qt6ct", QtWarningMsg)

//QT_QPA_PLATFORMTHEME=qt6ct

Qt6CTPlatformTheme::Qt6CTPlatformTheme()
{
    Qt6CT::initConfig();
    if(QGuiApplication::desktopSettingsAware())
    {
        readSettings();
        QMetaObject::invokeMethod(this, "applySettings", Qt::QueuedConnection);
#ifdef QT_WIDGETS_LIB
        QMetaObject::invokeMethod(this, "createFSWatcher", Qt::QueuedConnection);
#endif
        QGuiApplication::setFont(m_generalFont);
    }
    qCDebug(lqt6ct) << "using qt6ct plugin";
#ifdef QT_WIDGETS_LIB
    if(!QStyleFactory::keys().contains("qt6ct-style"))
        qCCritical(lqt6ct) << "unable to find qt6ct proxy style";
#endif
}

Qt6CTPlatformTheme::~Qt6CTPlatformTheme()
{
    if(m_palette)
        delete m_palette;

}

bool Qt6CTPlatformTheme::usePlatformNativeDialog(DialogType type) const
{
    return m_theme ? m_theme->usePlatformNativeDialog(type) :
                     QPlatformTheme::usePlatformNativeDialog(type);
}

QPlatformDialogHelper *Qt6CTPlatformTheme::createPlatformDialogHelper(DialogType type) const
{
    return m_theme ? m_theme->createPlatformDialogHelper(type) :
                     QPlatformTheme::createPlatformDialogHelper(type);
}

const QPalette *Qt6CTPlatformTheme::palette(QPlatformTheme::Palette type) const
{
    return (m_usePalette && m_palette) ? m_palette : QPlatformTheme::palette(type);
}

const QFont *Qt6CTPlatformTheme::font(QPlatformTheme::Font type) const
{
    if(type == QPlatformTheme::FixedFont)
        return &m_fixedFont;
    return &m_generalFont;
}

QVariant Qt6CTPlatformTheme::themeHint(QPlatformTheme::ThemeHint hint) const
{
    switch (hint)
    {
    case QPlatformTheme::CursorFlashTime:
        return m_cursorFlashTime;
    case MouseDoubleClickInterval:
        return m_doubleClickInterval;
    case QPlatformTheme::ToolButtonStyle:
        return m_toolButtonStyle;
    case QPlatformTheme::SystemIconThemeName:
        return m_iconTheme;
    case QPlatformTheme::StyleNames:
        return QStringList() << "qt6ct-style";
    case QPlatformTheme::IconThemeSearchPaths:
        return Qt6CT::iconPaths();
    case QPlatformTheme::DialogButtonBoxLayout:
        return m_buttonBoxLayout;
    case QPlatformTheme::KeyboardScheme:
        return m_keyboardScheme;
    case QPlatformTheme::UiEffects:
        return m_uiEffects;
    case QPlatformTheme::WheelScrollLines:
        return m_wheelScrollLines;
    case QPlatformTheme::ShowShortcutsInContextMenus:
        return m_showShortcutsInContextMenus;
    default:
        return QPlatformTheme::themeHint(hint);
    }
}

QIcon Qt6CTPlatformTheme::fileIcon(const QFileInfo &fileInfo, QPlatformTheme::IconOptions iconOptions) const
{
    if((iconOptions & DontUseCustomDirectoryIcons) && fileInfo.isDir())
        return QIcon::fromTheme(QLatin1String("inode-directory"));

    QMimeDatabase db;
    QMimeType type = db.mimeTypeForFile(fileInfo);
    return QIcon::fromTheme(type.iconName());
}

void Qt6CTPlatformTheme::applySettings()
{
    if(!QGuiApplication::desktopSettingsAware())
        return;

    if(!m_update)
    {
        //do not override application palette
        if(QCoreApplication::testAttribute(Qt::AA_SetPalette))
        {
            m_usePalette = false;
            qCDebug(lqt6ct) << "palette support is disabled";
        }
    }

#ifdef QT_WIDGETS_LIB
    if(hasWidgets())
    {
        qApp->setFont(m_generalFont);

        //Qt 5.6 or higher should be use themeHint function on application startup.
        //So, there is no need to call this function first time.
        if(m_update)
            qApp->setWheelScrollLines(m_wheelScrollLines);

        if(m_update && qApp->style()->objectName() == "qt6ct-style") //ignore application style
            qApp->setStyle("qt6ct-style"); //recreate style object

        if(!m_palette)
            m_palette = new QPalette(qApp->style()->standardPalette());

        if(m_update && m_usePalette)
            qApp->setPalette(*m_palette);


        //do not override application style
        if(m_prevStyleSheet == qApp->styleSheet())
            qApp->setStyleSheet(m_userStyleSheet);
        else
            qCDebug(lqt6ct) << "custom style sheet is disabled";
        m_prevStyleSheet = m_userStyleSheet;
    }
#endif
    QGuiApplication::setFont(m_generalFont); //apply font
    if(m_update)
        QIconLoader::instance()->updateSystemTheme(); //apply icons

    if(m_palette && m_usePalette)
        QGuiApplication::setPalette(*m_palette); //apply palette

#ifdef QT_WIDGETS_LIB
    if(m_palette && m_usePalette && !m_update)
        qApp->setPalette(*m_palette);

    if(hasWidgets() && m_update)
    {
        for(QWidget *w : qApp->allWidgets())
        {
            QEvent e(QEvent::ThemeChange);
            QApplication::sendEvent(w, &e);
            if(m_palette && m_usePalette)
                w->setPalette(*m_palette);
        }
    }
#endif

    m_update = true;
}

#ifdef QT_WIDGETS_LIB
void Qt6CTPlatformTheme::createFSWatcher()
{
    QFileSystemWatcher *watcher = new QFileSystemWatcher(this);
    watcher->addPath(Qt6CT::configPath());

    QTimer *timer = new QTimer(this);
    timer->setSingleShot(true);
    timer->setInterval(3000);
    connect(watcher, SIGNAL(directoryChanged(QString)), timer, SLOT(start()));
    connect(timer, SIGNAL(timeout()), SLOT(updateSettings()));
}

void Qt6CTPlatformTheme::updateSettings()
{
    qCDebug(lqt6ct) << "updating settings..";
    readSettings();
    applySettings();
}
#endif

void Qt6CTPlatformTheme::readSettings()
{
    if(m_palette)
    {
        delete m_palette;
        m_palette = nullptr;
    }

    QSettings settings(Qt6CT::configFile(), QSettings::IniFormat);

    settings.beginGroup("Appearance");
    m_style = settings.value("style", "Fusion").toString();
    QString schemePath = settings.value("color_scheme_path").toString();
    if(!schemePath.isEmpty() && settings.value("custom_palette", false).toBool())
    {
        schemePath = Qt6CT::resolvePath(schemePath); //replace environment variables
        m_palette = new QPalette(loadColorScheme(schemePath));
    }
    m_iconTheme = settings.value("icon_theme").toString();
    //load dialogs
    if(!m_update)
    {
        //do not mix gtk2 style and gtk3 dialogs
        QStringList keys = QPlatformThemeFactory::keys();
        QString name = settings.value("standard_dialogs", "default").toString();
        if((m_style == QLatin1String("gtk2") || m_style == QLatin1String("qt5gtk2")) &&
                (name == QLatin1String("gtk3") || name == QLatin1String("qt5gtk3")))
        {
            name = QLatin1String("gtk2");
        }
        if(keys.contains(name))
            m_theme.reset(QPlatformThemeFactory::create(name));
        else if(name == QLatin1String("gtk2") && keys.contains("qt5gtk2"))
            m_theme.reset(QPlatformThemeFactory::create("qt5gtk2"));
        else if(name == QLatin1String("gtk3") && keys.contains("qt5gtk3"))
            m_theme.reset(QPlatformThemeFactory::create("qt5gtk3"));
    }

    settings.endGroup();

    settings.beginGroup("Fonts");
    m_generalFont = settings.value("general", QPlatformTheme::font(QPlatformTheme::SystemFont)).value<QFont>();
    m_fixedFont = settings.value("fixed", QPlatformTheme::font(QPlatformTheme::FixedFont)).value<QFont>();
    settings.endGroup();

    settings.beginGroup("Interface");
    m_doubleClickInterval = QPlatformTheme::themeHint(QPlatformTheme::MouseDoubleClickInterval).toInt();
    m_doubleClickInterval = settings.value("double_click_interval", m_doubleClickInterval).toInt();
    m_cursorFlashTime = QPlatformTheme::themeHint(QPlatformTheme::CursorFlashTime).toInt();
    m_cursorFlashTime = settings.value("cursor_flash_time", m_cursorFlashTime).toInt();
    m_showShortcutsInContextMenus = settings.value("show_shortcuts_in_context_menus", true).toBool();
    m_buttonBoxLayout = QPlatformTheme::themeHint(QPlatformTheme::DialogButtonBoxLayout).toInt();
    m_buttonBoxLayout = settings.value("buttonbox_layout", m_buttonBoxLayout).toInt();
    m_keyboardScheme = QPlatformTheme::themeHint(QPlatformTheme::KeyboardScheme).toInt();
    m_keyboardScheme = settings.value("keyboard_scheme", m_keyboardScheme).toInt();
    QCoreApplication::setAttribute(Qt::AA_DontShowIconsInMenus, !settings.value("menus_have_icons", true).toBool());
    m_toolButtonStyle = settings.value("toolbutton_style", Qt::ToolButtonFollowStyle).toInt();
    m_wheelScrollLines = settings.value("wheel_scroll_lines", 3).toInt();

    //load effects
    m_uiEffects = QPlatformTheme::themeHint(QPlatformTheme::UiEffects).toInt();
    if(settings.childKeys().contains("gui_effects"))
    {
        QStringList effectList = settings.value("gui_effects").toStringList();
        m_uiEffects = 0;
        if(effectList.contains("General"))
            m_uiEffects |= QPlatformTheme::GeneralUiEffect;
        if(effectList.contains("AnimateMenu"))
            m_uiEffects |= QPlatformTheme::AnimateMenuUiEffect;
        if(effectList.contains("FadeMenu"))
            m_uiEffects |= QPlatformTheme::FadeMenuUiEffect;
        if(effectList.contains("AnimateCombo"))
            m_uiEffects |= QPlatformTheme::AnimateComboUiEffect;
        if(effectList.contains("AnimateTooltip"))
            m_uiEffects |= QPlatformTheme::AnimateTooltipUiEffect;
        if(effectList.contains("FadeTooltip"))
            m_uiEffects |= QPlatformTheme::FadeTooltipUiEffect;
        if(effectList.contains("AnimateToolBox"))
            m_uiEffects |= QPlatformTheme::AnimateToolBoxUiEffect;
    }

    //load style sheets
#ifdef QT_WIDGETS_LIB
    QStringList qssPaths = settings.value("stylesheets").toStringList();
    m_userStyleSheet = loadStyleSheets(qssPaths);
#endif

    settings.endGroup();
}

#ifdef QT_WIDGETS_LIB
bool Qt6CTPlatformTheme::hasWidgets()
{
    return qobject_cast<QApplication *> (qApp) != nullptr;
}
#endif

QString Qt6CTPlatformTheme::loadStyleSheets(const QStringList &paths)
{
    QString content;
    for(const QString &path : paths)
    {
        if(!QFile::exists(path))
            continue;

        QFile file(path);
        file.open(QIODevice::ReadOnly);
        content.append(QString::fromUtf8(file.readAll()));
    }
    QRegularExpression regExp("//.*(\\n|$)");
    content.remove(regExp);
    return content;
}

QPalette Qt6CTPlatformTheme::loadColorScheme(const QString &filePath)
{
    QPalette customPalette;
    QSettings settings(filePath, QSettings::IniFormat);
    settings.beginGroup("ColorScheme");
    QStringList activeColors = settings.value("active_colors").toStringList();
    QStringList inactiveColors = settings.value("inactive_colors").toStringList();
    QStringList disabledColors = settings.value("disabled_colors").toStringList();
    settings.endGroup();

    if(activeColors.count() == QPalette::NColorRoles &&
            inactiveColors.count() == QPalette::NColorRoles &&
            disabledColors.count() == QPalette::NColorRoles)
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
        customPalette = *QPlatformTheme::palette(SystemPalette); //load fallback palette
    }

    return customPalette;
}

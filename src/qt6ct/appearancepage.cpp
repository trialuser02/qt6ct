/*
 * Copyright (c) 2020-2023, Ilya Kotov <forkotov02@ya.ru>
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

#include <QStyleFactory>
#include <QMdiSubWindow>
#include <QSettings>
#include <QDir>
#include <QInputDialog>
#include <QMessageBox>
#include <QMenu>
#include <QIcon>
#include <QStringList>
#include <KSharedConfig>
#include <KConfigGroup>
#include <qpa/qplatformthemefactory_p.h>
#include "qt6ct.h"
#include "appearancepage.h"
#include "paletteeditdialog.h"
#include "ui_appearancepage.h"
#include "ui_previewform.h"

AppearancePage::AppearancePage(QWidget *parent) :
    TabPage(parent),
    m_ui(new Ui::AppearancePage)
{
    m_ui->setupUi(this);
    QStringList keys = QStyleFactory::keys();
    keys.removeAll("qt6ct-style"); //hide qt6ct proxy style
    keys.removeAll("qt5ct-style"); //hide qt5ct proxy style
    keys.removeAll("qt5gtk2"); //hide qt5gtk2 alias
    keys.removeAll("gtk2"); //hide gtk2 alias
    m_ui->styleComboBox->addItems(keys);

    connect(m_ui->paletteComboBox, SIGNAL(activated(int)), SLOT(updatePalette()));
    connect(m_ui->customPaletteButton, SIGNAL(clicked()), SLOT(updatePalette()));
    connect(m_ui->defaultPaletteButton, SIGNAL(clicked()), SLOT(updatePalette()));

    m_previewWidget = new QWidget(this);
    m_previewUi = new Ui::PreviewForm();
    m_previewUi->setupUi(m_previewWidget);
    QMdiSubWindow *w = m_ui->mdiArea->addSubWindow(m_previewWidget, Qt::SubWindow | Qt::CustomizeWindowHint
                                                   | Qt::WindowMinMaxButtonsHint
                                                   | Qt::WindowTitleHint
                                                   | Qt::WindowDoesNotAcceptFocus);
    w->setFocusPolicy(Qt::NoFocus);
    w->move(10, 10);

    QMenu *menu = new QMenu(this);
    menu->addAction(QIcon::fromTheme("document-new"), tr("Create"), this, SLOT(createColorScheme()));
    m_changeColorSchemeAction = menu->addAction(QIcon::fromTheme("accessories-text-editor"), tr("Edit"), this, SLOT(changeColorScheme()));
    menu->addAction(QIcon::fromTheme("edit-copy"), tr("Create a Copy"), this, SLOT(copyColorScheme()));
    m_renameColorSchemeAction = menu->addAction(tr("Rename"), this, SLOT(renameColorScheme()));
    menu->addSeparator();
    m_removeColorSchemeAction = menu->addAction(QIcon::fromTheme("edit-delete"), tr("Remove"), this, SLOT(removeColorScheme()));
    m_ui->colorSchemeButton->setMenu(menu);

    m_changeColorSchemeAction->setIcon(QIcon::fromTheme("accessories-text-editor"));
    m_removeColorSchemeAction->setIcon(QIcon::fromTheme("list-remove"));
    connect(menu, SIGNAL(aboutToShow()), SLOT(updateActions()));

    keys = QPlatformThemeFactory::keys();
    m_ui->dialogComboBox->addItem(tr("Default"), "default");
    if(keys.contains("gtk2") || keys.contains("qt6gtk2"))
        m_ui->dialogComboBox->addItem("GTK2", "gtk2");
    else if(keys.contains("gtk3") || keys.contains("qt6gtk3"))
        m_ui->dialogComboBox->addItem("GTK3", "gtk3");
    if(keys.contains("kde"))
        m_ui->dialogComboBox->addItem("KDE", "kde");
    if (keys.contains("xdgdesktopportal"))
        m_ui->dialogComboBox->addItem("XDG Desktop Portal", "xdgdesktopportal");

    readSettings();
}

AppearancePage::~AppearancePage()
{
    delete m_selectedStyle;
    delete m_ui;
    delete m_previewUi;
}

void AppearancePage::writeSettings(QSettings *settings)
{
    settings->beginGroup("Appearance");
    settings->setValue("style", m_ui->styleComboBox->currentText());
    settings->setValue("custom_palette", m_ui->customPaletteButton->isChecked());
    settings->setValue("color_scheme_path", m_ui->colorSchemeComboBox->currentData().toString());
    settings->setValue("standard_dialogs", m_ui->dialogComboBox->currentData().toString());
    settings->endGroup();
}

void AppearancePage::on_styleComboBox_textActivated(const QString &text)
{
    QStyle *style = QStyleFactory::create(text);
    if(!style)
        return;
    setStyle(m_previewWidget, style);

    delete m_selectedStyle;
    m_selectedStyle = style;

    updatePalette();
}

void AppearancePage::on_colorSchemeComboBox_activated(int)
{
    m_customPalette = Qt6CT::loadColorScheme(m_ui->colorSchemeComboBox->currentData().toString(), palette());
    updatePalette();
}

void AppearancePage::createColorScheme()
{
    QString name = QInputDialog::getText(this, tr("Enter Color Scheme Name"), tr("File name:"));
    if(name.isEmpty())
        return;

    if(!name.endsWith(".conf", Qt::CaseInsensitive))
        name.append(".conf");

    if(m_ui->colorSchemeComboBox->findText(name.section('.',0,0)) != -1)
    {
        QMessageBox::warning(this, tr("Error"), tr("The color scheme \"%1\" already exists")
                             .arg(name.section('.',0,0)));
        return;
    }

    QString schemePath = Qt6CT::userColorSchemePath() + QLatin1String("/") + name;

    createColorScheme(schemePath, palette());
    m_ui->colorSchemeComboBox->addItem(name.section('.',0,0), schemePath);
}

void AppearancePage::changeColorScheme()
{
    if(m_ui->colorSchemeComboBox->currentIndex() < 0)
        return;

    if(!QFileInfo(m_ui->colorSchemeComboBox->currentData().toString()).isWritable())
    {
        QMessageBox::information(this, tr("Warning"), tr("The color scheme \"%1\" is read only")
                                 .arg(m_ui->colorSchemeComboBox->currentText()));
        return;
    }

    PaletteEditDialog d(m_customPalette, m_selectedStyle, this);
    connect(&d, SIGNAL(paletteChanged(QPalette)), SLOT(setPreviewPalette(QPalette)));
    if(d.exec() == QDialog::Accepted)
    {
        m_customPalette = d.selectedPalette();
        createColorScheme(m_ui->colorSchemeComboBox->currentData().toString(), m_customPalette);
    }
    updatePalette();
}

void AppearancePage::removeColorScheme()
{
    int index = m_ui->colorSchemeComboBox->currentIndex();
    if(index < 0 || m_ui->colorSchemeComboBox->count() <= 1)
        return;

    if(!QFileInfo(m_ui->colorSchemeComboBox->currentData().toString()).isWritable())
    {
        QMessageBox::information(this, tr("Warning"), tr("The color scheme \"%1\" is read only")
                                 .arg(m_ui->colorSchemeComboBox->currentText()));
        return;
    }

    int button = QMessageBox::question(this, tr("Confirm Remove"),
                                       tr("Are you sure you want to remove color scheme \"%1\"?")
                                       .arg(m_ui->colorSchemeComboBox->currentText()),
                                       QMessageBox::Yes | QMessageBox::No);
    if(button != QMessageBox::Yes)
        return;

    if(QFile::remove(m_ui->colorSchemeComboBox->currentData().toString()))
    {
        m_ui->colorSchemeComboBox->removeItem(index);
        on_colorSchemeComboBox_activated(0);
    }
}

void AppearancePage::copyColorScheme()
{
    if(m_ui->colorSchemeComboBox->currentIndex() < 0)
        return;

    QString name = QInputDialog::getText(this, tr("Enter Color Scheme Name"), tr("File name:"),
                                         QLineEdit::Normal,
                                         tr("%1 (copy)").arg(m_ui->colorSchemeComboBox->currentText()));
    if(name.isEmpty() || name == m_ui->colorSchemeComboBox->currentText())
        return;

    if(!name.endsWith(".conf", Qt::CaseInsensitive))
        name.append(".conf");

    if(m_ui->colorSchemeComboBox->findText(name.section('.',0,0)) != -1)
    {
        QMessageBox::warning(this, tr("Error"), tr("The color scheme \"%1\" already exists")
                             .arg(name.section('.',0,0)));
        return;
    }

    QString newPath =  Qt6CT::userColorSchemePath() + QLatin1String("/") + name;
    if(!QFile::copy(m_ui->colorSchemeComboBox->currentData().toString(), newPath))
    {
        QMessageBox::warning(this, tr("Error"), tr("Unable to copy file"));
        return;
    }
    m_ui->colorSchemeComboBox->addItem(name.section('.',0,0), newPath);
}

void AppearancePage::renameColorScheme()
{
    int index = m_ui->colorSchemeComboBox->currentIndex();

    if(index < 0)
        return;

    if(!QFileInfo(m_ui->colorSchemeComboBox->currentData().toString()).isWritable())
    {
        QMessageBox::information(this, tr("Warning"), tr("The color scheme \"%1\" is read only")
                                 .arg(m_ui->colorSchemeComboBox->currentText()));
        return;
    }

    QString name = QInputDialog::getText(this, tr("Enter Color Scheme Name"), tr("File name:"),
                                         QLineEdit::Normal, m_ui->colorSchemeComboBox->currentText());
    if(name.isEmpty() || name == m_ui->colorSchemeComboBox->currentText())
        return;

    if(!name.endsWith(".conf", Qt::CaseInsensitive))
        name.append(".conf");

    if(m_ui->colorSchemeComboBox->findText(name.section('.',0,0)) != -1)
    {
        QMessageBox::warning(this, tr("Error"), tr("The color scheme \"%1\" already exists")
                             .arg(name.section('.',0,0)));
        return;
    }

    QString newPath =  Qt6CT::userColorSchemePath() + QLatin1String("/") + name;
    QFile::rename(m_ui->colorSchemeComboBox->currentData().toString(), newPath);
    m_ui->colorSchemeComboBox->setItemText(index, name.section('.',0,0));
    m_ui->colorSchemeComboBox->setItemData(index, newPath);
}

void AppearancePage::updatePalette()
{
    if(!m_selectedStyle)
        return;

    setPreviewPalette(m_ui->customPaletteButton->isChecked() ?
                          m_customPalette : m_selectedStyle->standardPalette());
}

void AppearancePage::setPreviewPalette(const QPalette &p)
{
    QPalette previewPalette = palette();

    QPalette::ColorGroup colorGroup = QPalette::Disabled;

    if(m_ui->paletteComboBox->currentIndex() == 0)
    {
        colorGroup = QPalette::Active;
    }
    else if(m_ui->paletteComboBox->currentIndex() == 1)
    {
        colorGroup = QPalette::Inactive;
    }

    for (int i = 0; i < QPalette::NColorRoles; i++)
    {
        QPalette::ColorRole role = QPalette::ColorRole(i);
        previewPalette.setColor(QPalette::Active, role, p.color(colorGroup, role));
        previewPalette.setColor(QPalette::Inactive, role, p.color(colorGroup, role));
    }

    setPalette(m_ui->mdiArea, previewPalette);
}

void AppearancePage::updateActions()
{
    if(m_ui->colorSchemeComboBox->count() == 0 ||
            !QFileInfo(m_ui->colorSchemeComboBox->currentData().toString()).isWritable())
    {
        m_changeColorSchemeAction->setVisible(false);
        m_renameColorSchemeAction->setVisible(false);
        m_removeColorSchemeAction->setVisible(false);
    }
    else
    {
        m_changeColorSchemeAction->setVisible(true);
        m_renameColorSchemeAction->setVisible(true);
        m_removeColorSchemeAction->setVisible(m_ui->colorSchemeComboBox->count() > 1);
    }
}

void AppearancePage::readSettings()
{
    QSettings settings(Qt6CT::configFile(), QSettings::IniFormat);
    settings.beginGroup("Appearance");
    QString style = settings.value("style", "Fusion").toString();
    m_ui->styleComboBox->setCurrentText(style);

    m_ui->customPaletteButton->setChecked(settings.value("custom_palette", false).toBool());
    QString colorSchemePath = settings.value("color_scheme_path").toString();
    colorSchemePath = Qt6CT::resolvePath(colorSchemePath); //replace environment variables

    QDir("/").mkpath(Qt6CT::userColorSchemePath());
    findColorSchemes(Qt6CT::userColorSchemePath());
    findColorSchemes(Qt6CT::sharedColorSchemePaths());

    if(m_ui->colorSchemeComboBox->count() == 0)
    {
        m_customPalette = palette(); //load fallback palette
    }
    else
    {
        int index = m_ui->colorSchemeComboBox->findData(colorSchemePath);
        if(index >= 0)
            m_ui->colorSchemeComboBox->setCurrentIndex(index);
        m_customPalette = Qt6CT::loadColorScheme(m_ui->colorSchemeComboBox->currentData().toString(), palette());
    }

    on_styleComboBox_textActivated(m_ui->styleComboBox->currentText());

    int index = m_ui->dialogComboBox->findData(settings.value("standard_dialogs").toString());
    m_ui->dialogComboBox->setCurrentIndex(qMax(index, 0));

    settings.endGroup();
}

void AppearancePage::setStyle(QWidget *w, QStyle *s)
{
    for(QObject *o : w->children())
    {
        if(o->isWidgetType())
        {
            setStyle(qobject_cast<QWidget *>(o), s);
        }
    }
    w->setStyle(s);
}

void AppearancePage::setPalette(QWidget *w, QPalette p)
{
    for(QObject *o : w->children())
    {
        if(o->isWidgetType())
        {
            setPalette(qobject_cast<QWidget *>(o), p);
        }
    }
    w->setPalette(p);
}

void AppearancePage::findColorSchemes(const QString &path)
{
    QDir dir(path);
    dir.setFilter(QDir::Files);
    dir.setNameFilters(QStringList() << "*.conf" << "*.colors");

    for(const QFileInfo &info : dir.entryInfoList())
    {
        QString name;
        QString path = info.filePath();
        if(info.suffix() == "colors") {
            KSharedConfigPtr config = KSharedConfig::openConfig(path, KConfig::SimpleConfig);
            KConfigGroup group(config, "General");
            name = group.readEntry("Name", info.baseName()) + " (KColorScheme)";
        }
        else
        {
            name = info.baseName();
        }
        m_ui->colorSchemeComboBox->addItem(name, path);
    }
}

void AppearancePage::findColorSchemes(const QStringList &paths)
{
    for(const QString &p : paths)
        findColorSchemes(p);
}

void AppearancePage::createColorScheme(const QString &name, const QPalette &palette)
{
    QSettings settings(name, QSettings::IniFormat);
    settings.beginGroup("ColorScheme");

    QStringList activeColors, inactiveColors, disabledColors;
    for (int i = 0; i < QPalette::NColorRoles; i++)
    {
        QPalette::ColorRole role = QPalette::ColorRole(i);
        activeColors << palette.color(QPalette::Active, role).name(QColor::HexArgb);
        inactiveColors << palette.color(QPalette::Inactive, role).name(QColor::HexArgb);
        disabledColors << palette.color(QPalette::Disabled, role).name(QColor::HexArgb);
    }

    settings.setValue("active_colors",activeColors);
    settings.setValue("inactive_colors",inactiveColors);
    settings.setValue("disabled_colors",disabledColors);

    settings.endGroup();
}

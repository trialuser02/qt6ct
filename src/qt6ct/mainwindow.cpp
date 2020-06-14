/*
 * Copyright (c) 2014-2020, Ilya Kotov <forkotov02@ya.ru>
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

#include <QApplication>
#include <QSettings>
#include <QStyle>
#include <QProcessEnvironment>
#include <QMessageBox>
#include <QStyleFactory>
#include "qt6ct.h"
#include "mainwindow.h"
#include "appearancepage.h"
#include "fontspage.h"
#include "iconthemepage.h"
#include "interfacepage.h"
#include "qsspage.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QWidget(parent),
    m_ui(new Ui::MainWindow)
{
    m_ui->setupUi(this);
    m_ui->tabWidget->addTab(new AppearancePage(this), tr("Appearance"));
    m_ui->tabWidget->addTab(new FontsPage(this), tr("Fonts"));
    m_ui->tabWidget->addTab(new IconThemePage(this), tr("Icon Theme"));
    m_ui->tabWidget->addTab(new InterfacePage(this), tr("Interface"));
#ifdef USE_WIDGETS
    m_ui->tabWidget->addTab(new QSSPage(this), tr("Style Sheets"));
#endif

    QSettings settings(Qt6CT::configFile(), QSettings::IniFormat);
    restoreGeometry(settings.value("SettingsWindow/geometry").toByteArray());

    setWindowIcon(QIcon::fromTheme("preferences-desktop-theme"));

    m_ui->versionLabel->setText(tr("Version: %1").arg(QT6CT_VERSION_STR));
    m_ui->warningIconLabel->setPixmap(qApp->style()->standardIcon(QStyle::SP_MessageBoxWarning).pixmap(16, 16));

    checkConfiguration();
    m_ui->warningFrame->setVisible(!m_errors.isEmpty());
}

MainWindow::~MainWindow()
{
    delete m_ui;
}

void MainWindow::showEvent(QShowEvent *)
{
    m_ui->tabWidget->setFocus();
}

void MainWindow::closeEvent(QCloseEvent *)
{
    QSettings settings(Qt6CT::configFile(), QSettings::IniFormat);
    settings.setValue("SettingsWindow/geometry", saveGeometry());
}

void MainWindow::on_buttonBox_clicked(QAbstractButton *button)
{
    int id = m_ui->buttonBox->standardButton(button);
    if(id == QDialogButtonBox::Ok || id == QDialogButtonBox::Apply)
    {
        for(int i = 0; i < m_ui->tabWidget->count(); ++i)
        {
            TabPage *p = qobject_cast<TabPage*>(m_ui->tabWidget->widget(i));
            if(p)
                p->writeSettings();
        }
    }

    if(id == QDialogButtonBox::Ok || id == QDialogButtonBox::Cancel)
    {
        close();
        qApp->quit();
    }
}

void MainWindow::on_infoButton_clicked()
{
    QMessageBox::warning(this, tr("Warning"), m_errors.join("<br><br>"));
}

void MainWindow::checkConfiguration()
{
    QProcessEnvironment env = QProcessEnvironment::systemEnvironment();

    if(env.contains("QT_STYLE_OVERRIDE"))
    {
        m_errors << tr("Please remove the <b>QT_STYLE_OVERRIDE</b> environment variable (current value: <b>%1</b>).")
                    .arg(env.value("QT_STYLE_OVERRIDE"));
    }

    if(!env.contains("QT_QPA_PLATFORMTHEME"))
    {
        m_errors << tr("The <b>QT_QPA_PLATFORMTHEME</b> environment variable is not set (required value: <b>qt6ct</b>).");
    }
    else if(env.value("QT_QPA_PLATFORMTHEME") != "qt6ct")
    {
        m_errors << tr("The <b>QT_QPA_PLATFORMTHEME</b> environment variable is not set correctly "
                       "(current value: <b>%1</b>, required value: <b>qt6ct</b>).")
                    .arg(env.value("QT_QPA_PLATFORMTHEME"));
    }

    if(!QStyleFactory::keys().contains("qt6ct-style"))
    {
        m_errors << tr("Unable to find <b>libqt6ct-style.so</b>");
    }
}

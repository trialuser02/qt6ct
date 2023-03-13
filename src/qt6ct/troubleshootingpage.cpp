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

#include <QFileDialog>
#include <QSettings>
#include "qt6ct.h"
#include "ui_troubleshootingpage.h"
#include "troubleshootingpage.h"

TroubleshootingPage::TroubleshootingPage(QWidget *parent) :
    TabPage(parent),
    m_ui(new Ui::TroubleshootingPage)
{
    m_ui->setupUi(this);
    readSettings();
}

TroubleshootingPage::~TroubleshootingPage()
{
    delete m_ui;
}

void TroubleshootingPage::writeSettings(QSettings *settings)
{
    settings->beginGroup("Troubleshooting");

    QStringList ignoredApps;
    for(int i = 0; i < m_ui->ignoredAppsListWidget->count(); ++i)
        ignoredApps << m_ui->ignoredAppsListWidget->item(i)->text();

    settings->setValue("ignored_applications", ignoredApps);
    settings->setValue("force_raster_widgets", m_ui->forceRasterCheckBox->checkState());
    settings->endGroup();
}

void TroubleshootingPage::on_addAppButton_clicked()
{
    QString path = QFileDialog::getOpenFileName(this, tr("Select Application"), "/usr/bin", tr("Executable files (*)"));
    if(!path.isEmpty())
        m_ui->ignoredAppsListWidget->addItem(path);
}

void TroubleshootingPage::on_removeAppButton_clicked()
{
    delete m_ui->ignoredAppsListWidget->currentItem();
}

void TroubleshootingPage::readSettings()
{
    m_ui->ignoredAppsListWidget->clear();
    QSettings settings(Qt6CT::configFile(), QSettings::IniFormat);
    settings.beginGroup("Troubleshooting");
    m_ui->ignoredAppsListWidget->addItems(settings.value("ignored_applications").toStringList());
    m_ui->forceRasterCheckBox->setCheckState(Qt::CheckState(settings.value("force_raster_widgets", Qt::PartiallyChecked).toInt()));
    settings.endGroup();
}

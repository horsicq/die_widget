/* Copyright (c) 2019-2025 hors<horsicq@gmail.com>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */
#include "dieoptionswidget.h"

#include "ui_dieoptionswidget.h"

DIEOptionsWidget::DIEOptionsWidget(QWidget *pParent) : XShortcutsWidget(pParent), ui(new Ui::DIEOptionsWidget)
{
    ui->setupUi(this);

#ifndef USE_YARA
    ui->groupBoxYaraRules->hide();
#endif
}

DIEOptionsWidget::~DIEOptionsWidget()
{
    delete ui;
}

void DIEOptionsWidget::adjustView()
{
    // TODO
}

void DIEOptionsWidget::setOptions(XOptions *pOptions)
{
    m_pOptions = pOptions;

    reload();
}

void DIEOptionsWidget::save()
{
    m_pOptions->getCheckBox(ui->checkBoxDeepScan, XOptions::ID_SCAN_FLAG_DEEP);
    m_pOptions->getCheckBox(ui->checkBoxScanAfterOpen, XOptions::ID_SCAN_SCANAFTEROPEN);
    m_pOptions->getCheckBox(ui->checkBoxRecursiveScan, XOptions::ID_SCAN_FLAG_RECURSIVE);
    m_pOptions->getCheckBox(ui->checkBoxHeuristicScan, XOptions::ID_SCAN_FLAG_HEURISTIC);
    m_pOptions->getCheckBox(ui->checkBoxAggressiveScan, XOptions::ID_SCAN_FLAG_AGGRESSIVE);
    m_pOptions->getCheckBox(ui->checkBoxVerbose, XOptions::ID_SCAN_FLAG_VERBOSE);
    m_pOptions->getCheckBox(ui->checkBoxAllTypesScan, XOptions::ID_SCAN_FLAG_ALLTYPES);
    m_pOptions->getCheckBox(ui->checkBoxFormatResult, XOptions::ID_SCAN_FORMATRESULT);
    m_pOptions->getCheckBox(ui->checkBoxProfiling, XOptions::ID_SCAN_LOG_PROFILING);
    m_pOptions->getCheckBox(ui->checkBoxHighlight, XOptions::ID_SCAN_HIGHLIGHT);
    m_pOptions->getCheckBox(ui->checkBoxSort, XOptions::ID_SCAN_SORT);
    m_pOptions->getCheckBox(ui->checkBoxHideUnknown, XOptions::ID_SCAN_HIDEUNKNOWN);
    m_pOptions->getLineEdit(ui->lineEditDIEDatabase, XOptions::ID_SCAN_DATABASE_MAIN_PATH);
    m_pOptions->getLineEdit(ui->lineEditDIEDatabaseExtra, XOptions::ID_SCAN_DATABASE_EXTRA_PATH);
    m_pOptions->getLineEdit(ui->lineEditDIEDatabaseCustom, XOptions::ID_SCAN_DATABASE_CUSTOM_PATH);
    m_pOptions->getCheckBox(ui->groupBoxDIEDatabaseExtra, XOptions::ID_SCAN_DATABASE_EXTRA_ENABLED);
    m_pOptions->getCheckBox(ui->groupBoxDIEDatabaseCustom, XOptions::ID_SCAN_DATABASE_CUSTOM_ENABLED);
#ifdef USE_YARA
    if (m_pOptions->isIDPresent(XOptions::ID_SCAN_YARARULESPATH)) {
        m_pOptions->getLineEdit(ui->lineEditYaraRules, XOptions::ID_SCAN_YARARULESPATH);
    }
#endif
    if (m_pOptions->isIDPresent(XOptions::ID_SCAN_ENGINE)) {
        m_pOptions->getComboBox(ui->comboBoxScanEngine, XOptions::ID_SCAN_ENGINE);
    } else if (m_pOptions->isIDPresent(XOptions::ID_SCAN_ENGINE_EMPTY)) {
        m_pOptions->getComboBox(ui->comboBoxScanEngine, XOptions::ID_SCAN_ENGINE_EMPTY);
    }
}

void DIEOptionsWidget::reload()
{
    m_pOptions->setCheckBox(ui->checkBoxScanAfterOpen, XOptions::ID_SCAN_SCANAFTEROPEN);
    m_pOptions->setCheckBox(ui->checkBoxRecursiveScan, XOptions::ID_SCAN_FLAG_RECURSIVE);
    m_pOptions->setCheckBox(ui->checkBoxDeepScan, XOptions::ID_SCAN_FLAG_DEEP);
    m_pOptions->setCheckBox(ui->checkBoxHeuristicScan, XOptions::ID_SCAN_FLAG_HEURISTIC);
    m_pOptions->setCheckBox(ui->checkBoxAggressiveScan, XOptions::ID_SCAN_FLAG_AGGRESSIVE);
    m_pOptions->setCheckBox(ui->checkBoxVerbose, XOptions::ID_SCAN_FLAG_VERBOSE);
    m_pOptions->setCheckBox(ui->checkBoxFormatResult, XOptions::ID_SCAN_FORMATRESULT);
    m_pOptions->setCheckBox(ui->checkBoxAllTypesScan, XOptions::ID_SCAN_FLAG_ALLTYPES);
    m_pOptions->setCheckBox(ui->checkBoxHighlight, XOptions::ID_SCAN_HIGHLIGHT);
    m_pOptions->setCheckBox(ui->checkBoxSort, XOptions::ID_SCAN_SORT);
    m_pOptions->setCheckBox(ui->checkBoxHideUnknown, XOptions::ID_SCAN_HIDEUNKNOWN);
    m_pOptions->setCheckBox(ui->checkBoxProfiling, XOptions::ID_SCAN_LOG_PROFILING);
    m_pOptions->setLineEdit(ui->lineEditDIEDatabase, XOptions::ID_SCAN_DATABASE_MAIN_PATH);
    m_pOptions->setLineEdit(ui->lineEditDIEDatabaseExtra, XOptions::ID_SCAN_DATABASE_EXTRA_PATH);
    m_pOptions->setLineEdit(ui->lineEditDIEDatabaseCustom, XOptions::ID_SCAN_DATABASE_CUSTOM_PATH);
    m_pOptions->setLineEdit(ui->lineEditYaraRules, XOptions::ID_SCAN_YARARULESPATH);
    m_pOptions->setCheckBox(ui->groupBoxDIEDatabaseExtra, XOptions::ID_SCAN_DATABASE_EXTRA_ENABLED);
    m_pOptions->setCheckBox(ui->groupBoxDIEDatabaseCustom, XOptions::ID_SCAN_DATABASE_CUSTOM_ENABLED);

    if (m_pOptions->isIDPresent(XOptions::ID_SCAN_YARARULESPATH)) {
        ui->groupBoxYaraRules->show();
        m_pOptions->getLineEdit(ui->lineEditYaraRules, XOptions::ID_SCAN_YARARULESPATH);
    } else {
        ui->groupBoxYaraRules->hide();
    }

    if (m_pOptions->isIDPresent(XOptions::ID_SCAN_ENGINE)) {
        ui->groupBoxScanEngine->show();
        m_pOptions->setComboBox(ui->comboBoxScanEngine, XOptions::ID_SCAN_ENGINE);
    } else if (m_pOptions->isIDPresent(XOptions::ID_SCAN_ENGINE_EMPTY)) {
        ui->groupBoxScanEngine->show();
        m_pOptions->setComboBox(ui->comboBoxScanEngine, XOptions::ID_SCAN_ENGINE_EMPTY);
    } else {
        ui->groupBoxScanEngine->hide();
    }
}

void DIEOptionsWidget::setDefaultValues(XOptions *pOptions)
{
    pOptions->addID(XOptions::ID_SCAN_SCANAFTEROPEN, true);
    pOptions->addID(XOptions::ID_SCAN_FLAG_RECURSIVE, true);
    pOptions->addID(XOptions::ID_SCAN_FLAG_DEEP, true);
    pOptions->addID(XOptions::ID_SCAN_FLAG_HEURISTIC, true);
    pOptions->addID(XOptions::ID_SCAN_FLAG_AGGRESSIVE, false);
    pOptions->addID(XOptions::ID_SCAN_FLAG_VERBOSE, true);
    pOptions->addID(XOptions::ID_SCAN_FLAG_ALLTYPES, false);
    pOptions->addID(XOptions::ID_SCAN_FORMATRESULT, true);
    pOptions->addID(XOptions::ID_SCAN_LOG_PROFILING, false);
    pOptions->addID(XOptions::ID_SCAN_HIGHLIGHT, true);
    pOptions->addID(XOptions::ID_SCAN_SORT, true);
    pOptions->addID(XOptions::ID_SCAN_HIDEUNKNOWN, false);
    pOptions->addID(XOptions::ID_SCAN_DATABASE_MAIN_PATH, "$data/db");
    pOptions->addID(XOptions::ID_SCAN_DATABASE_EXTRA_PATH, "$data/db_extra");
    pOptions->addID(XOptions::ID_SCAN_DATABASE_CUSTOM_PATH, "$data/db_custom");
    pOptions->addID(XOptions::ID_SCAN_DATABASE_EXTRA_ENABLED, true);
    pOptions->addID(XOptions::ID_SCAN_DATABASE_CUSTOM_ENABLED, true);
    pOptions->addID(XOptions::ID_ENGINE_BUFFERSIZE, 2 * 1024 * 1024);  // Obsolete TODO remove
}

void DIEOptionsWidget::reloadData(bool bSaveSelection)
{
    Q_UNUSED(bSaveSelection)

    reload();
}

void DIEOptionsWidget::on_toolButtonDIEDatabase_clicked()
{
    QString sText = ui->lineEditDIEDatabase->text();
    QString sInitDirectory = XBinary::convertPathName(sText);

    QString sDirectoryName = QFileDialog::getExistingDirectory(this, tr("Open directory") + QString("..."), sInitDirectory, QFileDialog::ShowDirsOnly);

    if (!sDirectoryName.isEmpty()) {
        ui->lineEditDIEDatabase->setText(sDirectoryName);
    }
}

void DIEOptionsWidget::on_toolButtonDIEDatabaseExtra_clicked()
{
    QString sText = ui->lineEditDIEDatabaseExtra->text();
    QString sInitDirectory = XBinary::convertPathName(sText);

    QString sDirectoryName = QFileDialog::getExistingDirectory(this, tr("Open directory") + QString("..."), sInitDirectory, QFileDialog::ShowDirsOnly);

    if (!sDirectoryName.isEmpty()) {
        ui->lineEditDIEDatabaseExtra->setText(sDirectoryName);
    }
}

void DIEOptionsWidget::on_toolButtonDIEDatabaseCustom_clicked()
{
    QString sText = ui->lineEditDIEDatabaseCustom->text();
    QString sInitDirectory = XBinary::convertPathName(sText);

    QString sDirectoryName = QFileDialog::getExistingDirectory(this, tr("Open directory") + QString("..."), sInitDirectory, QFileDialog::ShowDirsOnly);

    if (!sDirectoryName.isEmpty()) {
        ui->lineEditDIEDatabaseCustom->setText(sDirectoryName);
    }
}

void DIEOptionsWidget::on_toolButtonYaraRules_clicked()
{
    QString sText = ui->lineEditYaraRules->text();
    QString sInitDirectory = XBinary::convertPathName(sText);

    QString sDirectoryName = QFileDialog::getExistingDirectory(this, tr("Open directory") + QString("..."), sInitDirectory, QFileDialog::ShowDirsOnly);

    if (!sDirectoryName.isEmpty()) {
        ui->lineEditYaraRules->setText(sDirectoryName);
    }
}

void DIEOptionsWidget::registerShortcuts(bool bState)
{
    Q_UNUSED(bState)
}

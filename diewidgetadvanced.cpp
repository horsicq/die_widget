/* Copyright (c) 2019-2024 hors<horsicq@gmail.com>
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
#include "diewidgetadvanced.h"
#include "ui_diewidgetadvanced.h"

DIEWidgetAdvanced::DIEWidgetAdvanced(QWidget *pParent) : XShortcutsWidget(pParent), ui(new Ui::DIEWidgetAdvanced)
{
    ui->setupUi(this);

    g_scanResult = {};
    g_pDevice = nullptr;
    g_pModel = nullptr;
}

DIEWidgetAdvanced::~DIEWidgetAdvanced()
{
    delete ui;
}

void DIEWidgetAdvanced::setData(QIODevice *pDevice, bool bScan, XBinary::FT fileType)
{
    g_pDevice = pDevice;

    XFormats::setFileTypeComboBox(fileType, pDevice, ui->comboBoxType, XBinary::TL_OPTION_ALL);

    if (bScan) {
        process();
    }
}

void DIEWidgetAdvanced::adjustView()
{
    QFont _font;
    QString sFont = getGlobalOptions()->getValue(XOptions::ID_SCAN_EDITORFONT).toString();

    if ((sFont != "") && _font.fromString(sFont)) {
        ui->plainTextEditSignature->setFont(_font);
    }
}

void DIEWidgetAdvanced::setGlobal(XShortcuts *pShortcuts, XOptions *pXOptions)
{
    ui->checkBoxAllTypesScan->setChecked(pXOptions->getValue(XOptions::ID_SCAN_ALLTYPES).toBool());
    ui->checkBoxDeepScan->setChecked(pXOptions->getValue(XOptions::ID_SCAN_DEEP).toBool());
    ui->checkBoxRecursiveScan->setChecked(pXOptions->getValue(XOptions::ID_SCAN_RECURSIVE).toBool());
    ui->checkBoxHeuristicScan->setChecked(pXOptions->getValue(XOptions::ID_SCAN_HEURISTIC).toBool());
    ui->checkBoxVerbose->setChecked(pXOptions->getValue(XOptions::ID_SCAN_VERBOSE).toBool());

    XShortcutsWidget::setGlobal(pShortcuts, pXOptions);
}

void DIEWidgetAdvanced::registerShortcuts(bool bState)
{
    Q_UNUSED(bState)
    // TODO
}

void DIEWidgetAdvanced::process()
{
    DiE_Script dieScript;

    dieScript.loadDatabase(getGlobalOptions()->getValue(XOptions::ID_SCAN_DATABASEPATH).toString(), true);  // TODO optimize
    dieScript.loadDatabase(getGlobalOptions()->getValue(XOptions::ID_SCAN_CUSTOMDATABASEPATH).toString(), false);

    DiE_Script::OPTIONS options = {};
    options.bShowType = true;
    options.bShowVersion = true;
    options.bShowOptions = true;
    options.bIsRecursiveScan = ui->checkBoxRecursiveScan->isChecked();
    options.bIsDeepScan = ui->checkBoxDeepScan->isChecked();
    options.bIsHeuristicScan = ui->checkBoxHeuristicScan->isChecked();
    options.bIsVerbose = ui->checkBoxVerbose->isChecked();
    options.bAllTypesScan = ui->checkBoxAllTypesScan->isChecked();
    options.bIsProfiling = false;
    options.bShowScanTime = false;
    options.fileType = (XBinary::FT)(ui->comboBoxType->currentData().toInt());

    DialogDIEScanProcess ds(this, &dieScript);
    ds.setData(g_pDevice, options);
    ds.exec();

    DiE_Script::SCAN_RESULT scanResult = dieScript.getScanResultProcess();

    // QAbstractItemModel *pOldModel = ui->treeViewResult->model();
    ScanItemModel *pOldModel = g_pModel;

    g_pModel = new ScanItemModel(&(scanResult.listRecords), 1, getGlobalOptions()->getValue(XOptions::ID_SCAN_HIGHLIGHT).toBool());
    ui->treeViewResult->setModel(g_pModel);
    ui->treeViewResult->expandAll();

    connect(ui->treeViewResult->selectionModel(), SIGNAL(selectionChanged(QItemSelection, QItemSelection)), SLOT(onSelectionChanged(QItemSelection, QItemSelection)));

    // deleteOldAbstractModel(&pOldModel);
    delete pOldModel;
}

void DIEWidgetAdvanced::on_pushButtonSave_clicked()
{
    QString sSaveFileName = XBinary::getResultFileName(g_pDevice, QString("%1.txt").arg(QString("DiE")));

    QString _sFileName = QFileDialog::getSaveFileName(this, tr("Save"), sSaveFileName, QString("%1 (*.txt);;%2 (*)").arg(tr("Text files"), tr("All files")));

    if (!_sFileName.isEmpty()) {
        if (!XOptions::saveTreeView(ui->treeViewResult, sSaveFileName)) {
            QMessageBox::critical(XOptions::getMainWidget(this), tr("Error"), QString("%1: %2").arg(tr("Cannot save file"), _sFileName));
        }
    }
}

void DIEWidgetAdvanced::on_pushButtonScan_clicked()
{
    process();
}

void DIEWidgetAdvanced::onSelectionChanged(const QItemSelection &itemSelected, const QItemSelection &itemDeselected)
{
    Q_UNUSED(itemDeselected)

    ui->plainTextEditSignature->clear();
    ui->lineEditSignatureName->clear();

    QModelIndexList listSelected = itemSelected.indexes();

    if (listSelected.count() >= 1) {
        QString sSignatureName = listSelected.at(0).data(Qt::UserRole + ScanItemModel::UD_INFO).toString();
        QString sSignatureFileName = listSelected.at(0).data(Qt::UserRole + ScanItemModel::UD_INFO2).toString();  // TODO

        QByteArray baData = XBinary::readFile(sSignatureFileName);
        ui->plainTextEditSignature->setPlainText(baData);
        ui->lineEditSignatureName->setText(sSignatureName);
    }
}

void DIEWidgetAdvanced::on_comboBoxType_currentIndexChanged(int nIndex)
{
    Q_UNUSED(nIndex)

    process();
}

void DIEWidgetAdvanced::on_pushButtonSignatures_clicked()
{
    DiE_Script dieScript;

    dieScript.loadDatabase(getGlobalOptions()->getValue(XOptions::ID_SCAN_DATABASEPATH).toString(), true);
    dieScript.loadDatabase(getGlobalOptions()->getValue(XOptions::ID_SCAN_CUSTOMDATABASEPATH).toString(), false);  // TODO optimize

    DialogDIESignatures dialogSignatures(this, &dieScript);
    dialogSignatures.setGlobal(getShortcuts(), getGlobalOptions());
    dialogSignatures.setData(g_pDevice, (XBinary::FT)(ui->comboBoxType->currentData().toInt()), "");

    dialogSignatures.exec();
}

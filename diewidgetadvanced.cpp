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

    XOptions::adjustToolButton(ui->toolButtonScan, XOptions::ICONTYPE_SCAN);
    XOptions::adjustToolButton(ui->toolButtonSave, XOptions::ICONTYPE_SAVE);
    XOptions::adjustToolButton(ui->toolButtonSignatures, XOptions::ICONTYPE_SIGNATURE);

    ui->comboBoxType->setToolTip(tr("Type"));
    ui->comboBoxFlags->setToolTip(tr("Flags"));
    ui->comboBoxDatabases->setToolTip(tr("Database"));
    ui->treeViewResult->setToolTip(tr("Result"));
    ui->plainTextEditSignature->setToolTip(tr("Signature"));
    ui->lineEditSignatureName->setToolTip(tr("Signature name"));
    ui->toolButtonSave->setToolTip(tr("Save"));
    ui->toolButtonScan->setToolTip(tr("Scan"));
    ui->toolButtonSignatures->setToolTip(tr("Signatures"));

    g_scanResult = {};
    g_pDevice = nullptr;
    g_pModel = nullptr;

    g_bInitDatabase = false;
    g_scanOptions = {};

    ui->comboBoxFlags->setData(XScanEngine::getScanFlags(), XComboBoxEx::CBTYPE_FLAGS, 0, tr("Flags"));
    ui->comboBoxDatabases->setData(XScanEngine::getDatabases(), XComboBoxEx::CBTYPE_FLAGS, 0, tr("Database"));
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
    getGlobalOptions()->adjustWidget(ui->plainTextEditSignature, XOptions::ID_VIEW_FONT_TEXTEDITS);
    getGlobalOptions()->adjustTreeView(ui->treeViewResult, XOptions::ID_VIEW_FONT_TREEVIEWS);

    quint64 nFlags = XScanEngine::getScanFlagsFromGlobalOptions(getGlobalOptions());
    ui->comboBoxFlags->setValue(nFlags);

    quint64 nDatabases = XScanEngine::getDatabasesFromGlobalOptions(getGlobalOptions());
    ui->comboBoxDatabases->setValue(nDatabases);
}

void DIEWidgetAdvanced::setGlobal(XShortcuts *pShortcuts, XOptions *pXOptions)
{
    XShortcutsWidget::setGlobal(pShortcuts, pXOptions);
}

void DIEWidgetAdvanced::registerShortcuts(bool bState)
{
    Q_UNUSED(bState)
    // TODO
}

void DIEWidgetAdvanced::process()
{
    g_scanOptions.bUseCustomDatabase = true;
    g_scanOptions.bUseExtraDatabase = true;
    g_scanOptions.bShowType = true;
    g_scanOptions.bShowVersion = true;
    g_scanOptions.bShowInfo = true;
    g_scanOptions.bLogProfiling = false;
    g_scanOptions.bShowScanTime = false;
    g_scanOptions.fileType = (XBinary::FT)(ui->comboBoxType->currentData().toInt());
    g_scanOptions.nBufferSize = getGlobalOptions()->getValue(XOptions::ID_SCAN_BUFFERSIZE).toULongLong();
    g_scanOptions.bIsHighlight = getGlobalOptions()->getValue(XOptions::ID_SCAN_HIGHLIGHT).toBool();
    g_scanOptions.bIsSort = getGlobalOptions()->getValue(XOptions::ID_SCAN_SORT).toBool();

    if (!g_bInitDatabase) {
        g_bInitDatabase = g_dieScript.loadDatabaseFromGlobalOptions(getGlobalOptions());  // TODO optimize
    }

    quint64 nFlags = ui->comboBoxFlags->getValue().toULongLong();
    XScanEngine::setScanFlags(&g_scanOptions, nFlags);

    quint64 nDatabases = ui->comboBoxDatabases->getValue().toULongLong();
    XScanEngine::setDatabases(&g_scanOptions, nDatabases);

    XScanEngine::SCAN_RESULT scanResult = {};

    DialogDIEScanProcess ds(this, &g_dieScript);
    ds.setGlobal(getShortcuts(), getGlobalOptions());
    ds.setData(g_pDevice, &g_scanOptions, &scanResult);
    ds.exec();

    // QAbstractItemModel *pOldModel = ui->treeViewResult->model();
    ScanItemModel *pOldModel = g_pModel;

    g_pModel = new ScanItemModel(&g_scanOptions, &(scanResult.listRecords), 1);
    ui->treeViewResult->setModel(g_pModel);
    ui->treeViewResult->expandAll();

    connect(ui->treeViewResult->selectionModel(), SIGNAL(selectionChanged(QItemSelection, QItemSelection)), SLOT(onSelectionChanged(QItemSelection, QItemSelection)));

    // deleteOldAbstractModel(&pOldModel);
    delete pOldModel;
}

void DIEWidgetAdvanced::on_toolButtonSave_clicked()
{
    QString sSaveFileName = XBinary::getResultFileName(g_pDevice, QString("%1.txt").arg(QString("DiE")));

    QString _sFileName = QFileDialog::getSaveFileName(this, tr("Save"), sSaveFileName, QString("%1 (*.txt);;%2 (*)").arg(tr("Text files"), tr("All files")));

    if (!_sFileName.isEmpty()) {
        if (!XOptions::saveTreeView(ui->treeViewResult, sSaveFileName)) {
            QMessageBox::critical(XOptions::getMainWidget(this), tr("Error"), QString("%1: %2").arg(tr("Cannot save file"), _sFileName));
        }
    }
}

void DIEWidgetAdvanced::on_toolButtonScan_clicked()
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

void DIEWidgetAdvanced::on_toolButtonSignatures_clicked()
{
    if (!g_bInitDatabase) {
        g_bInitDatabase = g_dieScript.loadDatabaseFromGlobalOptions(getGlobalOptions());
    }

    DialogDIESignatures dialogSignatures(this, &g_dieScript);
    dialogSignatures.setGlobal(getShortcuts(), getGlobalOptions());
    dialogSignatures.setData(g_pDevice, (XBinary::FT)(ui->comboBoxType->currentData().toInt()), "");

    dialogSignatures.exec();
}

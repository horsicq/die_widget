/* Copyright (c) 2019-2026 hors<horsicq@gmail.com>
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

    m_scanResult = {};
    m_pDevice = nullptr;
    m_pModel = nullptr;

    m_scanOptions = {};

    ui->comboBoxFlags->setData(XScanEngine::getScanFlags(), XComboBoxEx::CBTYPE_FLAGS, 0, tr("Flags"));
    ui->comboBoxDatabases->setData(XScanEngine::getDatabases(), XComboBoxEx::CBTYPE_FLAGS, 0, tr("Database"));
}

DIEWidgetAdvanced::~DIEWidgetAdvanced()
{
    delete ui;
}

void DIEWidgetAdvanced::setData(QIODevice *pDevice, bool bScan, XBinary::FT fileType)
{
    m_pDevice = pDevice;

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

void DIEWidgetAdvanced::reloadData(bool bSaveSelection)
{
    Q_UNUSED(bSaveSelection)
    // TODO
    process();
}

void DIEWidgetAdvanced::registerShortcuts(bool bState)
{
    Q_UNUSED(bState)
    // TODO
}

void DIEWidgetAdvanced::process()
{
    m_scanOptions.bUseCustomDatabase = true;
    m_scanOptions.bUseExtraDatabase = true;
    m_scanOptions.bShowType = true;
    m_scanOptions.bShowVersion = true;
    m_scanOptions.bShowInfo = true;
    m_scanOptions.bLogProfiling = false;
    m_scanOptions.bShowScanTime = false;
    m_scanOptions.fileType = (XBinary::FT)(ui->comboBoxType->currentData().toInt());
    m_scanOptions.nBufferSize = getGlobalOptions()->getValue(XOptions::ID_ENGINE_BUFFERSIZE).toULongLong();
    m_scanOptions.bIsHighlight = getGlobalOptions()->getValue(XOptions::ID_SCAN_HIGHLIGHT).toBool();
    m_scanOptions.bHideUnknown = getGlobalOptions()->getValue(XOptions::ID_SCAN_HIDEUNKNOWN).toBool();
    m_scanOptions.bIsSort = getGlobalOptions()->getValue(XOptions::ID_SCAN_SORT).toBool();

    DiE_Script dieScript;

    dieScript.loadDatabaseFromGlobalOptions(getGlobalOptions());  // TODO optimize

    quint64 nFlags = ui->comboBoxFlags->getValue().toULongLong();
    XScanEngine::setScanFlags(&m_scanOptions, nFlags);

    quint64 nDatabases = ui->comboBoxDatabases->getValue().toULongLong();
    XScanEngine::setDatabases(&m_scanOptions, nDatabases);

    XScanEngine::SCAN_RESULT scanResult = {};

    XDialogProcess ds(this, &dieScript);
    ds.setGlobal(getShortcuts(), getGlobalOptions());
    dieScript.setData(m_pDevice, &m_scanOptions, &scanResult, ds.getPdStruct());
    ds.start();
    ds.exec();

    // QAbstractItemModel *pOldModel = ui->treeViewResult->model();
    ScanItemModel *pOldModel = m_pModel;

    m_pModel = new ScanItemModel(&m_scanOptions, &(scanResult.listRecords), 1);
    ui->treeViewResult->setModel(m_pModel);
    ui->treeViewResult->expandAll();

    connect(ui->treeViewResult->selectionModel(), SIGNAL(selectionChanged(QItemSelection, QItemSelection)), SLOT(onSelectionChanged(QItemSelection, QItemSelection)));

    // deleteOldAbstractModel(&pOldModel);
    delete pOldModel;
}

void DIEWidgetAdvanced::on_toolButtonSave_clicked()
{
    QString sSaveFileName = XBinary::getResultFileName(m_pDevice, QString("%1.txt").arg(QString("DiE")));

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
    DiE_Script dieScript;
    dieScript.loadDatabaseFromGlobalOptions(getGlobalOptions());

    DialogDIESignatures dialogSignatures(this, &dieScript);
    dialogSignatures.setGlobal(getShortcuts(), getGlobalOptions());
    dialogSignatures.setData(m_pDevice, (XBinary::FT)(ui->comboBoxType->currentData().toInt()), "");

    dialogSignatures.exec();
}

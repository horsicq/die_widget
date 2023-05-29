/* Copyright (c) 2019-2023 hors<horsicq@gmail.com>
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
#include "dialogdiesignatures.h"

#include "ui_dialogdiesignatures.h"

DialogDIESignatures::DialogDIESignatures(QWidget *pParent, DiE_Script *pDieScript, QString sFileName, XBinary::FT fileType, QString sSignature)
    : XShortcutsDialog(pParent), ui(new Ui::DialogDIESignatures)
{
    ui->setupUi(this);

    memset(shortCuts, 0, sizeof shortCuts);

    g_data = {};

    setWindowFlags(Qt::Window);

    this->pDieScript = pDieScript;
    this->sFileName = sFileName;
    this->fileType = fileType;
    this->sSignature = sSignature;

    connect(pDieScript, SIGNAL(infoMessage(QString)), this, SLOT(infoMessage(QString)));

    ui->plainTextEditSignature->setLineWrapMode(QPlainTextEdit::NoWrap);

    ui->treeWidgetSignatures->setSortingEnabled(false);

    QTreeWidgetItem *pRootItem = new QTreeWidgetItem(ui->treeWidgetSignatures);
    pRootItem->setText(0, tr("Database"));

    _handleTreeItems(pRootItem, XBinary::FT_UNKNOWN);

    handleTreeItems(pRootItem, XBinary::FT_BINARY, "Binary");
    handleTreeItems(pRootItem, XBinary::FT_COM, "COM");
    handleTreeItems(pRootItem, XBinary::FT_MSDOS, "MSDOS");
    handleTreeItems(pRootItem, XBinary::FT_NE, "NE");
    handleTreeItems(pRootItem, XBinary::FT_LE, "LE");
    handleTreeItems(pRootItem, XBinary::FT_LX, "LX");
    handleTreeItems(pRootItem, XBinary::FT_PE, "PE");
    handleTreeItems(pRootItem, XBinary::FT_MACHO, "MACH");
    handleTreeItems(pRootItem, XBinary::FT_ELF, "ELF");

    ui->treeWidgetSignatures->setSortingEnabled(true);
    ui->treeWidgetSignatures->sortByColumn(0, Qt::AscendingOrder);

    g_bCurrentEdited = false;
    ui->pushButtonSave->setEnabled(false);

    ui->checkBoxShowType->setChecked(true);
    ui->checkBoxShowOptions->setChecked(true);
    ui->checkBoxShowVersion->setChecked(true);
    ui->checkBoxDeepScan->setChecked(true);
    ui->checkBoxHeuristicScan->setChecked(true);
    ui->checkBoxRecursiveScan->setChecked(true);

    ui->checkBoxReadOnly->setChecked(true);

    if (fileType != XBinary::FT_UNKNOWN) {
        qint32 nNumberOfTopLevelItems = ui->treeWidgetSignatures->topLevelItemCount();

        for (qint32 i = 0; i < nNumberOfTopLevelItems; i++) {
            if (_setTreeItem(ui->treeWidgetSignatures, ui->treeWidgetSignatures->topLevelItem(i), fileType, sSignature)) {
                break;
            }
        }
    } else {
        ui->treeWidgetSignatures->expandAll();
    }

    ui->comboBoxFunction->addItem("detect", "detect");

#ifndef QT_SCRIPTTOOLS_LIB
    ui->pushButtonDebug->hide();
#endif
}

DialogDIESignatures::~DialogDIESignatures()
{
    delete ui;
}

void DialogDIESignatures::adjustView()
{
    QFont _font;
    QString sFont = getGlobalOptions()->getValue(XOptions::ID_SCAN_EDITORFONT).toString();

    if ((sFont != "") && _font.fromString(sFont)) {
        ui->plainTextEditSignature->setFont(_font);
    }
}

qint32 DialogDIESignatures::handleTreeItems(QTreeWidgetItem *pRootItem, XBinary::FT fileType, QString sText)
{
    qint32 nResult = 0;

    if (pDieScript->isSignaturesPresent(fileType)) {
        QTreeWidgetItem *pItem = new QTreeWidgetItem(pRootItem);
        pItem->setText(0, sText);
        nResult = _handleTreeItems(pItem, fileType);
    }

    return nResult;
}

qint32 DialogDIESignatures::_handleTreeItems(QTreeWidgetItem *pItemParent, XBinary::FT fileType)
{
    qint32 nResult = 0;

    QList<DiE_ScriptEngine::SIGNATURE_RECORD> *pListSignatures = pDieScript->getSignatures();

    qint32 nNumberOfSignatures = pListSignatures->count();

    for (qint32 i = 0; i < nNumberOfSignatures; i++) {
        if (pListSignatures->at(i).fileType == fileType) {
            QTreeWidgetItem *pRootItem = new QTreeWidgetItem(pItemParent);
            pRootItem->setText(0, pListSignatures->at(i).sName);
            pRootItem->setData(0, Qt::UserRole + UD_FILEPATH, pListSignatures->at(i).sFilePath);
            pRootItem->setData(0, Qt::UserRole + UD_FILETYPE, pListSignatures->at(i).fileType);
            pRootItem->setData(0, Qt::UserRole + UD_NAME, pListSignatures->at(i).sName);

            nResult++;
        }
    }

    return nResult;
}

void DialogDIESignatures::runScript(QString sFunction, bool bIsDebug)
{
    enableControls(false);

    QTreeWidgetItem *pItemCurrent = ui->treeWidgetSignatures->currentItem();

    if (pItemCurrent) {
        if (g_bCurrentEdited) {
            save();
        }

        ui->plainTextEditResult->clear();

        DiE_Script::OPTIONS scanOptions = {};

        scanOptions.bShowType = ui->checkBoxShowType->isChecked();
        scanOptions.bShowOptions = ui->checkBoxShowOptions->isChecked();
        scanOptions.bShowVersion = ui->checkBoxShowVersion->isChecked();
        scanOptions.bIsDeepScan = ui->checkBoxDeepScan->isChecked();
        scanOptions.bIsHeuristicScan = ui->checkBoxHeuristicScan->isChecked();
        scanOptions.bIsVerbose = ui->checkBoxVerbose->isChecked();
        scanOptions.bIsRecursiveScan = ui->checkBoxRecursiveScan->isChecked();

        scanOptions.sSignatureName = pItemCurrent->data(0, Qt::UserRole + UD_NAME).toString();
        scanOptions.fileType = (XBinary::FT)ui->treeWidgetSignatures->currentItem()->data(0, Qt::UserRole + UD_FILETYPE).toInt();

        DiE_Script::SCAN_RESULT scanResult = {};

        if (bIsDebug) {
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
            QScriptEngineDebugger debugger(this);
            QMainWindow *debugWindow = debugger.standardWindow();
            debugWindow->setWindowModality(Qt::WindowModal);
            debugWindow->setWindowTitle(tr("Debugger"));
            //        debugWindow->resize(600,350);
            pDieScript->setDebugger(&debugger);
#endif

            scanResult = pDieScript->processFile(sFileName, &scanOptions, sFunction);
        } else {
            scanResult = pDieScript->processFile(sFileName, &scanOptions, sFunction);
        }

        if (bIsDebug) {
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
            pDieScript->removeDebugger();
#endif
        }

        QList<XBinary::SCANSTRUCT> listResult = DiE_Script::convert(&(scanResult.listRecords));

        ScanItemModel model(&listResult);

        ui->plainTextEditResult->appendPlainText(model.toFormattedString());

        if (scanResult.listErrors.count()) {
            ui->plainTextEditResult->appendPlainText(DiE_Script::getErrorsString(&scanResult));
        }

        ui->lineEditElapsedTime->setText(QString("%1 %2").arg(scanResult.nScanTime).arg(tr("msec")));
        // TODO only scripts for this type if not messagebox
    }

    enableControls(true);
}

void DialogDIESignatures::on_treeWidgetSignatures_currentItemChanged(QTreeWidgetItem *pItemCurrent, QTreeWidgetItem *pItemPrevious)
{
    Q_UNUSED(pItemPrevious)

    QString sSignatureFilePath = pItemCurrent->data(0, Qt::UserRole).toString();

    if (sSignatureFilePath != sCurrentSignatureFilePath) {
        const bool bBlocked1 = ui->plainTextEditSignature->blockSignals(true);

        if (g_bCurrentEdited) {
            // TODO handle warning
        }

        sCurrentSignatureFilePath = sSignatureFilePath;
        DiE_ScriptEngine::SIGNATURE_RECORD signatureRecord = pDieScript->getSignatureByFilePath(sCurrentSignatureFilePath);

        ui->plainTextEditSignature->setPlainText(signatureRecord.sText);
        ui->pushButtonSave->setEnabled(false);

        ui->plainTextEditSignature->blockSignals(bBlocked1);
    }
}

void DialogDIESignatures::on_pushButtonSave_clicked()
{
    save();
}

void DialogDIESignatures::save()
{
    if (pDieScript->updateSignature(sCurrentSignatureFilePath, ui->plainTextEditSignature->toPlainText())) {
        g_bCurrentEdited = false;
        ui->pushButtonSave->setEnabled(false);
    } else {
        // Handle error
    }
}

void DialogDIESignatures::on_pushButtonRun_clicked()
{
    runScript(ui->comboBoxFunction->currentData().toString(), false);
}

void DialogDIESignatures::on_pushButtonDebug_clicked()
{
    runScript(ui->comboBoxFunction->currentData().toString(), true);
}

void DialogDIESignatures::on_pushButtonClearResult_clicked()
{
    ui->plainTextEditResult->clear();
}

void DialogDIESignatures::on_pushButtonClose_clicked()
{
    this->close();
}

void DialogDIESignatures::on_plainTextEditSignature_textChanged()
{
    g_bCurrentEdited = true;
    ui->pushButtonSave->setEnabled(true);
}

void DialogDIESignatures::on_checkBoxReadOnly_toggled(bool bChecked)
{
    ui->plainTextEditSignature->setReadOnly(bChecked);
}

bool DialogDIESignatures::_setTreeItem(QTreeWidget *pTree, QTreeWidgetItem *pItem, XBinary::FT fileType, const QString &sSignature)
{
    bool bResult = false;

    XBinary::FT _fileType = (XBinary::FT)pItem->data(0, Qt::UserRole + UD_FILETYPE).toInt();
    QString _sSignature = pItem->data(0, Qt::UserRole + UD_NAME).toString();

    if ((XBinary::checkFileType(_fileType, fileType)) && ((sSignature == "") || (_sSignature == sSignature))) {
        pTree->setCurrentItem(pItem);

        bResult = true;
    } else {
        qint32 nNumberOfChildren = pItem->childCount();

        for (qint32 i = 0; i < nNumberOfChildren; i++) {
            if (_setTreeItem(pTree, pItem->child(i), fileType, sSignature)) {
                bResult = true;
                break;
            }
        }
    }

    return bResult;
}

void DialogDIESignatures::enableControls(bool bState)
{
    ui->treeWidgetSignatures->setEnabled(bState);
    ui->pushButtonClearResult->setEnabled(bState);
    ui->pushButtonClose->setEnabled(bState);
    ui->pushButtonDebug->setEnabled(bState);
    ui->pushButtonRun->setEnabled(bState);

    if (g_bCurrentEdited) {
        ui->pushButtonSave->setEnabled(bState);
    }
}

void DialogDIESignatures::infoMessage(const QString &sInfoMessage)
{
    ui->plainTextEditResult->appendPlainText(sInfoMessage);
}

void DialogDIESignatures::on_pushButtonFind_clicked()
{
    findString();
}

void DialogDIESignatures::on_pushButtonFindNext_clicked()
{
    findNext();
}

void DialogDIESignatures::findString()
{
    DialogFindText dialogFindText(this);

    dialogFindText.setData(&g_data);

    if (dialogFindText.exec() == QDialog::Accepted) {
        findNext();
    }
}

void DialogDIESignatures::findNext()
{
    QTextDocument::FindFlags findFlags = (QTextDocument::FindFlags)0;

    if (g_data.bIsMatchCase) {
        findFlags |= QTextDocument::FindCaseSensitively;
    }

    ui->plainTextEditSignature->find(g_data.sText, findFlags);
}

void DialogDIESignatures::registerShortcuts(bool bState)
{
    if (bState) {
        if (!shortCuts[SC_FIND_STRING]) shortCuts[SC_FIND_STRING] = new QShortcut(getShortcuts()->getShortcut(X_ID_SCAN_EDITOR_FIND_STRING), this, SLOT(findString()));
        if (!shortCuts[SC_FIND_NEXT]) shortCuts[SC_FIND_NEXT] = new QShortcut(getShortcuts()->getShortcut(X_ID_SCAN_EDITOR_FIND_NEXT), this, SLOT(findNext()));
    } else {
        for (qint32 i = 0; i < __SC_SIZE; i++) {
            if (shortCuts[i]) {
                delete shortCuts[i];
                shortCuts[i] = nullptr;
            }
        }
    }
}

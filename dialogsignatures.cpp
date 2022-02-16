/* Copyright (c) 2019-2022 hors<horsicq@gmail.com>
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
#include "dialogsignatures.h"
#include "ui_dialogsignatures.h"

DialogSignatures::DialogSignatures(QWidget *pParent,DiE_Script *pDieScript,QString sFileName,XBinary::FT fileType,QString sSignature) :
    QDialog(pParent),
    ui(new Ui::DialogSignatures)
{
    ui->setupUi(this);

    setWindowFlags(Qt::Window);

    this->pDieScript=pDieScript;
    this->sFileName=sFileName;
    this->fileType=fileType;
    this->sSignature=sSignature;

    connect(pDieScript,SIGNAL(infoMessage(QString)),this,SLOT(infoMessage(QString)));

    ui->plainTextEditSignature->setLineWrapMode(QPlainTextEdit::NoWrap);

    ui->treeWidgetSignatures->setSortingEnabled(false);

    QTreeWidgetItem *pRootItem=new QTreeWidgetItem(ui->treeWidgetSignatures);
    pRootItem->setText(0,tr("Database"));

    _handleTreeItems(pRootItem,XBinary::FT_UNKNOWN);

    handleTreeItems(pRootItem,XBinary::FT_BINARY,   "Binary");
    handleTreeItems(pRootItem,XBinary::FT_COM,      "COM");
    handleTreeItems(pRootItem,XBinary::FT_MSDOS,    "MSDOS");
    handleTreeItems(pRootItem,XBinary::FT_NE,       "NE");
    handleTreeItems(pRootItem,XBinary::FT_LE,       "LE");
    handleTreeItems(pRootItem,XBinary::FT_LX,       "LX");
    handleTreeItems(pRootItem,XBinary::FT_PE,       "PE");
    handleTreeItems(pRootItem,XBinary::FT_MACHO,    "MACH");
    handleTreeItems(pRootItem,XBinary::FT_ELF,      "ELF");

    ui->treeWidgetSignatures->setSortingEnabled(true);
    ui->treeWidgetSignatures->sortByColumn(0,Qt::AscendingOrder);

    bCurrentEdited=false;
    ui->pushButtonSave->setEnabled(false);

    ui->checkBoxShowType->setChecked(true);
    ui->checkBoxShowOptions->setChecked(true);
    ui->checkBoxShowVersion->setChecked(true);
    ui->checkBoxDeepscan->setChecked(true);

    ui->checkBoxReadOnly->setChecked(true);

    if(fileType!=XBinary::FT_UNKNOWN)
    {
        qint32 nNumberOfTopLevelItems=ui->treeWidgetSignatures->topLevelItemCount();

        for(qint32 i=0;i<nNumberOfTopLevelItems;i++)
        {
            if(_setTreeItem(ui->treeWidgetSignatures,ui->treeWidgetSignatures->topLevelItem(i),fileType,sSignature))
            {
                break;
            }
        }
    }
    else
    {
        ui->treeWidgetSignatures->expandAll();
    }

#ifndef QT_SCRIPTTOOLS_LIB
    ui->pushButtonDebug->hide();
#endif
}

DialogSignatures::~DialogSignatures()
{
    delete ui;
}

qint32 DialogSignatures::handleTreeItems(QTreeWidgetItem *pRootItem,XBinary::FT fileType,QString sText)
{
    qint32 nResult=0;

    if(pDieScript->isSignaturesPresent(fileType))
    {
        QTreeWidgetItem *pItem=new QTreeWidgetItem(pRootItem);
        pItem->setText(0,sText);
        nResult=_handleTreeItems(pItem,fileType);
    }

    return nResult;
}

qint32 DialogSignatures::_handleTreeItems(QTreeWidgetItem *pParent,XBinary::FT fileType)
{
    qint32 nResult=0;

    QList<DiE_ScriptEngine::SIGNATURE_RECORD> *pListSignatures=pDieScript->getSignatures();

    qint32 nNumberOfSignatures=pListSignatures->count();

    for(qint32 i=0;i<nNumberOfSignatures;i++)
    {
        if(pListSignatures->at(i).fileType==fileType)
        {
            QTreeWidgetItem *pRootItem=new QTreeWidgetItem(pParent);
            pRootItem->setText(0,pListSignatures->at(i).sName);
            pRootItem->setData(0,Qt::UserRole+UD_FILEPATH,pListSignatures->at(i).sFilePath);
            pRootItem->setData(0,Qt::UserRole+UD_FILETYPE,pListSignatures->at(i).fileType);
            pRootItem->setData(0,Qt::UserRole+UD_NAME,pListSignatures->at(i).sName);

            nResult++;
        }
    }

    return nResult;
}

void DialogSignatures::runScript(bool bIsDebug)
{
    enableControls(false);

    QTreeWidgetItem *pCurrentItem=ui->treeWidgetSignatures->currentItem();

    if(pCurrentItem)
    {
        if(ui->pushButtonSave->isEnabled())
        {
            save(); // TODO Check
        }

        ui->plainTextEditResult->clear();

        DiE_Script::SCAN_OPTIONS scanOptions={};

        scanOptions.bShowType=ui->checkBoxShowType->isChecked();
        scanOptions.bShowOptions=ui->checkBoxShowOptions->isChecked();
        scanOptions.bShowVersion=ui->checkBoxShowVersion->isChecked();
        scanOptions.bDeepScan=ui->checkBoxDeepscan->isChecked();

        scanOptions.sSignatureName=pCurrentItem->data(0,Qt::UserRole+UD_NAME).toString();
        scanOptions.fileType=(XBinary::FT)ui->treeWidgetSignatures->currentItem()->data(0,Qt::UserRole+UD_FILETYPE).toInt();

        DiE_Script::SCAN_RESULT scanResult={};

        if(bIsDebug)
        {
        #if QT_VERSION < QT_VERSION_CHECK(6,0,0)
            QScriptEngineDebugger debugger(this);
            QMainWindow *debugWindow=debugger.standardWindow();
            debugWindow->setWindowModality(Qt::WindowModal);
            debugWindow->setWindowTitle(tr("Debugger"));
            //        debugWindow->resize(600,350);
            pDieScript->setDebugger(&debugger);
        #endif

            scanResult=pDieScript->scanFile(sFileName,&scanOptions);
        }
        else
        {
            scanResult=pDieScript->scanFile(sFileName,&scanOptions);
        }

        if(bIsDebug)
        {
        #if QT_VERSION < QT_VERSION_CHECK(6,0,0)
            pDieScript->removeDebugger();
        #endif
        }

        QList<XBinary::SCANSTRUCT> listResult=DiE_Script::convert(&(scanResult.listRecords));

        ScanItemModel model(&listResult);

        ui->plainTextEditResult->appendPlainText(model.toFormattedString());

        if(scanResult.listErrors.count())
        {
            ui->plainTextEditResult->appendPlainText(DiE_Script::getErrorsString(&scanResult));
        }

        ui->lineEditElapsedTime->setText(QString("%1 %2").arg(scanResult.nScanTime).arg(tr("msec")));
        // TODO only scripts for this type if not messagebox
    }

    enableControls(true);
}

void DialogSignatures::on_treeWidgetSignatures_currentItemChanged(QTreeWidgetItem *pCurrent,QTreeWidgetItem *pPrevious)
{
    Q_UNUSED(pPrevious)

    QString sSignatureFilePath=pCurrent->data(0,Qt::UserRole).toString();

    if(sSignatureFilePath!=sCurrentSignatureFilePath)
    {
    #if QT_VERSION >= 0x050300
        QSignalBlocker blocker(ui->plainTextEditSignature);
    #else
        const bool bBlocked1=ui->plainTextEditSignature->blockSignals(true);
    #endif

        if(bCurrentEdited)
        {
            // TODO handle warning
        }

        sCurrentSignatureFilePath=sSignatureFilePath;
        DiE_ScriptEngine::SIGNATURE_RECORD signatureRecord=pDieScript->getSignatureByFilePath(sCurrentSignatureFilePath);

        ui->plainTextEditSignature->setPlainText(signatureRecord.sText);
        ui->pushButtonSave->setEnabled(false);

    #if QT_VERSION < 0x050300
        ui->plainTextEditSignature->blockSignals(bBlocked1);
    #endif
    }
}

void DialogSignatures::on_pushButtonSave_clicked()
{
    save();
}

void DialogSignatures::save()
{
    if(pDieScript->updateSignature(sCurrentSignatureFilePath,ui->plainTextEditSignature->toPlainText()))
    {
        bCurrentEdited=false;
        ui->pushButtonSave->setEnabled(false);
    }
    else
    {
        // Handle error
    }
}

void DialogSignatures::on_pushButtonRun_clicked()
{
    runScript(false);
}

void DialogSignatures::on_pushButtonDebug_clicked()
{
    runScript(true);
}

void DialogSignatures::on_pushButtonClearResult_clicked()
{
    ui->plainTextEditResult->clear();
}

void DialogSignatures::on_pushButtonClose_clicked()
{
    this->close();
}

void DialogSignatures::on_plainTextEditSignature_textChanged()
{
    bCurrentEdited=true;
    ui->pushButtonSave->setEnabled(true);
}

void DialogSignatures::on_checkBoxReadOnly_toggled(bool bChecked)
{
    ui->plainTextEditSignature->setReadOnly(bChecked);
}

bool DialogSignatures::_setTreeItem(QTreeWidget *pTree,QTreeWidgetItem *pItem,XBinary::FT fileType,QString sSignature)
{
    bool bResult=false;

    XBinary::FT _fileType=(XBinary::FT)pItem->data(0,Qt::UserRole+UD_FILETYPE).toInt();
    QString _sSignature=pItem->data(0,Qt::UserRole+UD_NAME).toString();

    if( (XBinary::checkFileType(_fileType,fileType))&&
        ((sSignature=="")||(_sSignature==sSignature)))
    {
        pTree->setCurrentItem(pItem);

        bResult=true;
    }
    else
    {
        qint32 nNumberOfChildren=pItem->childCount();

        for(qint32 i=0;i<nNumberOfChildren;i++)
        {
            if(_setTreeItem(pTree,pItem->child(i),fileType,sSignature))
            {
                bResult=true;
                break;
            }
        }
    }

    return bResult;
}

void DialogSignatures::enableControls(bool bState)
{
    ui->treeWidgetSignatures->setEnabled(bState);
    ui->pushButtonClearResult->setEnabled(bState);
    ui->pushButtonClose->setEnabled(bState);
    ui->pushButtonDebug->setEnabled(bState);
    ui->pushButtonRun->setEnabled(bState);

    if(bCurrentEdited)
    {
        ui->pushButtonSave->setEnabled(bState);
    }
}

void DialogSignatures::infoMessage(QString sInfoMessage)
{
    ui->plainTextEditResult->appendPlainText(sInfoMessage);
}

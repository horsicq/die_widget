// copyright (c) 2019-2020 hors<horsicq@gmail.com>
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:

// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.

// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.
//
#include "dialogsignatures.h"
#include "ui_dialogsignatures.h"

DialogSignatures::DialogSignatures(QWidget *pParent, DiE_Script *pDieScript, QString sFileName, XBinary::FT fileType, QString sSignature) :
    QDialog(pParent),
    ui(new Ui::DialogSignatures)
{
    ui->setupUi(this);

    setWindowFlags(Qt::Window);

    this->pDieScript=pDieScript;
    this->sFileName=sFileName;
    this->fileType=fileType;
    this->sSignature=sSignature;

    ui->plainTextEditSignature->setLineWrapMode(QPlainTextEdit::NoWrap);

    ui->treeWidgetSignatures->setSortingEnabled(false);

    QTreeWidgetItem *pRootItem=new QTreeWidgetItem(ui->treeWidgetSignatures);
    pRootItem->setText(0,tr("Database"));

    _handleTreeItems(pRootItem,XBinary::FT_UNKNOWN);

    handleTreeItems(pRootItem,XBinary::FT_BINARY,   "Binary");
    handleTreeItems(pRootItem,XBinary::FT_MSDOS,    "MSDOS");
    handleTreeItems(pRootItem,XBinary::FT_MACH,     "MACH");
    handleTreeItems(pRootItem,XBinary::FT_ELF,      "ELF");
    handleTreeItems(pRootItem,XBinary::FT_PE,       "PE");

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
        int nCount=ui->treeWidgetSignatures->topLevelItemCount();

        for(int i=0;i<nCount;i++)
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
}

DialogSignatures::~DialogSignatures()
{
    delete ui;
}

int DialogSignatures::handleTreeItems(QTreeWidgetItem *pRootItem, XBinary::FT fileType, QString sText)
{
    int nResult=0;

    if(pDieScript->isSignaturesPresent(fileType))
    {
        QTreeWidgetItem *pItem=new QTreeWidgetItem(pRootItem);
        pItem->setText(0,sText);
        nResult=_handleTreeItems(pItem,fileType);
    }

    return nResult;
}

int DialogSignatures::_handleTreeItems(QTreeWidgetItem *pParent,XBinary::FT fileType)
{
    int nResult=0;

    QList<DiE_ScriptEngine::SIGNATURE_RECORD> *pListSignatures=pDieScript->getSignatures();

    int nCount=pListSignatures->count();

    for(int i=0;i<nCount;i++)
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
            save();
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
            QScriptEngineDebugger debugger(this);
            QMainWindow *debugWindow=debugger.standardWindow();
            debugWindow->setWindowModality(Qt::ApplicationModal);
            debugWindow->setWindowTitle(tr("Debugger"));
            //        debugWindow->resize(600,350);
            pDieScript->setDebugger(&debugger);

            scanResult=pDieScript->scanFile(sFileName,&scanOptions);
        }
        else
        {
            scanResult=pDieScript->scanFile(sFileName,&scanOptions);
        }

        ui->plainTextEditResult->setPlainText(DiE_Script::scanResultToPlainString(&scanResult));

        ui->lineEditElapsedTime->setText(QString("%1 %2").arg(scanResult.nScanTime).arg(tr("msec")));
        // TODO only scripts for this type if not messagebox
    }

    enableControls(true);
}

void DialogSignatures::on_treeWidgetSignatures_currentItemChanged(QTreeWidgetItem *current, QTreeWidgetItem *previous)
{
    Q_UNUSED(previous)

    QString sSignatureFilePath=current->data(0,Qt::UserRole).toString();

    if(sSignatureFilePath!=sCurrentSignatureFilePath)
    {
        QSignalBlocker blocker(ui->plainTextEditSignature);

        if(bCurrentEdited)
        {
            // TODO handle warning
        }

        sCurrentSignatureFilePath=sSignatureFilePath;
        DiE_ScriptEngine::SIGNATURE_RECORD signatureRecord=pDieScript->getSignatureByFilePath(sCurrentSignatureFilePath);

        ui->plainTextEditSignature->setPlainText(signatureRecord.sText);
        ui->pushButtonSave->setEnabled(false);
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

bool DialogSignatures::_setTreeItem(QTreeWidget *pTree, QTreeWidgetItem *pItem, XBinary::FT fileType, QString sSignature)
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
        int nCount=pItem->childCount();

        for(int i=0;i<nCount;i++)
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
    ui->pushButtonSave->setEnabled(bState);
}

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

DialogSignatures::DialogSignatures(QWidget *parent, DiE_Script *pDieScript, QString sFileName) :
    QDialog(parent),
    ui(new Ui::DialogSignatures)
{
    ui->setupUi(this);

    setWindowFlags(Qt::Window);

    this->pDieScript=pDieScript;
    this->sFileName=sFileName;

    ui->textEditSignature->setLineWrapMode(QTextEdit::NoWrap);

    QTreeWidgetItem *pRootItem=new QTreeWidgetItem(ui->treeWidgetSignatures);
    pRootItem->setText(0,tr("Database"));

    _handleTreeItems(pRootItem,XBinary::FT_UNKNOWN);

    handleTreeItems(pRootItem,XBinary::FT_BINARY,"Binary");
    handleTreeItems(pRootItem,XBinary::FT_MSDOS,"MSDOS");
    handleTreeItems(pRootItem,XBinary::FT_MACH,"MACH");
    handleTreeItems(pRootItem,XBinary::FT_ELF,"ELF");
    handleTreeItems(pRootItem,XBinary::FT_PE,"PE");

    bCurrentEdited=false;
    ui->pushButtonSave->setEnabled(false);

    ui->checkBoxShowType->setChecked(true);
    ui->checkBoxShowOptions->setChecked(true);
    ui->checkBoxShowVersion->setChecked(true);
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

    QList<DiE_ScriptEngine::SIGNATURE_RECORD> *pSignatures=pDieScript->getSignatures();

    int nCount=pSignatures->count();

    for(int i=0;i<nCount;i++)
    {
        if(pSignatures->at(i).fileType==fileType)
        {
            QTreeWidgetItem *pRootItem=new QTreeWidgetItem(pParent);
            pRootItem->setText(0,pSignatures->at(i).sName);
            pRootItem->setData(0,Qt::UserRole+UD_FILEPATH,pSignatures->at(i).sFilePath);
            pRootItem->setData(0,Qt::UserRole+UD_FILETYPE,pSignatures->at(i).fileType);
            pRootItem->setData(0,Qt::UserRole+UD_NAME,pSignatures->at(i).sName);

            nResult++;
        }
    }

    return nResult;
}

void DialogSignatures::runScript(bool bIsDebug)
{
    DiE_Script::SCAN_OPTIONS scanOptions={};

    scanOptions.bShowType=ui->checkBoxShowType->isChecked();
    scanOptions.bShowOptions=ui->checkBoxShowOptions->isChecked();
    scanOptions.bShowVersion=ui->checkBoxShowVersion->isChecked();
    scanOptions.bDeepScan=ui->checkBoxDeepscan->isChecked();

    scanOptions.sSignatureName=ui->treeWidgetSignatures->currentItem()->data(0,Qt::UserRole+UD_NAME).toString();
    scanOptions.fileType=(XBinary::FT)ui->treeWidgetSignatures->currentItem()->data(0,Qt::UserRole+UD_FILETYPE).toInt();

    DiE_Script::SCAN_RESULT scanResult=pDieScript->scanFile(sFileName,&scanOptions);

    // TODO is debug
    // TODO only scripts for this type if not messagebox
    // Save before run
}

void DialogSignatures::on_treeWidgetSignatures_currentItemChanged(QTreeWidgetItem *current, QTreeWidgetItem *previous)
{
    Q_UNUSED(previous)

    QString sSignatureFilePath=current->data(0,Qt::UserRole).toString();

    if(sSignatureFilePath!=sCurrentSignatureFilePath)
    {
        QSignalBlocker blocker(ui->textEditSignature);

        if(bCurrentEdited)
        {
            // TODO handle warning
        }

        sCurrentSignatureFilePath=sSignatureFilePath;
        DiE_ScriptEngine::SIGNATURE_RECORD signatureRecord=pDieScript->getSignatureByFilePath(sCurrentSignatureFilePath);

        ui->textEditSignature->setText(signatureRecord.sText);
    }
}

void DialogSignatures::on_textEditSignature_textChanged()
{
    bCurrentEdited=true;
    ui->pushButtonSave->setEnabled(true);
}

void DialogSignatures::on_pushButtonSave_clicked()
{
    save();
}

void DialogSignatures::save()
{
    if(pDieScript->updateSignature(sCurrentSignatureFilePath,ui->textEditSignature->toPlainText()))
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

void DialogSignatures::on_pushButtonSaveResult_clicked()
{
    // TODO
}

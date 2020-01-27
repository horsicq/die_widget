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

    this->pDieScript=pDieScript;
    this->sFileName=sFileName;

    ui->textEditSignature->setLineWrapMode(QTextEdit::NoWrap);

    QTreeWidgetItem *pRootItem=new QTreeWidgetItem(ui->treeWidgetSignatures);
    pRootItem->setText(0,tr("Database"));
    QTreeWidgetItem *pBinaryItem=new QTreeWidgetItem(pRootItem);
    pBinaryItem->setText(0,"Binary");
    QTreeWidgetItem *pMsDosItem=new QTreeWidgetItem(pRootItem);
    pMsDosItem->setText(0,"MSDOS");
    QTreeWidgetItem *pMachItem=new QTreeWidgetItem(pRootItem);
    pMachItem->setText(0,"MACH");
    QTreeWidgetItem *pElfItem=new QTreeWidgetItem(pRootItem);
    pElfItem->setText(0,"ELF");
    QTreeWidgetItem *pPeItem=new QTreeWidgetItem(pRootItem);
    pPeItem->setText(0,"PE");

    handleTreeItems(pRootItem,XBinary::FT_UNKNOWN);
    handleTreeItems(pBinaryItem,XBinary::FT_BINARY);
    handleTreeItems(pMsDosItem,XBinary::FT_MSDOS);
    handleTreeItems(pMachItem,XBinary::FT_MACH);
    handleTreeItems(pElfItem,XBinary::FT_ELF);
    handleTreeItems(pPeItem,XBinary::FT_PE);

    bCurrentEdited=false;
    ui->pushButtonSave->setEnabled(false);

    ui->checkBoxShowOptions->setChecked(true);
    ui->checkBoxShowVersion->setChecked(true);
}

DialogSignatures::~DialogSignatures()
{
    delete ui;
}

int DialogSignatures::handleTreeItems(QTreeWidgetItem *pParent,XBinary::FT fileType)
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
            pRootItem->setData(0,Qt::UserRole,pSignatures->at(i).sFilePath);

            nResult++;
        }
    }

    return nResult;
}

void DialogSignatures::on_treeWidgetSignatures_currentItemChanged(QTreeWidgetItem *current, QTreeWidgetItem *previous)
{
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
    DiE_Script::SCAN_OPTIONS scanOptions={};

    scanOptions.bShowOptions=ui->checkBoxShowOptions->isChecked();
    scanOptions.bShowVersion=ui->checkBoxShowVersion->isChecked();

    pDieScript->scanFile(sFileName,&scanOptions);

    // TODO !!!
}

void DialogSignatures::on_pushButtonDebug_clicked()
{

}

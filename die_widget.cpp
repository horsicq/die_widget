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
#include "die_widget.h"
#include "ui_die_widget.h"

DIE_Widget::DIE_Widget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::DIE_Widget)
{
    ui->setupUi(this);

    connect(&watcher,SIGNAL(finished()),this,SLOT(onScanFinished()));
    connect(&dieScript,SIGNAL(progressMaximumChanged(qint32)),this,SLOT(onProgressMaximumChanged(qint32)));
    connect(&dieScript,SIGNAL(progressValueChanged(qint32)),this,SLOT(onProgressValueChanged(qint32)));
}

DIE_Widget::~DIE_Widget()
{
    delete ui;
}

void DIE_Widget::setOptions(DIE_Widget::OPTIONS *pOptions)
{
    ui->checkBoxShowVersion->setChecked(pOptions->bShowVersion);
    ui->checkBoxShowOptions->setChecked(pOptions->bShowOptions);
    ui->checkBoxDeepScan->setChecked(pOptions->bDeepScan);

    if(dieScript.getDatabasePath()!=pOptions->sDatabasePath)
    {
        dieScript.loadDatabase(pOptions->sDatabasePath);
    }
}

void DIE_Widget::setData(QString sFileName, bool bScan)
{
    clear();

    this->sFileName=sFileName;
    scanType=ST_FILE;

    if(bScan)
    {
        process();
    }
}

void DIE_Widget::on_pushButtonScan_clicked()
{
    process();
}

void DIE_Widget::clear()
{
    scanType=ST_UNKNOWN;
    scanOptions={};
    scanResult={};
    bProcess=false;

    ui->tableWidgetResult->setRowCount(0);
}

void DIE_Widget::process()
{
    if(!bProcess)
    {
        bProcess=true;

        ui->pushButtonScan->setText(tr("Stop"));

        scanOptions.bShowVersion=ui->checkBoxShowVersion->isChecked();
        scanOptions.bShowOptions=ui->checkBoxShowOptions->isChecked();
        scanOptions.bDeepScan=ui->checkBoxDeepScan->isChecked();
        scanOptions.bShowType=true;
        //    scanOptions.bDebug=true;

        QFuture<void> future=QtConcurrent::run(this,&DIE_Widget::scan);

        watcher.setFuture(future);
    }
    else
    {
        stop();
        watcher.waitForFinished();
        ui->pushButtonScan->setText(tr("Scan"));
    }
}

void DIE_Widget::scan()
{
    if(scanType!=ST_UNKNOWN)
    {
        if(scanType==ST_FILE)
        {
            scanResult=dieScript.scanFile(sFileName,&scanOptions);
        }
    }
}

void DIE_Widget::stop()
{
    dieScript.stop();
}

void DIE_Widget::onScanFinished()
{
    bProcess=false;

    int nErrorCount=scanResult.listErrors.count();

    QString sLogButtonText;

    if(nErrorCount)
    {
        sLogButtonText=QString("%1(%2)").arg(tr("Log")).arg(nErrorCount);
    }
    else
    {
        sLogButtonText=tr("Log");
    }

    ui->pushButtonLog->setText(sLogButtonText);

    ui->lineEditElapsedTime->setText(QString("%1 %2").arg(scanResult.nScanTime).arg(tr("msec")));

    ui->tableWidgetResult->setColumnCount(0);

    int nCount=scanResult.listRecords.count();

    ui->tableWidgetResult->setRowCount(nCount);
    ui->tableWidgetResult->setColumnCount(4);

    // TODO if different filetypes +1 column

    for(int i=0;i<nCount;i++)
    {
        QTableWidgetItem *pWidgetType=new QTableWidgetItem;
        pWidgetType->setText(scanResult.listRecords.at(i).sType);
        ui->tableWidgetResult->setItem(i,COLUMN_TYPE,pWidgetType);

        QTableWidgetItem *pWidgetString=new QTableWidgetItem;
        pWidgetString->setText(scanResult.listRecords.at(i).sString);
        ui->tableWidgetResult->setItem(i,COLUMN_STRING,pWidgetString);

        QTableWidgetItem *pWidgetSignature=new QTableWidgetItem;
        pWidgetSignature->setText("S");
        ui->tableWidgetResult->setItem(i,COLUMN_SIGNATURE,pWidgetSignature);

        QTableWidgetItem *pWidgetInfo=new QTableWidgetItem;
        pWidgetInfo->setText("?");
        ui->tableWidgetResult->setItem(i,COLUMN_INFO,pWidgetInfo);
    }

    ui->tableWidgetResult->horizontalHeader()->setVisible(true);
//        ui->tableWidgetResult->horizontalHeader()->setFixedHeight(0);

    ui->tableWidgetResult->horizontalHeader()->setSectionResizeMode(COLUMN_TYPE,QHeaderView::ResizeToContents);
    ui->tableWidgetResult->horizontalHeader()->setSectionResizeMode(COLUMN_STRING,QHeaderView::Stretch);
    ui->tableWidgetResult->horizontalHeader()->setSectionResizeMode(COLUMN_SIGNATURE,QHeaderView::Interactive);
    ui->tableWidgetResult->horizontalHeader()->setSectionResizeMode(COLUMN_INFO,QHeaderView::Interactive);

    ui->tableWidgetResult->setColumnWidth(COLUMN_SIGNATURE,20);
    ui->tableWidgetResult->setColumnWidth(COLUMN_INFO,20);

    ui->tableWidgetResult->horizontalHeader()->setVisible(false);
    ui->progressBarProgress->setMaximum(100);
    ui->progressBarProgress->setValue(100);
    ui->pushButtonScan->setText(tr("Scan"));
}

void DIE_Widget::onProgressMaximumChanged(qint32 nMaximum)
{
    ui->progressBarProgress->setMaximum(nMaximum);
}

void DIE_Widget::onProgressValueChanged(qint32 nValue)
{
    ui->progressBarProgress->setValue(nValue);
}

void DIE_Widget::on_pushButtonSignatures_clicked()
{
    DialogSignatures dialogSignatures(this,&dieScript,sFileName);

    dialogSignatures.exec();
}

void DIE_Widget::on_pushButtonExtraInformation_clicked()
{
    // TODO
    // TODO Dialog
    // TODO Zip or folder
    QString sText="TODO";

    if(sText!="")
    {
        DialogInfo dialogInfo(this,sText); // TODO
        dialogInfo.exec();
    }
}

void DIE_Widget::on_pushButtonLog_clicked()
{
    // TODO
    // TODO Dialog
    int nErrorCount=scanResult.listErrors.count();

    if(nErrorCount)
    {
        DialogLog dialogLog(this,DiE_Script::getErrorsString(&scanResult)); // TODO
        dialogLog.exec();
    }
}

void DIE_Widget::on_tableWidgetResult_cellClicked(int row, int column)
{
    if(column==COLUMN_SIGNATURE)
    {
        // TODO
    }
    else if(column==COLUMN_INFO)
    {
        // TODO
    }
}

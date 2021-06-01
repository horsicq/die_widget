// copyright (c) 2019-2021 hors<horsicq@gmail.com>
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

DIE_Widget::DIE_Widget(QWidget *pParent) :
    QWidget(pParent),
    ui(new Ui::DIE_Widget)
{
    ui->setupUi(this);

    connect(&watcher,SIGNAL(finished()),this,SLOT(onScanFinished()));
    connect(&dieScript,SIGNAL(progressMaximumChanged(qint32)),this,SLOT(onProgressMaximumChanged(qint32)));
    connect(&dieScript,SIGNAL(progressValueChanged(qint32)),this,SLOT(onProgressValueChanged(qint32)));

    ui->pushButtonDieLog->setEnabled(false);

    clear();
}

DIE_Widget::~DIE_Widget()
{
    if(bProcess)
    {
        stop();
        watcher.waitForFinished();
    }

    delete ui;
}

void DIE_Widget::setOptions(DIE_Widget::OPTIONS *pOptions)
{
    ui->checkBoxDeepScan->setChecked(pOptions->bDeepScan);

    if(dieScript.getDatabasePath()!=pOptions->sDatabasePath)
    {
        dieScript.loadDatabase(pOptions->sDatabasePath);
    }
}

void DIE_Widget::setData(QString sFileName, bool bScan, XBinary::FT fileType)
{
    clear();

    // TODO
    if((fileType==XBinary::FT_ZIP)||(fileType==XBinary::FT_DEX))
    {
        fileType=XBinary::FT_BINARY;
    }

    this->sFileName=sFileName;
    this->fileType=fileType;
    scanType=ST_FILE;

    if(bScan)
    {
        process();
    }
}

void DIE_Widget::setDatabasePath(QString sDatabasePath)
{
    dieScript.loadDatabase(sDatabasePath);
}

void DIE_Widget::setInfoPath(QString sInfoPath)
{
    this->sInfoPath=sInfoPath;
}

void DIE_Widget::on_pushButtonDieScan_clicked()
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
        enableControls(false);
        bProcess=true;

        ui->pushButtonDieScan->setText(tr("Stop"));

        scanOptions.bShowVersion=true;
        scanOptions.bShowOptions=true;
        scanOptions.bDeepScan=ui->checkBoxDeepScan->isChecked();
        scanOptions.bShowType=true;
        scanOptions.fileType=fileType;
        //    scanOptions.bDebug=true;

        QFuture<void> future=QtConcurrent::run(this,&DIE_Widget::scan);

        watcher.setFuture(future);
    }
    else
    {
        ui->pushButtonDieScan->setEnabled(false);

        stop();
        watcher.waitForFinished();
        ui->pushButtonDieScan->setText(tr("Scan"));
        enableControls(true);
    }
}

void DIE_Widget::scan()
{
    if(scanType!=ST_UNKNOWN)
    {
        if(scanType==ST_FILE)
        {
            emit scanStarted();

            scanResult=dieScript.scanFile(sFileName,&scanOptions);

            emit scanFinished();
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

    int nNumberOfErrors=scanResult.listErrors.count();

    QString sLogButtonText;

    if(nNumberOfErrors)
    {
        sLogButtonText=QString("%1(%2)").arg(tr("Log")).arg(nNumberOfErrors);
    }
    else
    {
        sLogButtonText=tr("Log");
    }

    ui->pushButtonDieLog->setText(sLogButtonText);
    ui->pushButtonDieLog->setEnabled(nNumberOfErrors);

    ui->lineEditElapsedTime->setText(QString("%1 %2").arg(scanResult.nScanTime).arg(tr("msec")));

    ui->tableWidgetResult->setColumnCount(0);

    int nNumberOfRecords=scanResult.listRecords.count();

    ui->tableWidgetResult->setRowCount(nNumberOfRecords);
    ui->tableWidgetResult->setColumnCount(4);

    // TODO if different filetypes +1 column

    for(int i=0;i<nNumberOfRecords;i++)
    {
        QTableWidgetItem *pWidgetType=new QTableWidgetItem;
        pWidgetType->setText(scanResult.listRecords.at(i).sType);
        ui->tableWidgetResult->setItem(i,COLUMN_TYPE,pWidgetType);

        QTableWidgetItem *pWidgetString=new QTableWidgetItem;
        pWidgetString->setTextAlignment(Qt::AlignCenter);
        pWidgetString->setText(scanResult.listRecords.at(i).sResult);
        ui->tableWidgetResult->setItem(i,COLUMN_STRING,pWidgetString);

        QTableWidgetItem *pWidgetSignature=new QTableWidgetItem;
        pWidgetSignature->setText("S");
        ui->tableWidgetResult->setItem(i,COLUMN_SIGNATURE,pWidgetSignature);

        if(XBinary::isFileExists(getInfoFileName(scanResult.listRecords.at(i).sName)))
        {
            QTableWidgetItem *pWidgetInfo=new QTableWidgetItem;
            pWidgetInfo->setText("?");
            ui->tableWidgetResult->setItem(i,COLUMN_INFO,pWidgetInfo);
        }
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
    ui->pushButtonDieScan->setText(tr("Scan"));

    ui->pushButtonDieScan->setEnabled(true);
    enableControls(true);
}

void DIE_Widget::onProgressMaximumChanged(qint32 nMaximum)
{
    ui->progressBarProgress->setMaximum(nMaximum);
}

void DIE_Widget::onProgressValueChanged(qint32 nValue)
{
    ui->progressBarProgress->setValue(nValue);
}

void DIE_Widget::on_pushButtonDieSignatures_clicked()
{
    DialogSignatures dialogSignatures(this,&dieScript,sFileName,scanOptions.fileType,"");

    dialogSignatures.exec();
}

void DIE_Widget::on_pushButtonDieExtraInformation_clicked()
{
    DialogTextInfo dialogInfo(this);

    dialogInfo.setText(DiE_Script::scanResultToPlainString(&scanResult));

    dialogInfo.exec();
}

void DIE_Widget::on_pushButtonDieLog_clicked()
{
    DialogLog dialogLog(this,DiE_Script::getErrorsString(&scanResult));

    dialogLog.exec();
}

void DIE_Widget::on_tableWidgetResult_cellClicked(int nRow, int nColumn)
{
    if(nRow<scanResult.listRecords.count())
    {
        if(nColumn==COLUMN_SIGNATURE)
        {
            showSignature(scanResult.listRecords.at(nRow).sSignature);
        }
        else if(nColumn==COLUMN_INFO)
        {
            showInfo(scanResult.listRecords.at(nRow).sName);
        }
    }
}

void DIE_Widget::showInfo(QString sName)
{
    QString sFileName=getInfoFileName(sName);

    if(XBinary::isFileExists(sFileName))
    {
        DialogTextInfo dialogInfo(this);

        dialogInfo.setFile(sFileName);

        dialogInfo.exec();
    }
}

void DIE_Widget::showSignature(QString sName)
{
    DialogSignatures dialogSignatures(this,&dieScript,sFileName,scanOptions.fileType,sName);

    dialogSignatures.exec();
}

void DIE_Widget::enableControls(bool bState)
{
    if(!bState)
    {
        ui->tableWidgetResult->clear();
    }

    ui->tableWidgetResult->setEnabled(bState);
    ui->checkBoxDeepScan->setEnabled(bState);
    ui->pushButtonDieSignatures->setEnabled(bState);
    ui->pushButtonDieLog->setEnabled(bState);
    ui->pushButtonDieExtraInformation->setEnabled(bState);
    ui->lineEditElapsedTime->setEnabled(bState);
}

QString DIE_Widget::getInfoFileName(QString sName)
{
    QString sResult=XBinary::convertPathName(sInfoPath)+QDir::separator()+QString("%1.html").arg(sName);

    return sResult;
}

void DIE_Widget::on_tableWidgetResult_customContextMenuRequested(const QPoint &pos)
{
    QModelIndexList listIndexes=ui->tableWidgetResult->selectionModel()->selectedIndexes();

    if(listIndexes.size()>0)
    {
        QModelIndex index=listIndexes.at(0);

        if(index.column()==1)
        {
            QString sString=ui->tableWidgetResult->model()->data(index).toString();

            QMenu contextMenu(this);

            QAction actionCopy(QString("%1 \"%2\"").arg(tr("Copy as"),sString),this);
            connect(&actionCopy, SIGNAL(triggered()), this, SLOT(copyResult()));

            contextMenu.addAction(&actionCopy);

            contextMenu.exec(ui->tableWidgetResult->viewport()->mapToGlobal(pos));
        }
    }
}

void DIE_Widget::copyResult()
{
    QModelIndexList listIndexes=ui->tableWidgetResult->selectionModel()->selectedIndexes();

    if(listIndexes.size()>0)
    {
        QModelIndex index=listIndexes.at(0);

        if(index.column()==1)
        {
            QString sString=ui->tableWidgetResult->model()->data(index).toString();
            QApplication::clipboard()->setText(sString);
        }
    }
}

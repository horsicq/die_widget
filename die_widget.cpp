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
#include "die_widget.h"

#include "ui_die_widget.h"

DIE_Widget::DIE_Widget(QWidget *pParent) : XShortcutsWidget(pParent), ui(new Ui::DIE_Widget)
{
    ui->setupUi(this);

    g_pdStruct = XBinary::createPdStruct();

    connect(&watcher, SIGNAL(finished()), this, SLOT(onScanFinished()));

    ui->pushButtonDieLog->setEnabled(false);

    ui->checkBoxRecursiveScan->setChecked(true);

    g_pTimer = new QTimer(this);
    connect(g_pTimer, SIGNAL(timeout()), this, SLOT(timerSlot()));

    clear();
}

DIE_Widget::~DIE_Widget()
{
    if (bProcess) {
        stop();
        watcher.waitForFinished();
    }

    delete ui;
}

// void DIE_Widget::setOptions(DIE_Widget::OPTIONS *pOptions)
//{
//     ui->checkBoxRecursiveScan->setChecked(pOptions->bRecursiveScan);
//     ui->checkBoxDeepScan->setChecked(pOptions->bDeepScan);
//     ui->checkBoxAllTypes->setChecked(pOptions->bAllTypesScan);

//    if(g_dieScript.getDatabasePath()!=pOptions->sDatabasePath)
//    {
//        g_dieScript.loadDatabase(pOptions->sDatabasePath);
//    }
//}

void DIE_Widget::setData(const QString &sFileName, bool bScan, XBinary::FT fileType)
{
    clear();

    //    if(fileType==XBinary::FT_BINARY)
    //    {
    //        // TODO Check !!!
    //        fileType=XBinary::FT_COM;
    //    }

    // TODO
    if ((fileType == XBinary::FT_ZIP) || (fileType == XBinary::FT_DEX))  // TODO
    {
        fileType = XBinary::FT_BINARY;
    }

    this->sFileName = sFileName;
    this->fileType = fileType;
    g_scanType = ST_FILE;

    if (bScan) {
        process();
    }
}

void DIE_Widget::adjustView()
{
    this->g_sInfoPath = getGlobalOptions()->getInfoPath();
    g_dieScript.loadDatabase(getGlobalOptions()->getDatabasePath());  // TODO in Thread
}

void DIE_Widget::setGlobal(XShortcuts *pShortcuts, XOptions *pXOptions)
{
    ui->checkBoxAllTypesScan->setChecked(pXOptions->isAllTypesScan());
    ui->checkBoxDeepScan->setChecked(pXOptions->isDeepScan());
    ui->checkBoxHeuristicScan->setChecked(pXOptions->isHeuristicScan());
    ui->checkBoxVerbose->setChecked(pXOptions->isVerbose());
    ui->checkBoxRecursiveScan->setChecked(pXOptions->isRecursiveScan());

    XShortcutsWidget::setGlobal(pShortcuts, pXOptions);
}

void DIE_Widget::on_pushButtonDieScan_clicked()
{
    process();
}

void DIE_Widget::clear()
{
    g_scanType = ST_UNKNOWN;
    g_scanOptions = {};
    scanResult = {};
    bProcess = false;

    ui->treeViewResult->setModel(0);
}

void DIE_Widget::process()
{
    if (!bProcess) {
        enableControls(false);
        bProcess = true;

        ui->pushButtonDieScan->setText(tr("Stop"));
        ui->progressBarProgress->setValue(0);

        g_scanOptions.bShowVersion = true;
        g_scanOptions.bShowOptions = true;
        g_scanOptions.bIsRecursiveScan = ui->checkBoxRecursiveScan->isChecked();
        g_scanOptions.bIsDeepScan = ui->checkBoxDeepScan->isChecked();
        g_scanOptions.bIsHeuristicScan = ui->checkBoxHeuristicScan->isChecked();
        g_scanOptions.bIsVerbose = ui->checkBoxVerbose->isChecked();
        g_scanOptions.bAllTypesScan = ui->checkBoxAllTypesScan->isChecked();
        g_scanOptions.bShowType = true;
        g_scanOptions.fileType = fileType;
        g_scanOptions.bDebug = true;

        g_pTimer->start(200);  // TODO const

#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
        QFuture<void> future = QtConcurrent::run(&DIE_Widget::scan, this);
#else
        QFuture<void> future = QtConcurrent::run(this, &DIE_Widget::scan);
#endif

        watcher.setFuture(future);
    } else {
        ui->pushButtonDieScan->setEnabled(false);

        stop();
        watcher.waitForFinished();
        ui->pushButtonDieScan->setText(tr("Scan"));
        enableControls(true);
    }
}

void DIE_Widget::scan()
{
    if (g_scanType != ST_UNKNOWN) {
        if (g_scanType == ST_FILE) {
            emit scanStarted();

            g_pdStruct = XBinary::createPdStruct();

            scanResult = g_dieScript.scanFile(sFileName, &g_scanOptions, &g_pdStruct);

            emit scanFinished();
        }
    }
}

void DIE_Widget::stop()
{
    g_pdStruct.bIsStop = true;
}

void DIE_Widget::onScanFinished()
{
    bProcess = false;

    g_pTimer->stop();

    qint32 nNumberOfErrors = scanResult.listErrors.count();

    QString sLogButtonText;

    if (nNumberOfErrors) {
        sLogButtonText = QString("%1(%2)").arg(tr("Log"), QString::number(nNumberOfErrors));
    } else {
        sLogButtonText = tr("Log");
    }

    ui->pushButtonDieLog->setText(sLogButtonText);
    ui->pushButtonDieLog->setEnabled(nNumberOfErrors);

    ui->toolButtonElapsedTime->setText(QString("%1 %2").arg(scanResult.nScanTime).arg(tr("msec")));

    QAbstractItemModel *pOldModel = ui->treeViewResult->model();

    QList<XBinary::SCANSTRUCT> _listRecords = DiE_Script::convert(&(scanResult.listRecords));

    ScanItemModel *pModel = new ScanItemModel(&_listRecords, 3);
    ui->treeViewResult->setModel(pModel);
    ui->treeViewResult->expandAll();

    deleteOldAbstractModel(&pOldModel);

    //    ui->tableWidgetResult->setColumnCount(0);

    //    qint32 nNumberOfRecords=scanResult.listRecords.count();

    //    ui->tableWidgetResult->setRowCount(nNumberOfRecords);
    //    ui->tableWidgetResult->setColumnCount(4);

    //    // TODO if different filetypes +1 column

    //    for(qint32 i=0;i<nNumberOfRecords;i++)
    //    {
    //        QTableWidgetItem *pWidgetType=new QTableWidgetItem;
    //        pWidgetType->setText(scanResult.listRecords.at(i).sType);
    //        ui->tableWidgetResult->setItem(i,COLUMN_TYPE,pWidgetType);

    //        QTableWidgetItem *pWidgetString=new QTableWidgetItem;
    //        pWidgetString->setTextAlignment(Qt::AlignCenter);
    //        pWidgetString->setText(scanResult.listRecords.at(i).sResult);
    //        ui->tableWidgetResult->setItem(i,COLUMN_STRING,pWidgetString);

    //        QTableWidgetItem *pWidgetSignature=new QTableWidgetItem;
    //        pWidgetSignature->setText("S");
    //        ui->tableWidgetResult->setItem(i,COLUMN_SIGNATURE,pWidgetSignature);

    //        if(XBinary::isFileExists(getInfoFileName(scanResult.listRecords.at(i).sName)))
    //        {
    //            QTableWidgetItem *pWidgetInfo=new QTableWidgetItem;
    //            pWidgetInfo->setText("?");
    //            ui->tableWidgetResult->setItem(i,COLUMN_INFO,pWidgetInfo);
    //        }
    //    }

    //    ui->tableWidgetResult->horizontalHeader()->setVisible(true);
    ////        ui->tableWidgetResult->horizontalHeader()->setFixedHeight(0);

    //    ui->tableWidgetResult->horizontalHeader()->setSectionResizeMode(COLUMN_TYPE,QHeaderView::ResizeToContents);
    ui->treeViewResult->header()->setSectionResizeMode(COLUMN_STRING, QHeaderView::Stretch);
    ui->treeViewResult->header()->setSectionResizeMode(COLUMN_SIGNATURE, QHeaderView::Fixed);
    ui->treeViewResult->header()->setSectionResizeMode(COLUMN_INFO, QHeaderView::Fixed);

    ui->treeViewResult->setColumnWidth(COLUMN_SIGNATURE, 20);
    ui->treeViewResult->setColumnWidth(COLUMN_INFO, 20);

    ui->treeViewResult->header()->setVisible(false);

    ui->progressBarProgress->setMaximum(100);
    ui->progressBarProgress->setValue(100);
    ui->pushButtonDieScan->setText(tr("Scan"));

    ui->pushButtonDieScan->setEnabled(true);
    enableControls(true);
}

void DIE_Widget::on_pushButtonDieSignatures_clicked()
{
    DialogSignatures dialogSignatures(this, &g_dieScript, sFileName, g_scanOptions.fileType, "");

    dialogSignatures.setGlobal(getShortcuts(), getGlobalOptions());

    dialogSignatures.exec();
}

void DIE_Widget::on_pushButtonDieExtraInformation_clicked()
{
    DialogTextInfo dialogInfo(this);

    QList<XBinary::SCANSTRUCT> listResult = DiE_Script::convert(&(scanResult.listRecords));

    ScanItemModel model(&listResult);

    dialogInfo.setText(model.toFormattedString());

    dialogInfo.exec();
}

void DIE_Widget::on_pushButtonDieLog_clicked()
{
    DialogLog dialogLog(this, DiE_Script::getErrorsString(&scanResult));

    dialogLog.exec();
}

void DIE_Widget::showInfo(QString sName)
{
    if (sName != "") {
        QString sFileName = getInfoFileName(sName);

        if (XBinary::isFileExists(sFileName)) {
            DialogTextInfo dialogInfo(this);

            dialogInfo.setFileName(sFileName);

            dialogInfo.exec();
        } else {
            QString sLink = QString("http://www.google.com/search?q=%1").arg(sName);
            QDesktopServices::openUrl(QUrl(sLink));
        }
    }
}

void DIE_Widget::showSignature(XBinary::FT fileType, QString sName)
{
    if (sName != "") {
        DialogSignatures dialogSignatures(this, &g_dieScript, sFileName, fileType, sName);

        dialogSignatures.setGlobal(getShortcuts(), getGlobalOptions());

        dialogSignatures.exec();
    }
}

void DIE_Widget::enableControls(bool bState)
{
    if (!bState) {
        QAbstractItemModel *pOldModel = ui->treeViewResult->model();
        ui->treeViewResult->setModel(0);

        deleteOldAbstractModel(&pOldModel);
    }

    ui->treeViewResult->setEnabled(bState);
    ui->checkBoxRecursiveScan->setEnabled(bState);
    ui->checkBoxDeepScan->setEnabled(bState);
    ui->checkBoxHeuristicScan->setEnabled(bState);
    ui->checkBoxVerbose->setEnabled(bState);
    ui->checkBoxAllTypesScan->setEnabled(bState);
    ui->pushButtonDieSignatures->setEnabled(bState);
    ui->pushButtonDieLog->setEnabled(bState);
    ui->pushButtonDieExtraInformation->setEnabled(bState);
    ui->toolButtonElapsedTime->setEnabled(bState);
    ui->pushButtonDieScanDirectory->setEnabled(bState);
}

QString DIE_Widget::getInfoFileName(QString sName)
{
    QString sResult = XBinary::convertPathName(g_sInfoPath) + QDir::separator() + QString("html") + QDir::separator() + QString("%1.html").arg(sName);

    return sResult;
}

void DIE_Widget::copyResult()
{
    QModelIndexList listIndexes = ui->treeViewResult->selectionModel()->selectedIndexes();

    if (listIndexes.size() > 0) {
        QModelIndex index = listIndexes.at(0);

        if (index.column() == 0) {
            QString sString = ui->treeViewResult->model()->data(index).toString();
            QApplication::clipboard()->setText(sString);
        }
    }
}

void DIE_Widget::on_pushButtonDieScanDirectory_clicked()
{
    DialogDIEScanDirectory dds(this, QFileInfo(sFileName).absolutePath(), g_dieScript.getDatabasePath());

    dds.exec();
}

void DIE_Widget::registerShortcuts(bool bState)
{
    Q_UNUSED(bState)
}

void DIE_Widget::on_toolButtonElapsedTime_clicked()
{
    DialogElapsed dialogElapsed(this);

    dialogElapsed.setData(&scanResult);

    dialogElapsed.exec();
}

void DIE_Widget::on_treeViewResult_clicked(const QModelIndex &index)
{
    if (index.column() == COLUMN_SIGNATURE) {
        QString sSignature = ui->treeViewResult->model()->data(index, Qt::UserRole + ScanItemModel::UD_INFO).toString();
        XBinary::FT fileType = (XBinary::FT)(ui->treeViewResult->model()->data(index, Qt::UserRole + ScanItemModel::UD_FILETYPE).toInt());

        showSignature(fileType, sSignature);
    } else if (index.column() == COLUMN_INFO) {
        QString sName = ui->treeViewResult->model()->data(index, Qt::UserRole + ScanItemModel::UD_NAME).toString();

        showInfo(sName);
    }
}

void DIE_Widget::on_treeViewResult_customContextMenuRequested(const QPoint &pos)
{
    QModelIndexList listIndexes = ui->treeViewResult->selectionModel()->selectedIndexes();

    if (listIndexes.size() > 0) {
        QModelIndex index = listIndexes.at(0);

        if (index.column() == 0) {
            QString sString = ui->treeViewResult->model()->data(index).toString();

            QMenu contextMenu(this);

            QAction actionCopy(QString("%1 \"%2\"").arg(tr("Copy as"), sString), this);
            connect(&actionCopy, SIGNAL(triggered()), this, SLOT(copyResult()));

            contextMenu.addAction(&actionCopy);

            contextMenu.exec(ui->treeViewResult->viewport()->mapToGlobal(pos));
        }
    }
}

void DIE_Widget::timerSlot()
{
    ui->progressBarProgress->setMaximum(100);
    ui->progressBarProgress->setValue(XBinary::getPdStructProcent(&g_pdStruct));
}

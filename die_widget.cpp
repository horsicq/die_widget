/* Copyright (c) 2019-2025 hors<horsicq@gmail.com>
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

// bool _scanEngineCallback(const QString &sCurrentSignature, qint32 nNumberOfSignatures, qint32 nCurrentIndex, void *pUserData)
// {
//     return false;
// }

DIE_Widget::DIE_Widget(QWidget *pParent) : XShortcutsWidget(pParent), ui(new Ui::DIE_Widget)
{
    ui->setupUi(this);

    g_pdStruct = XBinary::createPdStruct();
    g_pModel = nullptr;
    g_bProcess = false;

    connect(&g_watcher, SIGNAL(finished()), this, SLOT(onScanFinished()));

    connect(&g_dieScript, SIGNAL(errorMessage(QString)), this, SLOT(handleErrorString(QString)));
    connect(&g_dieScript, SIGNAL(warningMessage(QString)), this, SLOT(handleWarningString(QString)));
    // connect(&g_dieScript, SIGNAL(infoMessage(QString)), this, SLOT(handleInfoString(QString)));

    ui->pushButtonDieLog->setEnabled(false);

    g_pTimer = new QTimer(this);
    connect(g_pTimer, SIGNAL(timeout()), this, SLOT(timerSlot()));

    clear();

    g_bInitDatabase = false;

    ui->comboBoxFlags->setData(XScanEngine::getScanFlags(), XComboBoxEx::CBTYPE_FLAGS, 0, tr("Flags"));
    ui->comboBoxDatabases->setData(XScanEngine::getDatabases(), XComboBoxEx::CBTYPE_FLAGS, 0, tr("Database"));

    ui->comboBoxDatabases->setItemEnabled(1, false);

    ui->stackedWidgetDieScan->setCurrentIndex(0);

    ui->toolButtonElapsedTime->setText(QString("%1 %2").arg(0).arg(tr("msec")));  // TODO Function
}

DIE_Widget::~DIE_Widget()
{
    if (g_bProcess) {
        stop();
        g_watcher.waitForFinished();
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
    // if ((fileType == XBinary::FT_ZIP) || (fileType == XBinary::FT_DEX))  // TODO
    // {
    //     fileType = XBinary::FT_BINARY;
    // }

    this->g_sFileName = sFileName;
    this->g_fileType = fileType;
    g_scanType = ST_FILE;

    if (bScan) {
        process();
    }
}

void DIE_Widget::adjustView()
{
    this->g_sInfoPath = getGlobalOptions()->getInfoPath();
    g_bInitDatabase = false;

    quint64 nFlags = XScanEngine::getScanFlagsFromGlobalOptions(getGlobalOptions());
    ui->comboBoxFlags->setValue(nFlags);

    quint64 nDatabases = XScanEngine::getDatabasesFromGlobalOptions(getGlobalOptions());
    ui->comboBoxDatabases->setValue(nDatabases);
}

void DIE_Widget::setGlobal(XShortcuts *pShortcuts, XOptions *pXOptions)
{
    XShortcutsWidget::setGlobal(pShortcuts, pXOptions);
}

void DIE_Widget::reloadData(bool bSaveSelection)
{
    Q_UNUSED(bSaveSelection)
    process();
}

void DIE_Widget::clear()
{
    g_scanType = ST_UNKNOWN;
    g_scanOptions = {};
    g_scanResult = {};
    g_bProcess = false;

    ui->treeViewResult->setModel(0);
}

void DIE_Widget::process()
{
    if (!g_bProcess) {
        enableControls(false);
        g_bProcess = true;
        // ui->progressBarProgress->setValue(0);

        g_scanOptions.bUseCustomDatabase = true;
        g_scanOptions.bUseExtraDatabase = true;
        g_scanOptions.bShowType = true;
        g_scanOptions.bShowVersion = true;
        g_scanOptions.bShowInfo = true;
        g_scanOptions.bLogProfiling = getGlobalOptions()->getValue(XOptions::ID_SCAN_LOG_PROFILING).toBool();
        g_scanOptions.fileType = g_fileType;
        g_scanOptions.bShowScanTime = true;
        g_scanOptions.nBufferSize = getGlobalOptions()->getValue(XOptions::ID_SCAN_BUFFERSIZE).toULongLong();
        g_scanOptions.bIsHighlight = getGlobalOptions()->getValue(XOptions::ID_SCAN_HIGHLIGHT).toBool();
        g_scanOptions.bHideUnknown = getGlobalOptions()->getValue(XOptions::ID_SCAN_HIDEUNKNOWN).toBool();
        g_scanOptions.bIsSort = getGlobalOptions()->getValue(XOptions::ID_SCAN_SORT).toBool();
        // g_scanOptions.scanEngineCallback = _scanEngineCallback;
        // g_scanOptions.pUserData = (void *)123;

        quint64 nFlags = ui->comboBoxFlags->getValue().toULongLong();
        XScanEngine::setScanFlags(&g_scanOptions, nFlags);

        quint64 nDatabases = ui->comboBoxDatabases->getValue().toULongLong();
        XScanEngine::setDatabases(&g_scanOptions, nDatabases);

        XScanEngine::setScanFlagsToGlobalOptions(getGlobalOptions(), nFlags);
        XScanEngine::setDatabasesToGlobalOptions(getGlobalOptions(), nDatabases);

        g_pTimer->start(200);  // TODO const

        ui->progressBar0->hide();
        ui->progressBar1->hide();
        ui->progressBar2->hide();
        ui->progressBar3->hide();
        ui->progressBar4->hide();

#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
        QFuture<void> future = QtConcurrent::run(&DIE_Widget::scan, this);
#else
        QFuture<void> future = QtConcurrent::run(this, &DIE_Widget::scan);
#endif

        g_watcher.setFuture(future);
    } else {
        stop();
        g_watcher.waitForFinished();
        enableControls(true);
    }
}

void DIE_Widget::scan()
{
    g_listErrorsAndWarnings.clear();

    if (g_scanType != ST_UNKNOWN) {
        if (g_scanType == ST_FILE) {
            emit scanStarted();

            g_pdStruct = XBinary::createPdStruct();

            if (!g_bInitDatabase) {
                g_bInitDatabase = g_dieScript.loadDatabaseFromGlobalOptions(getGlobalOptions());
            }

            g_scanResult = g_dieScript.scanFile(g_sFileName, &g_scanOptions, &g_pdStruct);

            if (g_scanResult.ftInit == XBinary::FT_COM) {
                emit currentFileType(g_scanResult.ftInit);
            }

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
    g_bProcess = false;

    g_pTimer->stop();

    qint32 nNumberOfErrors = g_scanResult.listErrors.count() + g_listErrorsAndWarnings.count();

    QString sLogButtonText;

    if (nNumberOfErrors) {
        sLogButtonText = QString("%1(%2)").arg(tr("Log"), QString::number(nNumberOfErrors));
    } else {
        sLogButtonText = tr("Log");
    }

    ui->pushButtonDieLog->setText(sLogButtonText);
    ui->pushButtonDieLog->setEnabled(nNumberOfErrors);

    ui->toolButtonElapsedTime->setText(QString("%1 %2").arg(g_scanResult.nScanTime).arg(tr("msec")));

    // QAbstractItemModel *pOldModel = ui->treeViewResult->model();
    ScanItemModel *pOldModel = g_pModel;

    g_pModel = new ScanItemModel(&g_scanOptions, &(g_scanResult.listRecords), 3);
    ui->treeViewResult->setModel(g_pModel);
    ui->treeViewResult->expandAll();

    if (pOldModel) {
        delete pOldModel;
    }

    //    ui->tableWidgetResult->horizontalHeader()->setVisible(true);
    ////        ui->tableWidgetResult->horizontalHeader()->setFixedHeight(0);

    //    ui->tableWidgetResult->horizontalHeader()->setSectionResizeMode(COLUMN_TYPE,QHeaderView::ResizeToContents);
    ui->treeViewResult->header()->setSectionResizeMode(COLUMN_STRING, QHeaderView::Stretch);
    ui->treeViewResult->header()->setSectionResizeMode(COLUMN_SIGNATURE, QHeaderView::Fixed);
    ui->treeViewResult->header()->setSectionResizeMode(COLUMN_INFO, QHeaderView::Fixed);

    ui->treeViewResult->setColumnWidth(COLUMN_SIGNATURE, 20);
    ui->treeViewResult->setColumnWidth(COLUMN_INFO, 20);

    ui->treeViewResult->header()->setVisible(false);

    //    ui->progressBarProgress->setMaximum(100);
    //    ui->progressBarProgress->setValue(100);
    enableControls(true);
}

void DIE_Widget::on_pushButtonDieSignatures_clicked()
{
    if (g_sFileName != "") {
        QFile file;
        file.setFileName(g_sFileName);

        if (file.open(QIODevice::ReadOnly)) {
            DialogDIESignatures dialogSignatures(this, &g_dieScript);
            dialogSignatures.setGlobal(getShortcuts(), getGlobalOptions());
            dialogSignatures.setData(&file, g_scanOptions.fileType, "");

            dialogSignatures.exec();

            file.close();
        }
    }
}

void DIE_Widget::on_pushButtonDieExtraInformation_clicked()
{
    DialogTextInfo dialogInfo(this);
    dialogInfo.setGlobal(getShortcuts(), getGlobalOptions());

    ScanItemModel model(&g_scanOptions, &(g_scanResult.listRecords), 1);

    dialogInfo.setText(model.toFormattedString());

    dialogInfo.exec();
}

void DIE_Widget::on_pushButtonDieLog_clicked()
{
    DialogTextInfo dialogInfo(this);
    dialogInfo.setGlobal(getShortcuts(), getGlobalOptions());

    QList<QString> listMessages;
    listMessages.append(g_listErrorsAndWarnings);
    listMessages.append(DiE_Script::getErrorsAndWarningsStringList(&g_scanResult));

    dialogInfo.setStringList(listMessages);
    dialogInfo.setTitle(tr("Log"));

    dialogInfo.exec();
}

void DIE_Widget::showInfo(const QString &sName)
{
    if (sName != "") {
        QString sFileName = getInfoFileName(sName);

        if (XBinary::isFileExists(sFileName)) {
            DialogTextInfo dialogInfo(this);
            dialogInfo.setGlobal(getShortcuts(), getGlobalOptions());
            dialogInfo.setFileName(sFileName);

            dialogInfo.exec();
        } else {
            QString _sName = QUrl::toPercentEncoding(sName);
            QString sLink = QString("http://www.google.com/search?q=%1").arg(_sName);  // TODO Set Search Engine
            QDesktopServices::openUrl(QUrl(sLink));
        }
    }
}

void DIE_Widget::showSignature(XBinary::FT fileType, const QString &sName)
{
    if (sName != "") {
        QFile file;
        file.setFileName(g_sFileName);

        if (file.open(QIODevice::ReadOnly)) {
            DialogDIESignatures dialogSignatures(this, &g_dieScript);
            dialogSignatures.setGlobal(getShortcuts(), getGlobalOptions());
            dialogSignatures.setData(&file, fileType, sName);

            dialogSignatures.exec();

            file.close();
        }
    }
}

void DIE_Widget::enableControls(bool bState)
{
    if (!bState) {
        ScanItemModel *pOldModel = g_pModel;
        ui->treeViewResult->setModel(0);

        if (pOldModel) {
            delete pOldModel;
            g_pModel = nullptr;
        }
    }

    ui->treeViewResult->setEnabled(bState);
    ui->comboBoxFlags->setEnabled(bState);
    ui->pushButtonDieSignatures->setEnabled(bState);
    ui->pushButtonDieLog->setEnabled(bState);
    ui->pushButtonDieExtraInformation->setEnabled(bState);
    ui->toolButtonElapsedTime->setEnabled(bState);
    ui->pushButtonDieScanDirectory->setEnabled(bState);

    if (bState) {
        ui->stackedWidgetDieScan->setCurrentIndex(0);
    } else {
        ui->stackedWidgetDieScan->setCurrentIndex(1);
    }
}

QString DIE_Widget::getInfoFileName(const QString &sName)
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
    DialogDIEScanDirectory dds(this, QFileInfo(g_sFileName).absolutePath());
    dds.setGlobal(getShortcuts(), getGlobalOptions());
    dds.exec();
}

void DIE_Widget::registerShortcuts(bool bState)
{
    Q_UNUSED(bState)
}

void DIE_Widget::on_toolButtonElapsedTime_clicked()
{
    DialogDIESignaturesElapsed dialogElapsed(this);
    dialogElapsed.setGlobal(getShortcuts(), getGlobalOptions());
    dialogElapsed.setData(&g_scanResult);

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

            // TODO more
            QMenu contextMenu(this);  // TODO

            QAction actionCopy(QString("%1 \"%2\"").arg(tr("Copy as"), sString), this);
            connect(&actionCopy, SIGNAL(triggered()), this, SLOT(copyResult()));

            contextMenu.addAction(&actionCopy);

            contextMenu.exec(ui->treeViewResult->viewport()->mapToGlobal(pos));
        }
    }
}

void DIE_Widget::timerSlot()
{
    XFormats::setProgressBar(ui->progressBar0, g_pdStruct._pdRecord[0]);
    XFormats::setProgressBar(ui->progressBar1, g_pdStruct._pdRecord[1]);
    XFormats::setProgressBar(ui->progressBar2, g_pdStruct._pdRecord[2]);
    XFormats::setProgressBar(ui->progressBar3, g_pdStruct._pdRecord[3]);
    XFormats::setProgressBar(ui->progressBar4, g_pdStruct._pdRecord[4]);

    qint64 nOverallTotal = 0;

    for (int i = 0; i < 5; i++) {
        nOverallCurrent += g_pdStruct._pdRecord[i].nCurrent;
        nOverallTotal += g_pdStruct._pdRecord[i].nTotal;
    }

    qint32 nOverallProgress = 0;
    if (nOverallTotal > 0) {
        nOverallProgress = (qint32)((qreal)nOverallCurrent / nOverallTotal * 100);
    }

    emit scanProgress(nOverallProgress);
}

void DIE_Widget::on_pushButtonDieScanStart_clicked()
{
    ui->pushButtonDieScanStart->setEnabled(false);
    process();
    ui->pushButtonDieScanStart->setEnabled(true);
}

void DIE_Widget::on_pushButtonDieScanStop_clicked()
{
    ui->pushButtonDieScanStop->setEnabled(false);
    process();
    ui->pushButtonDieScanStop->setEnabled(true);
}

void DIE_Widget::handleErrorString(const QString &sErrorString)
{
    g_listErrorsAndWarnings.append(sErrorString);
}

void DIE_Widget::handleWarningString(const QString &sWarningString)
{
    g_listErrorsAndWarnings.append(sWarningString);
}

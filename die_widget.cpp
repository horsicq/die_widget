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

    m_pdStruct = XBinary::createPdStruct();
    m_pModel = nullptr;
    m_bProcess = false;

    connect(&m_watcher, SIGNAL(finished()), this, SLOT(onScanFinished()));

    connect(&m_dieScript, SIGNAL(errorMessage(QString)), this, SLOT(handleErrorString(QString)));
    connect(&m_dieScript, SIGNAL(warningMessage(QString)), this, SLOT(handleWarningString(QString)));
    // connect(&m_dieScript, SIGNAL(infoMessage(QString)), this, SLOT(handleInfoString(QString)));

    ui->pushButtonDieLog->setEnabled(false);

    m_pTimer = new QTimer(this);
    connect(m_pTimer, SIGNAL(timeout()), this, SLOT(timerSlot()));

    clear();

    m_bInitDatabase = false;

    ui->comboBoxFlags->setData(XScanEngine::getScanFlags(), XComboBoxEx::CBTYPE_FLAGS, 0, tr("Flags"));
    ui->comboBoxDatabases->setData(XScanEngine::getDatabases(), XComboBoxEx::CBTYPE_FLAGS, 0, tr("Database"));

    ui->comboBoxDatabases->setItemEnabled(1, false);

    ui->stackedWidgetDieScan->setCurrentIndex(0);

    ui->toolButtonElapsedTime->setText(QString("%1 %2").arg(0).arg(tr("msec")));  // TODO Function
}

DIE_Widget::~DIE_Widget()
{
    if (m_bProcess) {
        stop();
        m_watcher.waitForFinished();
    }

    delete ui;
}

// void DIE_Widget::setOptions(DIE_Widget::OPTIONS *pOptions)
//{
//     ui->checkBoxRecursiveScan->setChecked(pOptions->bRecursiveScan);
//     ui->checkBoxDeepScan->setChecked(pOptions->bDeepScan);
//     ui->checkBoxAllTypes->setChecked(pOptions->bAllTypesScan);

//    if(m_dieScript.getDatabasePath()!=pOptions->sDatabasePath)
//    {
//        m_dieScript.loadDatabase(pOptions->sDatabasePath);
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

    this->m_sFileName = sFileName;
    this->m_fileType = fileType;
    m_scanType = ST_FILE;

    if (bScan) {
        process();
    }
}

void DIE_Widget::adjustView()
{
    this->m_sInfoPath = getGlobalOptions()->getInfoPath();
    m_bInitDatabase = false;

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
    m_scanType = ST_UNKNOWN;
    m_scanOptions = {};
    m_scanResult = {};
    m_bProcess = false;

    ui->treeViewResult->setModel(0);
}

void DIE_Widget::process()
{
    if (!m_bProcess) {
        enableControls(false);
        m_bProcess = true;
        // ui->progressBarProgress->setValue(0);

        m_scanOptions.bUseCustomDatabase = true;
        m_scanOptions.bUseExtraDatabase = true;
        m_scanOptions.bShowType = true;
        m_scanOptions.bShowVersion = true;
        m_scanOptions.bShowInfo = true;
        m_scanOptions.bLogProfiling = getGlobalOptions()->getValue(XOptions::ID_SCAN_LOG_PROFILING).toBool();
        m_scanOptions.fileType = m_fileType;
        m_scanOptions.bShowScanTime = true;
        m_scanOptions.nBufferSize = getGlobalOptions()->getValue(XOptions::ID_ENGINE_BUFFERSIZE).toULongLong();
        m_scanOptions.bIsHighlight = getGlobalOptions()->getValue(XOptions::ID_SCAN_HIGHLIGHT).toBool();
        m_scanOptions.bHideUnknown = getGlobalOptions()->getValue(XOptions::ID_SCAN_HIDEUNKNOWN).toBool();
        m_scanOptions.bIsSort = getGlobalOptions()->getValue(XOptions::ID_SCAN_SORT).toBool();
        // m_scanOptions.scanEngineCallback = _scanEngineCallback;
        // m_scanOptions.pUserData = (void *)123;

        quint64 nFlags = ui->comboBoxFlags->getValue().toULongLong();
        XScanEngine::setScanFlags(&m_scanOptions, nFlags);

        quint64 nDatabases = ui->comboBoxDatabases->getValue().toULongLong();
        XScanEngine::setDatabases(&m_scanOptions, nDatabases);

        XScanEngine::setScanFlagsToGlobalOptions(getGlobalOptions(), nFlags);
        XScanEngine::setDatabasesToGlobalOptions(getGlobalOptions(), nDatabases);

        m_pTimer->start(200);  // TODO const

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

        m_watcher.setFuture(future);
    } else {
        stop();
        m_watcher.waitForFinished();
        enableControls(true);
    }
}

void DIE_Widget::scan()
{
    m_listErrorsAndWarnings.clear();

    if (m_scanType != ST_UNKNOWN) {
        if (m_scanType == ST_FILE) {
            emit scanStarted();

            m_pdStruct = XBinary::createPdStruct();

            if (!m_bInitDatabase) {
                m_bInitDatabase = m_dieScript.loadDatabaseFromGlobalOptions(getGlobalOptions());
            }

            m_scanResult = m_dieScript.scanFile(m_sFileName, &m_scanOptions, &m_pdStruct);

            if (m_scanResult.ftInit == XBinary::FT_COM) {
                emit currentFileType(m_scanResult.ftInit);
            }

            emit scanFinished();
        }
    }
}

void DIE_Widget::stop()
{
    m_pdStruct.bIsStop = true;
}

void DIE_Widget::onScanFinished()
{
    m_bProcess = false;

    m_pTimer->stop();

    qint32 nNumberOfErrors = m_scanResult.listErrors.count() + m_listErrorsAndWarnings.count();

    QString sLogButtonText;

    if (nNumberOfErrors) {
        sLogButtonText = QString("%1(%2)").arg(tr("Log"), QString::number(nNumberOfErrors));
    } else {
        sLogButtonText = tr("Log");
    }

    ui->pushButtonDieLog->setText(sLogButtonText);
    ui->pushButtonDieLog->setEnabled(nNumberOfErrors);

    ui->toolButtonElapsedTime->setText(QString("%1 %2").arg(m_scanResult.nScanTime).arg(tr("msec")));

    // QAbstractItemModel *pOldModel = ui->treeViewResult->model();
    ScanItemModel *pOldModel = m_pModel;

    m_pModel = new ScanItemModel(&m_scanOptions, &(m_scanResult.listRecords), 3);
    ui->treeViewResult->setModel(m_pModel);
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
    if (m_sFileName != "") {
        QFile file;
        file.setFileName(m_sFileName);

        if (file.open(QIODevice::ReadOnly)) {
            DialogDIESignatures dialogSignatures(this, &m_dieScript);
            dialogSignatures.setGlobal(getShortcuts(), getGlobalOptions());
            dialogSignatures.setData(&file, m_scanOptions.fileType, "");

            dialogSignatures.exec();

            file.close();
        }
    }
}

void DIE_Widget::on_pushButtonDieExtraInformation_clicked()
{
    DialogTextInfo dialogInfo(this);
    dialogInfo.setGlobal(getShortcuts(), getGlobalOptions());

    ScanItemModel model(&m_scanOptions, &(m_scanResult.listRecords), 1);

    dialogInfo.setText(model.toFormattedString());

    dialogInfo.exec();
}

void DIE_Widget::on_pushButtonDieLog_clicked()
{
    DialogTextInfo dialogInfo(this);
    dialogInfo.setGlobal(getShortcuts(), getGlobalOptions());

    QList<QString> listMessages;
    listMessages.append(m_listErrorsAndWarnings);
    listMessages.append(DiE_Script::getErrorsAndWarningsStringList(&m_scanResult));

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
        file.setFileName(m_sFileName);

        if (file.open(QIODevice::ReadOnly)) {
            DialogDIESignatures dialogSignatures(this, &m_dieScript);
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
        ScanItemModel *pOldModel = m_pModel;
        ui->treeViewResult->setModel(0);

        if (pOldModel) {
            delete pOldModel;
            m_pModel = nullptr;
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
    QString sResult = XBinary::convertPathName(m_sInfoPath) + QDir::separator() + QString("html") + QDir::separator() + QString("%1.html").arg(sName);

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
    DialogDIEScanDirectory dds(this, QFileInfo(m_sFileName).absolutePath());
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
    dialogElapsed.setData(&m_scanResult);

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
    XFormats::setProgressBar(ui->progressBar0, m_pdStruct._pdRecord[0]);
    XFormats::setProgressBar(ui->progressBar1, m_pdStruct._pdRecord[1]);
    XFormats::setProgressBar(ui->progressBar2, m_pdStruct._pdRecord[2]);
    XFormats::setProgressBar(ui->progressBar3, m_pdStruct._pdRecord[3]);
    XFormats::setProgressBar(ui->progressBar4, m_pdStruct._pdRecord[4]);

    qint64 nOverallCurrent = 0;
    qint64 nOverallTotal = 0;

    for (int i = 0; i < 5; i++) {
        nOverallCurrent += m_pdStruct._pdRecord[i].nCurrent;
        nOverallTotal += m_pdStruct._pdRecord[i].nTotal;
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
    m_listErrorsAndWarnings.append(sErrorString);
}

void DIE_Widget::handleWarningString(const QString &sWarningString)
{
    m_listErrorsAndWarnings.append(sWarningString);
}

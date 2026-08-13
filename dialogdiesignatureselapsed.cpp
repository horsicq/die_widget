/* Copyright (c) 2017-2026 hors<horsicq@gmail.com>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */
#include "dialogdiesignatureselapsed.h"

#include <QAction>
#include <QApplication>
#include <QClipboard>
#include <QHeaderView>
#include <QLocale>
#include <QSignalBlocker>

#include "ui_dialogdiesignatureselapsed.h"

DialogDIESignaturesElapsed::DialogDIESignaturesElapsed(QWidget *pParent)
    : XShortcutsDialog(pParent, true), ui(new Ui::DialogDIESignaturesElapsed) {
  ui->setupUi(this);

  ui->tableWidgetResult->setColumnCount(2);
  ui->tableWidgetResult->setHorizontalHeaderLabels(
      {tr("Duration (ms)"), tr("Script")});
  ui->tableWidgetResult->horizontalHeader()->setSectionResizeMode(
      0, QHeaderView::ResizeToContents);
  ui->tableWidgetResult->horizontalHeader()->setSectionResizeMode(
      1, QHeaderView::Stretch);
  ui->tableWidgetResult->horizontalHeader()->setMinimumSectionSize(80);
  ui->tableWidgetResult->setAccessibleName(tr("Signature timing results"));
  ui->tableWidgetResult->setAccessibleDescription(
      tr("Sortable table of elapsed time in milliseconds and script name"));
  ui->tableWidgetResult->setToolTip(
      tr("Select one or more rows to copy their timing details."));

  ui->labelResults->setBuddy(ui->tableWidgetResult);
  ui->labelStatus->setAccessibleDescription(
      tr("Summary of the available signature timing data"));
  ui->pushButtonCopy->setAccessibleDescription(
      tr("Copy the selected timing rows to the clipboard"));
  ui->pushButtonOK->setAccessibleDescription(
      tr("Close the signature timing dialog"));

  m_pActionCopy = new QAction(tr("Copy"), this);
  m_pActionCopy->setObjectName(QStringLiteral("actionCopy"));
  m_pActionCopy->setShortcut(QKeySequence::Copy);
  m_pActionCopy->setShortcutContext(Qt::WindowShortcut);
  m_pActionCopy->setEnabled(false);
  ui->tableWidgetResult->addAction(m_pActionCopy);

  connect(ui->pushButtonCopy, &QPushButton::clicked, this,
          &DialogDIESignaturesElapsed::copySelectedRows);
  connect(m_pActionCopy, &QAction::triggered, this,
          &DialogDIESignaturesElapsed::copySelectedRows);
  connect(ui->tableWidgetResult->selectionModel(),
          &QItemSelectionModel::selectionChanged, this,
          [this]() { _updateCopyState(); });

  ui->tableWidgetResult->setEnabled(false);
  ui->labelStatus->setText(tr("No timing data loaded."));
  _updateCopyState();
}

DialogDIESignaturesElapsed::~DialogDIESignaturesElapsed() { delete ui; }

void DialogDIESignaturesElapsed::adjustView() {
  getGlobalOptions()->adjustWidget(this, XOptions::ID_VIEW_FONT_CONTROLS);
}

void DialogDIESignaturesElapsed::setData(
    XScanEngine::SCAN_RESULT *pScanResult) {
  _setData(pScanResult);
}

void DialogDIESignaturesElapsed::setData(
    const XScanEngine::SCAN_RESULT &scanResult) {
  _setData(&scanResult);
}

void DialogDIESignaturesElapsed::_setData(
    const XScanEngine::SCAN_RESULT *pScanResult) {
  QTableWidget *pTable = ui->tableWidgetResult;
  const bool bSortingEnabled = pTable->isSortingEnabled();
  const QSignalBlocker signalBlocker(pTable);

  pTable->setUpdatesEnabled(false);
  pTable->setSortingEnabled(false);
  pTable->clearContents();
  pTable->setRowCount(0);
  pTable->clearSelection();
  ui->pushButtonCopy->setEnabled(false);
  m_pActionCopy->setEnabled(false);

  if (pScanResult == nullptr) {
    pTable->setEnabled(false);
    ui->labelStatus->setText(tr("Timing results are unavailable."));
    pTable->setSortingEnabled(bSortingEnabled);
    pTable->setUpdatesEnabled(true);
    return;
  }

  const QList<XScanEngine::DEBUG_RECORD> listRecords =
      pScanResult->listDebugRecords;
  const qint64 nScanTime = pScanResult->nScanTime;
  const qint32 nNumberOfRecords = listRecords.count();

  pTable->setRowCount(nNumberOfRecords);

  for (qint32 i = 0; i < nNumberOfRecords; i++) {
    const XScanEngine::DEBUG_RECORD &record = listRecords.at(i);
    QTableWidgetItem *pItemTime = new QTableWidgetItem;

    pItemTime->setData(Qt::DisplayRole, record.nElapsedTime);
    pItemTime->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
    pItemTime->setToolTip(
        tr("%1 milliseconds").arg(QLocale().toString(record.nElapsedTime)));
    pItemTime->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
    pTable->setItem(i, 0, pItemTime);

    QTableWidgetItem *pItemScript = new QTableWidgetItem;

    pItemScript->setText(record.sScript);
    pItemScript->setTextAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    pItemScript->setToolTip(record.sScript);
    pItemScript->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
    pTable->setItem(i, 1, pItemScript);
  }

  pTable->setSortingEnabled(bSortingEnabled);
  if (bSortingEnabled && nNumberOfRecords) {
    pTable->sortItems(0, Qt::DescendingOrder);
  }
  pTable->setEnabled(nNumberOfRecords != 0);
  pTable->setUpdatesEnabled(true);
  pTable->scrollToTop();

  if (nNumberOfRecords) {
    const QString sRecordCount =
        (nNumberOfRecords == 1)
            ? tr("1 timing record.")
            : tr("%1 timing records.")
                  .arg(QLocale().toString(nNumberOfRecords));
    ui->labelStatus->setText(
        tr("%1 Total scan time: %2 ms.")
            .arg(sRecordCount, QLocale().toString(nScanTime)));
  } else {
    ui->labelStatus->setText(
        tr("No per-script timing data was recorded. Total scan time: %1 ms.")
            .arg(QLocale().toString(nScanTime)));
  }
}

void DialogDIESignaturesElapsed::_updateCopyState() {
  const bool bCanCopy =
      !ui->tableWidgetResult->selectionModel()->selectedRows().isEmpty();
  ui->pushButtonCopy->setEnabled(bCanCopy);
  m_pActionCopy->setEnabled(bCanCopy);
}

void DialogDIESignaturesElapsed::copySelectedRows() {
  const QModelIndexList listRows =
      ui->tableWidgetResult->selectionModel()->selectedRows();
  QStringList listLines;

  for (const QModelIndex &index : listRows) {
    const qint32 nRow = index.row();
    const QTableWidgetItem *pTimeItem = ui->tableWidgetResult->item(nRow, 0);
    const QTableWidgetItem *pScriptItem = ui->tableWidgetResult->item(nRow, 1);

    if (pTimeItem && pScriptItem) {
      listLines.append(QString("%1\t%2")
                           .arg(pTimeItem->data(Qt::DisplayRole).toLongLong())
                           .arg(pScriptItem->text()));
    }
  }

  if (!listLines.isEmpty()) {
    QApplication::clipboard()->setText(listLines.join(QLatin1Char('\n')));
  }
}

void DialogDIESignaturesElapsed::on_pushButtonOK_clicked() { reject(); }

void DialogDIESignaturesElapsed::registerShortcuts(bool bState) {
  Q_UNUSED(bState)
}

/* Copyright (c) 2017-2022 hors<horsicq@gmail.com>
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
#include "dialogdiescanprocess.h"
#include "ui_dialogdiescanprocess.h"

DialogDIEScanProcess::DialogDIEScanProcess(QWidget *pParent) :
    QDialog(pParent),
    ui(new Ui::DialogDIEScanProcess)
{
    ui->setupUi(this);

    g_pDieScript=new DiE_Script;
    g_pThread=new QThread;

    g_pDieScript->moveToThread(g_pThread);

    connect(g_pThread, SIGNAL(started()), g_pDieScript, SLOT(processDirectory()));
    connect(g_pDieScript, SIGNAL(directoryScanCompleted(qint64)), this, SLOT(onCompleted(qint64)));
    connect(g_pDieScript, SIGNAL(directoryScanFileStarted(QString)),this,SIGNAL(scanFileStarted(QString)),Qt::DirectConnection);
    connect(g_pDieScript, SIGNAL(directoryScanResult(DiE_Script::SCAN_RESULT)),this,SIGNAL(scanResult(DiE_Script::SCAN_RESULT)),Qt::DirectConnection);

    g_pTimer=new QTimer(this);
    connect(g_pTimer, SIGNAL(timeout()), this, SLOT(timerSlot()));

    g_bIsRun=false;
}

void DialogDIEScanProcess::setData(QString sDirectoryName, DiE_Script::SCAN_OPTIONS options, QString sDatabasePath)
{
    g_bIsRun=true;
    g_pDieScript->loadDatabase(sDatabasePath);
    g_pDieScript->setProcessDirectory(sDirectoryName,options);
    g_pThread->start();
    g_pTimer->start(N_REFRESH_DELAY);
    ui->progressBarTotal->setMaximum(100);
}

DialogDIEScanProcess::~DialogDIEScanProcess()
{
    if(g_bIsRun)
    {
        g_pDieScript->stop();
    }

    g_pTimer->stop();

    g_pThread->quit();
    g_pThread->wait();

    delete ui;

    g_pThread->deleteLater(); // TODO !!!
    g_pDieScript->deleteLater(); // TODO !!!
}

void DialogDIEScanProcess::on_pushButtonCancel_clicked()
{
    if(g_bIsRun)
    {
        g_pDieScript->stop();
        g_pTimer->stop();
        g_bIsRun=false;
    }
}

void DialogDIEScanProcess::onCompleted(qint64 nElapsed)
{
    Q_UNUSED(nElapsed)

    g_bIsRun=false;
    this->close();
}

void DialogDIEScanProcess::onSetProgressMaximum(int nValue)
{
    ui->progressBarTotal->setMaximum(nValue);
}

void DialogDIEScanProcess::onSetProgressValueChanged(int nValue)
{
    ui->progressBarTotal->setValue(nValue);
}

void DialogDIEScanProcess::timerSlot()
{
    DiE_Script::DIRECTORYSTATS stats=g_pDieScript->getCurrentDirectoryStats();

    ui->labelTotal->setText(QString::number(stats.nTotal));
    ui->labelCurrent->setText(QString::number(stats.nCurrent));
    ui->labelCurrentStatus->setText(stats.sStatus);

    if(stats.nTotal)
    {
        ui->progressBarTotal->setValue((int)((stats.nCurrent*100)/stats.nTotal));
    }

    QDateTime dt;
    dt.setMSecsSinceEpoch(stats.nElapsed);
    QString sDateTime=dt.time().addSecs(-60*60).toString("hh:mm:ss");

    ui->labelTime->setText(sDateTime);
}

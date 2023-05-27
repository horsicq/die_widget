/* Copyright (c) 2017-2023 hors<horsicq@gmail.com>
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

DialogDIEScanProcess::DialogDIEScanProcess(QWidget *pParent) : XDialogProcess(pParent)
{
    g_pDieScript = new DiE_Script;
    g_pThread = new QThread;

    g_pDieScript->moveToThread(g_pThread);

    connect(g_pThread, SIGNAL(started()), g_pDieScript, SLOT(scanDirectory()));
    connect(g_pDieScript, SIGNAL(directoryScanCompleted(qint64)), this, SLOT(onCompleted(qint64)));
    connect(g_pDieScript, SIGNAL(directoryScanFileStarted(QString)), this, SIGNAL(scanFileStarted(QString)), Qt::DirectConnection);
    connect(g_pDieScript, SIGNAL(directoryScanResult(DiE_Script::SCAN_RESULT)), this, SIGNAL(scanResult(DiE_Script::SCAN_RESULT)), Qt::DirectConnection);
}

void DialogDIEScanProcess::setData(const QString &sDirectoryName, DiE_Script::OPTIONS options, const QString &sDatabasePath)
{
    g_pDieScript->loadDatabase(sDatabasePath);
    g_pDieScript->setProcessDirectory(sDirectoryName, options, getPdStruct());
    g_pThread->start();
}

DialogDIEScanProcess::~DialogDIEScanProcess()
{
    g_pThread->quit();
    g_pThread->wait();

    //    g_pThread->deleteLater(); // TODO !!!
    //    g_pDieScript->deleteLater(); // TODO !!!
    delete g_pThread;
    delete g_pDieScript;
}

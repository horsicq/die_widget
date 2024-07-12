/* Copyright (c) 2017-2024 hors<horsicq@gmail.com>
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

DialogDIEScanProcess::DialogDIEScanProcess(QWidget *pParent, DiE_Script *pDieScript) : XDialogProcess(pParent)
{
    g_pDieScript = pDieScript;
    g_pThread = new QThread;

    g_pDieScript->moveToThread(g_pThread);

    connect(g_pThread, SIGNAL(started()), g_pDieScript, SLOT(process()));
    connect(g_pDieScript, SIGNAL(completed(qint64)), this, SLOT(onCompleted(qint64)));
}

void DialogDIEScanProcess::setData(const QString &sDirectoryName, XScanEngine::SCAN_OPTIONS *pOptions)
{
    g_pDieScript->setData(sDirectoryName, pOptions, getPdStruct());

    connect(g_pDieScript, SIGNAL(scanFileStarted(QString)), this, SIGNAL(scanFileStarted(QString)), Qt::DirectConnection);
    connect(g_pDieScript, SIGNAL(scanResult(const XScanEngine::SCAN_RESULT &)), this, SIGNAL(scanResult(const XScanEngine::SCAN_RESULT &)), Qt::DirectConnection);

    g_pThread->start();
}

void DialogDIEScanProcess::setData(QIODevice *pDevice, XScanEngine::SCAN_OPTIONS *pOptions, XScanEngine::SCAN_RESULT *pScanResult)
{
    g_pDieScript->setData(pDevice, pOptions, pScanResult, getPdStruct());

    g_pThread->start();
}

DialogDIEScanProcess::~DialogDIEScanProcess()
{
    g_pThread->quit();
    g_pThread->wait();

    //    g_pThread->deleteLater(); // TODO !!!
    //    g_pDieScript->deleteLater(); // TODO !!!
    delete g_pThread;
}

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
#ifndef DIALOGDIESCANPROCESS_H
#define DIALOGDIESCANPROCESS_H

#include <QDateTime>
#include <QDialog>
#include <QFileDialog>
#include <QStandardItemModel>
#include <QThread>
#include <QTimer>

#include "die_script.h"
#include "xdialogprocess.h"

namespace Ui {
class DialogDIEScanProcess;
}

class DialogDIEScanProcess : public XDialogProcess {
    Q_OBJECT

public:
    explicit DialogDIEScanProcess(QWidget *pParent = nullptr);
    ~DialogDIEScanProcess();

    void setData(QString sDirectoryName, DiE_Script::OPTIONS options, QString sDatabasePath);

signals:
    void scanFileStarted(QString sFileName);
    void scanResult(DiE_Script::SCAN_RESULT scanResult);

private:
    static const qint32 N_REFRESH_DELAY = 1000;  // msec TODO Check mb set/get functions
    DiE_Script *g_pDieScript;
    QThread *g_pThread;
};

#endif  // DIALOGDIESCANPROCESS_H

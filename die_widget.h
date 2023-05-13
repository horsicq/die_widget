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
#ifndef DIE_WIDGET_H
#define DIE_WIDGET_H

#include <QClipboard>
#include <QDesktopServices>
#include <QFutureWatcher>
#include <QMenu>
#include <QWidget>
#include <QtConcurrent>

#include "dialogdiescandirectory.h"
#include "dialogelapsed.h"
#include "dialoglog.h"
#include "dialogsignatures.h"
#include "dialogtextinfo.h"
#include "die_script.h"
#include "xshortcutswidget.h"

namespace Ui {
class DIE_Widget;
}

class DIE_Widget : public XShortcutsWidget {
    Q_OBJECT

public:
    enum ST {
        ST_UNKNOWN = 0,
        ST_FILE
    };

    //    struct OPTIONS
    //    {
    //        bool bRecursiveScan;
    //        bool bDeepScan;
    //        bool bAllTypesScan;
    //        QString sDatabasePath;
    //        QString sInfoPath; // TODO make zip
    //    };

    enum COLUMN {
        //        COLUMN_TYPE=0,
        COLUMN_STRING = 0,
        COLUMN_SIGNATURE,
        COLUMN_INFO
    };

    explicit DIE_Widget(QWidget *pParent = nullptr);
    ~DIE_Widget();

    //    void setOptions(OPTIONS *pOptions);
    void setData(const QString &sFileName, bool bScan = false, XBinary::FT fileType = XBinary::FT_UNKNOWN);
    void adjustView();
    void setGlobal(XShortcuts *pShortcuts, XOptions *pXOptions);

private slots:
    void on_pushButtonDieScan_clicked();
    void clear();
    void process();
    void scan();
    void stop();
    void onScanFinished();
    void on_pushButtonDieSignatures_clicked();
    void on_pushButtonDieExtraInformation_clicked();
    void on_pushButtonDieLog_clicked();
    void showInfo(QString sName);
    void showSignature(XBinary::FT fileType, QString sName);
    void enableControls(bool bState);
    QString getInfoFileName(QString sName);
    void copyResult();
    void on_pushButtonDieScanDirectory_clicked();
    void on_toolButtonElapsedTime_clicked();
    void on_treeViewResult_clicked(const QModelIndex &index);
    void on_treeViewResult_customContextMenuRequested(const QPoint &pos);
    void timerSlot();

protected:
    virtual void registerShortcuts(bool bState);

signals:
    void scanStarted();
    void scanFinished();

private:
    Ui::DIE_Widget *ui;
    ST g_scanType;
    DiE_Script g_dieScript;
    DiE_Script::OPTIONS g_scanOptions;
    DiE_Script::SCAN_RESULT scanResult;
    QFutureWatcher<void> watcher;
    QString sFileName;
    XBinary::FT fileType;
    bool bProcess;
    QString g_sInfoPath;
    XBinary::PDSTRUCT g_pdStruct;
    QTimer *g_pTimer;
};

#endif  // DIE_WIDGET_H

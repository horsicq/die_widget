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
#ifndef DIE_WIDGET_H
#define DIE_WIDGET_H

#include <QWidget>
#include <QFutureWatcher>
#include <QtConcurrent>
#include "die_script.h"
#include "dialogsignatures.h"
#include "dialogtextinfo.h"
#include "dialoglog.h"

namespace Ui {
class DIE_Widget;
}

class DIE_Widget : public QWidget
{
    Q_OBJECT

public:
    enum ST
    {
        ST_UNKNOWN=0,
        ST_FILE
    };

    struct OPTIONS
    {
        bool bDeepScan;
        QString sDatabasePath;
        QString sInfoPath; // TODO make zip
    };

    enum COLUMN
    {
        COLUMN_TYPE=0,
        COLUMN_STRING,
        COLUMN_SIGNATURE,
        COLUMN_INFO
    };

    explicit DIE_Widget(QWidget *pParent=nullptr);
    ~DIE_Widget();

    void setOptions(OPTIONS *pOptions);
    void setData(QString sFileName,bool bScan=false,XBinary::FT fileType=XBinary::FT_UNKNOWN);
    void setDatabasePath(QString sDatabasePath);
    void setInfoPath(QString sInfoPath);

private slots:
    void on_pushButtonDieScan_clicked();
    void clear();
    void process();
    void scan();
    void stop();
    void onScanFinished();
    void onProgressMaximumChanged(qint32 nMaximum);
    void onProgressValueChanged(qint32 nValue);
    void on_pushButtonDieSignatures_clicked();
    void on_pushButtonDieExtraInformation_clicked();
    void on_pushButtonDieLog_clicked();
    void on_tableWidgetResult_cellClicked(int nRow,int nColumn);
    void showInfo(QString sName);
    void showSignature(QString sName);
    void enableControls(bool bState);

    QString getInfoFileName(QString sName);

signals:
    void scanStarted();
    void scanFinished();

private:
    Ui::DIE_Widget *ui;
    ST scanType;
    DiE_Script dieScript;
    DiE_Script::SCAN_OPTIONS scanOptions;
    DiE_Script::SCAN_RESULT scanResult;
    QFutureWatcher<void> watcher;
    QString sFileName;
    XBinary::FT fileType;
    bool bProcess;
    QString sInfoPath;
};

#endif // DIE_WIDGET_H

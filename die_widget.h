// copyright (c) 2019-2020 hors<horsicq@gmail.com>
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
#include "dialoginfo.h"
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
        bool bShowVersion;
        bool bShowOptions;
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

    explicit DIE_Widget(QWidget *parent=nullptr);
    ~DIE_Widget();

    void setOptions(OPTIONS *pOptions);
    void setData(QString sFileName,bool bScan=false);

private slots:
    void on_pushButtonScan_clicked();
    void clear();
    void process();
    void scan();
    void stop();
    void onScanFinished();
    void onProgressMaximumChanged(qint32 nMaximum);
    void onProgressValueChanged(qint32 nValue);
    void on_pushButtonSignatures_clicked();
    void on_pushButtonExtraInformation_clicked();
    void on_pushButtonLog_clicked();
    void on_tableWidgetResult_cellClicked(int row, int column);
    void showInfo(QString sName);
    void showSignature(QString sName);

private:
    Ui::DIE_Widget *ui;
    ST scanType;
    DiE_Script dieScript;
    DiE_Script::SCAN_OPTIONS scanOptions;
    DiE_Script::SCAN_RESULT scanResult;
    QFutureWatcher<void> watcher;
    QString sFileName;
    bool bProcess;
};

#endif // DIE_WIDGET_H

/* Copyright (c) 2018-2024 hors<horsicq@gmail.com>
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
#ifndef DIALOGDIESCANDIRECTORY_H
#define DIALOGDIESCANDIRECTORY_H

#include "dialogdiescanprocess.h"

namespace Ui {
class DialogDIEScanDirectory;
}

class DialogDIEScanDirectory : public QDialog {
    Q_OBJECT

public:
    explicit DialogDIEScanDirectory(QWidget *pParent, const QString &sDirName, const QString &sDatabasePath, const QString &sDatabasePathCustom);
    ~DialogDIEScanDirectory();

private slots:
    void on_pushButtonOpenDirectory_clicked();
    void on_pushButtonScan_clicked();
    void scanDirectory(const QString &sDirectoryName);
    void scanResult(DiE_Script::SCAN_RESULT scanResult);
    void appendResult(const QString &sResult);
    void on_pushButtonOK_clicked();
    void on_pushButtonClear_clicked();
    void on_pushButtonSave_clicked();

signals:
    void resultSignal(const QString &sText);

private:
    Ui::DialogDIEScanDirectory *ui;
    QString g_sDatabasePath;
    QString g_sDatabasePathCustom;
};

#endif  // DIALOGDIESCANDIRECTORY_H

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
#ifndef DIALOGDIESIGNATURESELAPSED_H
#define DIALOGDIESIGNATURESELAPSED_H

#include "die_script.h"
#include "xshortcutsdialog.h"

namespace Ui {
class DialogDIESignaturesElapsed;
}

class QAction;

class DialogDIESignaturesElapsed : public XShortcutsDialog {
  Q_OBJECT

public:
  explicit DialogDIESignaturesElapsed(QWidget *pParent = nullptr);
  ~DialogDIESignaturesElapsed() override;

  void adjustView() override;

  void setData(XScanEngine::SCAN_RESULT *pScanResult);
  void setData(const XScanEngine::SCAN_RESULT &scanResult);

private slots:
  void on_pushButtonOK_clicked();
  void copySelectedRows();

protected:
  void registerShortcuts(bool bState) override;

private:
  void _setData(const XScanEngine::SCAN_RESULT *pScanResult);
  void _updateCopyState();

  Ui::DialogDIESignaturesElapsed *ui;
  QAction *m_pActionCopy;
};

#endif // DIALOGDIESIGNATURESELAPSED_H

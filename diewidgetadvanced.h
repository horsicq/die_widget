/* Copyright (c) 2019-2025 hors<horsicq@gmail.com>
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
#ifndef DIEWIDGETADVANCED_H
#define DIEWIDGETADVANCED_H

#include "xshortcutswidget.h"
#include "die_widget.h"

namespace Ui {
class DIEWidgetAdvanced;
}

class DIEWidgetAdvanced : public XShortcutsWidget {
    Q_OBJECT

public:
    explicit DIEWidgetAdvanced(QWidget *pParent = nullptr);
    ~DIEWidgetAdvanced();

    void setData(QIODevice *pDevice, bool bScan, XBinary::FT fileType = XBinary::FT_UNKNOWN);

    virtual void adjustView();
    void setGlobal(XShortcuts *pShortcuts, XOptions *pXOptions);
    virtual void reloadData(bool bSaveSelection);

protected:
    virtual void registerShortcuts(bool bState);

private slots:
    void process();
    void on_toolButtonSave_clicked();
    void on_toolButtonScan_clicked();
    void onSelectionChanged(const QItemSelection &itemSelected, const QItemSelection &itemDeselected);
    void on_comboBoxType_currentIndexChanged(int nIndex);
    void on_toolButtonSignatures_clicked();

private:
    Ui::DIEWidgetAdvanced *ui;
    QIODevice *m_pDevice;
    ScanItemModel *g_pModel;
    XScanEngine::SCAN_RESULT g_scanResult;
    XScanEngine::SCAN_OPTIONS g_scanOptions;
};

#endif  // DIEWIDGETADVANCED_H

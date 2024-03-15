/* Copyright (c) 2019-2024 hors<horsicq@gmail.com>
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
#ifndef DIALOGDIESIGNATURES_H
#define DIALOGDIESIGNATURES_H

#include <QDialog>
#include <QMainWindow>
#include <QTreeWidgetItem>

#include "dialogfindtext.h"
#include "die_script.h"
#include "xshortcutsdialog.h"

namespace Ui {
class DialogDIESignatures;
}

// TODO rename DialogDieSignatures
class DialogDIESignatures : public XShortcutsDialog {
    Q_OBJECT

    enum SHORTCUT {
        SC_FIND_STRING,
        SC_FIND_NEXT,
        __SC_SIZE,
        // TODO more
    };

    enum UD {
        UD_FILEPATH = 0,
        UD_FILETYPE,
        UD_NAME
    };

public:
    explicit DialogDIESignatures(QWidget *pParent, DiE_Script *pDieScript);
    ~DialogDIESignatures();

    void setData(QIODevice *pDevice, XBinary::FT fileType, const QString &sSignature);

    void adjustView();

private slots:
    void on_treeWidgetSignatures_currentItemChanged(QTreeWidgetItem *pItemCurrent, QTreeWidgetItem *pItemPrevious);
    void on_pushButtonSave_clicked();
    void save();
    void on_pushButtonRun_clicked();
    void on_pushButtonDebug_clicked();
    void on_pushButtonClearResult_clicked();
    void on_pushButtonClose_clicked();

    qint32 handleTreeItems(QTreeWidgetItem *pRootItem, XBinary::FT fileType, const QString &sText);
    qint32 _handleTreeItems(QTreeWidgetItem *pItemParent, XBinary::FT fileType);
    void runScript(const QString &sFunction, bool bIsDebug);
    void on_plainTextEditSignature_textChanged();
    void on_checkBoxReadOnly_toggled(bool bChecked);

    bool _setTreeItem(QTreeWidget *pTree, QTreeWidgetItem *pItem, XBinary::FT fileType, const QString &sSignature);
    void enableControls(bool bState);
    void infoMessage(const QString &sInfoMessage);
    void on_pushButtonFind_clicked();
    void on_pushButtonFindNext_clicked();
    void findString();
    void findNext();

protected:
    virtual void registerShortcuts(bool bState);

private:
    Ui::DialogDIESignatures *ui;
    DiE_Script *pDieScript;
    QIODevice *g_pDevice;
    XBinary::FT g_fileType;
    QString g_sSignature;
    QString sCurrentSignatureFilePath;
    bool g_bCurrentEdited;
    DialogFindText::DATA g_data;
    QShortcut *shortCuts[__SC_SIZE];
};

#endif  // DIALOGDIESIGNATURES_H

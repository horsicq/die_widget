/* Copyright (c) 2019-2022 hors<horsicq@gmail.com>
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
#include "dieoptionswidget.h"
#include "ui_dieoptionswidget.h"

DIEOptionsWidget::DIEOptionsWidget(QWidget *pParent) :
    QWidget(pParent),
    ui(new Ui::DIEOptionsWidget)
{
    ui->setupUi(this);
}

DIEOptionsWidget::~DIEOptionsWidget()
{
    delete ui;
}

void DIEOptionsWidget::setOptions(XOptions *pOptions)
{
    g_pOptions=pOptions;

    reload();
}

void DIEOptionsWidget::save()
{
    g_pOptions->getCheckBox(ui->checkBoxDeepScan,XOptions::ID_SCAN_DEEP);
    g_pOptions->getCheckBox(ui->checkBoxScanAfterOpen,XOptions::ID_SCAN_SCANAFTEROPEN);
    g_pOptions->getCheckBox(ui->checkBoxRecursiveScan,XOptions::ID_SCAN_RECURSIVE);
    g_pOptions->getCheckBox(ui->checkBoxHeuristicScan,XOptions::ID_SCAN_HEURISTIC);
    g_pOptions->getCheckBox(ui->checkBoxVerbose,XOptions::ID_SCAN_VERBOSE);
    g_pOptions->getCheckBox(ui->checkBoxAllTypesScan,XOptions::ID_SCAN_ALLTYPES);
    g_pOptions->getLineEdit(ui->lineEditDIEDatabase,XOptions::ID_SCAN_DATABASEPATH);
    g_pOptions->getLineEdit(ui->lineEditDIEInfo,XOptions::ID_SCAN_INFOPATH);

    if(g_pOptions->isIDPresent(XOptions::ID_SCAN_ENGINE))
    {
        g_pOptions->getComboBox(ui->comboBoxScanEngine,XOptions::ID_SCAN_ENGINE);
    }

    g_pOptions->getLineEdit(ui->lineEditSignaturesEditorFont,XOptions::ID_SCAN_EDITORFONT);
}

void DIEOptionsWidget::reload()
{
    g_pOptions->setCheckBox(ui->checkBoxScanAfterOpen,XOptions::ID_SCAN_SCANAFTEROPEN);
    g_pOptions->setCheckBox(ui->checkBoxRecursiveScan,XOptions::ID_SCAN_RECURSIVE);
    g_pOptions->setCheckBox(ui->checkBoxDeepScan,XOptions::ID_SCAN_DEEP);
    g_pOptions->setCheckBox(ui->checkBoxHeuristicScan,XOptions::ID_SCAN_HEURISTIC);
    g_pOptions->setCheckBox(ui->checkBoxVerbose,XOptions::ID_SCAN_VERBOSE);
    g_pOptions->setCheckBox(ui->checkBoxAllTypesScan,XOptions::ID_SCAN_ALLTYPES);
    g_pOptions->setLineEdit(ui->lineEditDIEDatabase,XOptions::ID_SCAN_DATABASEPATH);
    g_pOptions->setLineEdit(ui->lineEditDIEInfo,XOptions::ID_SCAN_INFOPATH);

    if(g_pOptions->isIDPresent(XOptions::ID_SCAN_ENGINE))
    {
        g_pOptions->setComboBox(ui->comboBoxScanEngine,XOptions::ID_SCAN_ENGINE);
    }

    g_pOptions->setLineEdit(ui->lineEditSignaturesEditorFont,XOptions::ID_SCAN_EDITORFONT);
}

void DIEOptionsWidget::setDefaultValues(XOptions *pOptions)
{
    pOptions->addID(XOptions::ID_SCAN_SCANAFTEROPEN,true);
    pOptions->addID(XOptions::ID_SCAN_RECURSIVE,true);
    pOptions->addID(XOptions::ID_SCAN_DEEP,true);
    pOptions->addID(XOptions::ID_SCAN_HEURISTIC,false);
    pOptions->addID(XOptions::ID_SCAN_VERBOSE,true);
    pOptions->addID(XOptions::ID_SCAN_ALLTYPES,false);
    pOptions->addID(XOptions::ID_SCAN_DATABASEPATH,"$data/db");
    pOptions->addID(XOptions::ID_SCAN_INFOPATH,"$data/info");
    pOptions->addID(XOptions::ID_SCAN_ENGINE,"auto");

#ifdef Q_OS_WIN
    pOptions->addID(XOptions::ID_SCAN_EDITORFONT,"Courier,10,-1,5,50,0,0,0,0,0");
#endif
#ifdef Q_OS_LINUX
    pOptions->addID(XOptions::ID_SCAN_EDITORFONT,"DejaVu Sans Mono,10,-1,5,50,0,0,0,0,0");
#endif
#ifdef Q_OS_MACOS
    pOptions->addID(XOptions::ID_SCAN_EDITORFONT,"Menlo,10,-1,5,50,0,0,0,0,0"); // TODO Check
#endif
}

void DIEOptionsWidget::on_toolButtonDIEDatabase_clicked()
{
    QString sText=ui->lineEditDIEDatabase->text();
    QString sInitDirectory=XBinary::convertPathName(sText);

    QString sDirectoryName=QFileDialog::getExistingDirectory(this,tr("Open directory")+QString("..."),sInitDirectory,QFileDialog::ShowDirsOnly);

    if(!sDirectoryName.isEmpty())
    {
        ui->lineEditDIEDatabase->setText(sDirectoryName);
    }
}

void DIEOptionsWidget::on_toolButtonDIEInfo_clicked()
{
    QString sText=ui->lineEditDIEInfo->text();
    QString sInitDirectory=XBinary::convertPathName(sText);

    QString sDirectoryName=QFileDialog::getExistingDirectory(this,tr("Open directory")+QString("..."),sInitDirectory,QFileDialog::ShowDirsOnly);

    if(!sDirectoryName.isEmpty())
    {
        ui->lineEditDIEInfo->setText(sDirectoryName);
    }
}

void DIEOptionsWidget::on_toolButtonSignaturesEditorFont_clicked()
{
    QFont _font;
    _font.fromString(ui->lineEditSignaturesEditorFont->text());

    bool bOK=false;
    _font=QFontDialog::getFont(&bOK,_font,this);

    if(bOK)
    {
        ui->lineEditSignaturesEditorFont->setText(_font.toString());
    }
}

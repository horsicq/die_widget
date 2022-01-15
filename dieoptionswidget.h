/* Copyright (c) 2019-2021 hors<horsicq@gmail.com>
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
#ifndef DIEOPTIONSWIDGET_H
#define DIEOPTIONSWIDGET_H

#include <QWidget>
#include "xoptions.h"
#include "xbinary.h"

namespace Ui {
class DIEOptionsWidget;
}

class DIEOptionsWidget : public QWidget
{
    Q_OBJECT

public:
    explicit DIEOptionsWidget(QWidget *pParent=nullptr);
    ~DIEOptionsWidget();

    void setOptions(XOptions *pOptions);
    void save();

private slots:
    void on_toolButtonDIEDatabase_clicked();
    void on_toolButtonDIEInfo_clicked();

private:
    Ui::DIEOptionsWidget *ui;
    XOptions *g_pOptions;
};

#endif // DIEOPTIONSWIDGET_H
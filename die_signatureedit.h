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
#ifndef DIE_SIGNATUREEDIT_H
#define DIE_SIGNATUREEDIT_H

#include <QPainter>
#include <QPlainTextEdit>

#include "die_highlighter.h"
#include "xoptions.h"

class DIE_SignatureEdit : public QPlainTextEdit {
    Q_OBJECT

public:
    explicit DIE_SignatureEdit(QWidget *pParent = nullptr);
    void lineNumberAreaPaintEvent(QPaintEvent *pEvent);
    int lineNumberAreaWidth();
    void setPlainText(const QString &sText);

protected:
    virtual void keyPressEvent(QKeyEvent *pEvent) override;
    void resizeEvent(QResizeEvent *pEvent) override;

private slots:
    void updateLineNumberAreaWidth(int newBlockCount);
    void highlightCurrentLine();
    void updateLineNumberArea(const QRect &rect, int nDy);

private:
    DIE_Highlighter *pHighlighter;
    QWidget *g_pLineNumberArea;
};

class DIE_LineNumberArea : public QWidget {
    Q_OBJECT

public:
    DIE_LineNumberArea(QPlainTextEdit *pPlainTextEdit)
        : QWidget(pPlainTextEdit)
    {
        g_pPlainTextEdit = pPlainTextEdit;
    }

    QSize sizeHint() const
    {
        return QSize(((DIE_SignatureEdit *)g_pPlainTextEdit)->lineNumberAreaWidth(), 0);
    }

protected:
    void paintEvent(QPaintEvent *pEvent)
    {
        ((DIE_SignatureEdit *)g_pPlainTextEdit)->lineNumberAreaPaintEvent(pEvent);
    }

private:
    QPlainTextEdit *g_pPlainTextEdit;
};

#endif  // DIE_SIGNATUREEDIT_H

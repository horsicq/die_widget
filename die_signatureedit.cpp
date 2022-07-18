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
#include "die_signatureedit.h"

DIE_SignatureEdit::DIE_SignatureEdit(QWidget *pParent) :
    QPlainTextEdit(pParent)
{
    XOptions::setMonoFont(this);

    pHighlighter=new DIE_Highlighter(this->document());

    pHighlighter->setDocument(this->document());

    g_pLineNumberArea=new DIE_LineNumberArea(this);

    connect(this,&DIE_SignatureEdit::blockCountChanged,this,&DIE_SignatureEdit::updateLineNumberAreaWidth);
    connect(this,&DIE_SignatureEdit::updateRequest,this,&DIE_SignatureEdit::updateLineNumberArea);
    connect(this,&DIE_SignatureEdit::cursorPositionChanged,this,&DIE_SignatureEdit::highlightCurrentLine);

    updateLineNumberAreaWidth(0);
    highlightCurrentLine();
}

void DIE_SignatureEdit::lineNumberAreaPaintEvent(QPaintEvent *pEvent)
{
    QPainter painter(g_pLineNumberArea);
    painter.fillRect(pEvent->rect(),Qt::lightGray);

    QTextBlock block=firstVisibleBlock();
    int blockNumber=block.blockNumber();
    int top=qRound(blockBoundingGeometry(block).translated(contentOffset()).top());
    int bottom=top+qRound(blockBoundingRect(block).height());

    while(block.isValid()&&(top<=pEvent->rect().bottom()))
    {
        if(block.isVisible()&&(bottom>=pEvent->rect().top()))
        {
            QString number=QString::number(blockNumber+1);
            painter.setPen(Qt::black);
            painter.drawText(0,top,g_pLineNumberArea->width()-5,fontMetrics().height(),Qt::AlignRight,number);
        }

        block=block.next();
        top=bottom;
        bottom=top+qRound(blockBoundingRect(block).height());
        ++blockNumber;
    }
}

int DIE_SignatureEdit::lineNumberAreaWidth()
{
//    int digits=1;
//    int max=qMax(1,blockCount());

//    while(max>=10)
//    {
//        max/=10;
//        ++digits;
//    }

    int digits=3;

    int space=10+fontMetrics().horizontalAdvance(QLatin1Char('9'))*digits;

    return space;
}

void DIE_SignatureEdit::setPlainText(const QString &sText)
{
    QPlainTextEdit::setPlainText(sText);

    highlightCurrentLine();

    g_pLineNumberArea->update();
}

void DIE_SignatureEdit::keyPressEvent(QKeyEvent *pEvent)
{
    if(pEvent->key()==Qt::Key_Tab)
    {
        qint32 nPosition=textCursor().positionInBlock();

        qint32 nAppend=nPosition%4;

        QString sAppend;

        sAppend=sAppend.fill(QChar(' '),4-nAppend);

        QKeyEvent *pEventNew=new QKeyEvent(QEvent::KeyPress,0,Qt::NoModifier,sAppend);
        QPlainTextEdit::keyPressEvent(pEventNew);

        delete pEventNew;
    }
    else
    {
        QPlainTextEdit::keyPressEvent(pEvent);
    }
}

void DIE_SignatureEdit::resizeEvent(QResizeEvent *pEvent)
{
    QPlainTextEdit::resizeEvent(pEvent);

    QRect cr=contentsRect();
    g_pLineNumberArea->setGeometry(QRect(cr.left(),cr.top(),lineNumberAreaWidth(),cr.height()));
}

void DIE_SignatureEdit::updateLineNumberAreaWidth(int newBlockCount)
{
    setViewportMargins(lineNumberAreaWidth()+5,0,0,0);
}

void DIE_SignatureEdit::highlightCurrentLine()
{
    QList<QTextEdit::ExtraSelection> extraSelections;

    if(!isReadOnly())
    {
        QTextEdit::ExtraSelection selection;

        QColor lineColor=QColor(Qt::yellow).lighter(160);

        selection.format.setBackground(lineColor);
        selection.format.setProperty(QTextFormat::FullWidthSelection,true);
        selection.cursor=textCursor();
        selection.cursor.clearSelection();
        extraSelections.append(selection);
    }

    setExtraSelections(extraSelections);
}

void DIE_SignatureEdit::updateLineNumberArea(const QRect &rect,int dy)
{
    if(dy)
        g_pLineNumberArea->scroll(0,dy);
    else
        g_pLineNumberArea->update(0,rect.y(),g_pLineNumberArea->width(),rect.height());

    if(rect.contains(viewport()->rect()))
    {
        updateLineNumberAreaWidth(0);
    }
}

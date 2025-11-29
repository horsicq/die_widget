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
#include "die_signatureedit.h"

DIE_SignatureEdit::DIE_SignatureEdit(QWidget *pParent) : QPlainTextEdit(pParent)
{
    m_pHighlighter = new DIE_Highlighter(this->document());

    m_pHighlighter->setDocument(this->document());

    m_pLineNumberArea = new DIE_LineNumberArea(this);

    connect(this, &DIE_SignatureEdit::blockCountChanged, this, &DIE_SignatureEdit::updateLineNumberAreaWidth);
    connect(this, &DIE_SignatureEdit::updateRequest, this, &DIE_SignatureEdit::updateLineNumberArea);
    connect(this, &DIE_SignatureEdit::cursorPositionChanged, this, &DIE_SignatureEdit::highlightCurrentLine);

    updateLineNumberAreaWidth(0);
    highlightCurrentLine();
}

void DIE_SignatureEdit::lineNumberAreaPaintEvent(QPaintEvent *pEvent)
{
    QPainter painter(m_pLineNumberArea);
    painter.fillRect(pEvent->rect(), Qt::lightGray);

    QTextBlock block = firstVisibleBlock();
    qint32 nBlockNumber = block.blockNumber();
    qint32 nTop = qRound(blockBoundingGeometry(block).translated(contentOffset()).top());
    qint32 nBottom = nTop + qRound(blockBoundingRect(block).height());

    while (block.isValid() && (nTop <= pEvent->rect().bottom())) {
        if (block.isVisible() && (nBottom >= pEvent->rect().top())) {
            QString number = QString::number(nBlockNumber + 1);
            painter.setPen(Qt::black);
            painter.drawText(0, nTop, m_pLineNumberArea->width() - 5, fontMetrics().height(), Qt::AlignRight, number);
        }

        block = block.next();
        nTop = nBottom;
        nBottom = nTop + qRound(blockBoundingRect(block).height());
        ++nBlockNumber;
    }
}

qint32 DIE_SignatureEdit::lineNumberAreaWidth()
{
    qint32 nDigits = 3;
#if (QT_VERSION < QT_VERSION_CHECK(5, 11, 0))
    qint32 nSpace = 10 + fontMetrics().width(QLatin1Char('9')) * nDigits;
#else
    qint32 nSpace = 10 + fontMetrics().horizontalAdvance(QLatin1Char('9')) * nDigits;
#endif

    return nSpace;
}

void DIE_SignatureEdit::setPlainText(const QString &sText)
{
    QPlainTextEdit::setPlainText(sText);

    highlightCurrentLine();

    m_pLineNumberArea->update();
}

void DIE_SignatureEdit::keyPressEvent(QKeyEvent *pEvent)
{
    if (pEvent->key() == Qt::Key_Tab) {
        qint32 nPosition = textCursor().positionInBlock();

        qint32 nAppend = nPosition % 4;  // TODO options

        QString sAppend;

        sAppend = sAppend.fill(QChar(' '), 4 - nAppend);

        QKeyEvent *pEventNew = new QKeyEvent(QEvent::KeyPress, 0, Qt::NoModifier, sAppend);
        QPlainTextEdit::keyPressEvent(pEventNew);

        delete pEventNew;
    } else {
        QPlainTextEdit::keyPressEvent(pEvent);
    }
}

void DIE_SignatureEdit::resizeEvent(QResizeEvent *pEvent)
{
    QPlainTextEdit::resizeEvent(pEvent);

    QRect cr = contentsRect();
    m_pLineNumberArea->setGeometry(QRect(cr.left(), cr.top(), lineNumberAreaWidth(), cr.height()));
}

void DIE_SignatureEdit::updateLineNumberAreaWidth(qint32 newBlockCount)
{
    Q_UNUSED(newBlockCount)

    setViewportMargins(lineNumberAreaWidth() + 5, 0, 0, 0);
}

void DIE_SignatureEdit::highlightCurrentLine()
{
    QList<QTextEdit::ExtraSelection> extraSelections;

    if (!isReadOnly()) {
        QTextEdit::ExtraSelection selection;

        QColor lineColor = QColor(Qt::yellow).lighter(160);

        selection.format.setBackground(lineColor);
        selection.format.setProperty(QTextFormat::FullWidthSelection, true);
        selection.cursor = textCursor();
        selection.cursor.clearSelection();
        extraSelections.append(selection);
    }

    setExtraSelections(extraSelections);
}

void DIE_SignatureEdit::updateLineNumberArea(const QRect &rect, qint32 nDy)
{
    if (nDy) {
        m_pLineNumberArea->scroll(0, nDy);
    } else {
        m_pLineNumberArea->update(0, rect.y(), m_pLineNumberArea->width(), rect.height());
    }

    if (rect.contains(viewport()->rect())) {
        updateLineNumberAreaWidth(0);
    }
}

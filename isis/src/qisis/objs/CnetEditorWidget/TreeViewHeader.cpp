/** This is free and unencumbered software released into the public domain.

The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */

#include "TreeViewHeader.h"

#include <iostream>

#include <QAbstractItemModel>
#include <QFontMetrics>
#include <QLabel>
#include <QLinearGradient>
#include <QLocale>
#include <QPainter>
#include <QPen>
#include <QRect>
#include <QString>
#include <QVBoxLayout>

#include "TreeView.h"
#include "TreeViewContent.h"
#include "AbstractTreeModel.h"


namespace Isis {
  TreeViewHeader::TreeViewHeader(TreeViewContent *someContent,
      QWidget *parent) : QWidget(parent), m_content(someContent) {
    nullify();

    m_headerText = new QString("Header text");
    m_filterProgress = 0;
    m_filterProgressMin = 0;
    m_filterProgressMax = 0;
    m_rebuildProgress = 0;
    m_rebuildProgressMin = 0;
    m_rebuildProgressMax = 0;
    m_active = false;
    m_visibleCount = -1;
    m_totalCount = -1;
  }


  TreeViewHeader::TreeViewHeader(const TreeViewHeader &other) {
    nullify();

    m_headerText = new QString(*other.m_headerText);
  }


  TreeViewHeader::~TreeViewHeader() {
    if (m_headerText) {
      delete m_headerText;
      m_headerText = NULL;
    }
  }


  QSize TreeViewHeader::minimumSizeHint() const {
    return QSize(QFontMetrics(font()).width(*m_headerText) + 15,
        QFontMetrics(font()).height() + 6);
  }

  QString TreeViewHeader::getText() {
    return *m_headerText;
  }


  void TreeViewHeader::setText(QString text) {
    *m_headerText = text;
    updateGeometry();
    update();
  }


  TreeViewHeader &TreeViewHeader::operator=(
    const TreeViewHeader &other) {
    if (this != &other) {
      if (m_headerText) {
        delete m_headerText;
        m_headerText = NULL;
      }
      m_headerText = new QString;
      *m_headerText = *other.m_headerText;
    }

    return *this;
  }


  void TreeViewHeader::setActive(bool newActiveState) {
    m_active = newActiveState;
  }


  void TreeViewHeader::handleFilterCountsChanged(
    int visibleTopLevelItemCount, int topLevelItemCount) {
    m_visibleCount = visibleTopLevelItemCount;
    m_totalCount = topLevelItemCount;
    updateGeometry();
    update();
  }


  void TreeViewHeader::mouseReleaseEvent(QMouseEvent *event) {
    setActive(true);
    emit(activated());
    update();
  }


  void TreeViewHeader::paintEvent(QPaintEvent *event) {
    QPainter painter(this);
    painter.setRenderHints(QPainter::Antialiasing |
        QPainter::TextAntialiasing);
    paintHeader(&painter, height());
    painter.drawRect(0, 0, width(), height());
    painter.end();
  }


  void TreeViewHeader::nullify() {
    m_headerText = NULL;
  }


  void TreeViewHeader::paintHeader(QPainter *painter, int rowHeight) {
    QRect rect(0, 0, width(), rowHeight);

    int x = rect.center().x();
    QLinearGradient gradient(x, rect.top(), x, rect.bottom());

    //FIXME: selected needs to be member variable
    bool selected = false;
    QColor color = selected ? palette().highlight().color() :
        palette().button().color();

    // create gradient and fill header area with it
    int adjustment = 100;
    m_active ? adjustment += 7 : adjustment -= 3;
    gradient.setColorAt(0, color.darker(adjustment));
    gradient.setColorAt(0.5, color.lighter(adjustment));
    gradient.setColorAt(1, color.darker(adjustment));
    painter->fillRect(rect, gradient);

    // Save off composition mode and brush, which will need to be restored
    // after the progress is painted.
    QBrush brush = painter->brush();
    QPainter::CompositionMode compMode = painter->compositionMode();
    painter->setCompositionMode(QPainter::CompositionMode_SourceOver);

    // draw filter progress (if not at 100%)
    painter->setBrush(QBrush(QColor(0, 70, 100, 30)));
    paintProgress(painter, rect, m_filterProgressMin, m_filterProgressMax,
        m_filterProgress);

    // draw rebuild progress (if not at 100%)
    painter->setBrush(QBrush(QColor(100, 70, 0, 30)));
    paintProgress(painter, rect, m_rebuildProgressMin, m_rebuildProgressMax,
        m_rebuildProgress);

    // draw the header's text
    painter->setBrush(brush);
    painter->setCompositionMode(compMode);
    painter->setPen(selected ? palette().highlightedText().color() :
        palette().buttonText().color());

    QString title(*m_headerText);

    if (m_visibleCount >= 0) {
      title += " (";

      title += QLocale().toString(m_visibleCount);

      title += " / ";
      title += QLocale().toString(m_totalCount);
      title += ")";
    }

    painter->drawText(rect, title, QTextOption(Qt::AlignCenter));
  }


  void TreeViewHeader::paintProgress(QPainter *painter, const QRect &rect,
      int min, int max, int value) {
    // draw filter progress if not at 100%
    double progressPercent = 1.0;
    int progressRange = max - min;
    if (progressRange > 0)
      progressPercent = ((double)(value - min)) / progressRange;

    if (progressPercent < 1.0) {
      QRect progressRect(rect);
      progressRect.setWidth((int)(progressRect.width() * progressPercent));
      painter->fillRect(progressRect, painter->brush());
    }
  }


  void TreeViewHeader::updateFilterProgress(int newProgress) {
    m_filterProgress = newProgress;
    update();
  }


  void TreeViewHeader::updateFilterProgressRange(int min, int max) {
    m_filterProgressMin = min;
    m_filterProgressMax = max;
    update();
  }


  void TreeViewHeader::updateRebuildProgress(int newProgress) {
    m_rebuildProgress = newProgress;
    update();
  }


  void TreeViewHeader::updateRebuildProgressRange(int min, int max) {
    m_rebuildProgressMin = min;
    m_rebuildProgressMax = max;
    update();
  }
}

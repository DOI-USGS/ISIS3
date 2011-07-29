#include "IsisDebug.h"

#include "CnetTreeViewHeader.h"

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

#include "CnetTreeView.h"
#include "CnetTreeViewContent.h"
#include "TreeModel.h"

using std::cerr;


namespace Isis
{

  CnetTreeViewHeader::CnetTreeViewHeader(CnetTreeViewContent * someContent,
      QWidget * parent) : QWidget(parent), content(someContent)
  {
    nullify();

    ASSERT(content);

    headerText = new QString("Header text");
    filterProgress = 0;
    filterProgressMin = 0;
    filterProgressMax = 0;
    rebuildProgress = 0;
    rebuildProgressMin = 0;
    rebuildProgressMax = 0;
    active = false;
    visibleCount = -1;
    totalCount = -1;
  }


  CnetTreeViewHeader::CnetTreeViewHeader(const CnetTreeViewHeader & other)
  {
    nullify();

    headerText = new QString(*other.headerText);
  }


  CnetTreeViewHeader::~CnetTreeViewHeader()
  {
    if (headerText)
    {
      delete headerText;
      headerText = NULL;
    }
  }


  QSize CnetTreeViewHeader::minimumSizeHint() const
  {
    return QSize(QFontMetrics(font()).width(*headerText) + 15,
        QFontMetrics(font()).height() + 6);
  }

  QString CnetTreeViewHeader::getText()
  {
    ASSERT(headerText);
    return *headerText;
  }


  void CnetTreeViewHeader::setText(QString text)
  {
    ASSERT(headerText);
    *headerText = text;
    updateGeometry();
    update();
  }


  CnetTreeViewHeader & CnetTreeViewHeader::operator=(
    const CnetTreeViewHeader & other)
  {
    if (this != &other)
    {
      if (headerText)
      {
        delete headerText;
        headerText = NULL;
      }
      headerText = new QString;
      *headerText = *other.headerText;
    }

    return *this;
  }


  void CnetTreeViewHeader::setActive(bool newActiveState)
  {
    active = newActiveState;
  }


  void CnetTreeViewHeader::handleFilterCountsChanged(
    int visibleTopLevelItemCount, int topLevelItemCount)
  {
    visibleCount = visibleTopLevelItemCount;
    totalCount = topLevelItemCount;
    updateGeometry();
    update();
  }


  void CnetTreeViewHeader::mouseReleaseEvent(QMouseEvent * event)
  {
    setActive(true);
    emit(activated());
    update();
  }


  void CnetTreeViewHeader::paintEvent(QPaintEvent * event)
  {
    QPainter painter(this);
    painter.setRenderHints(QPainter::Antialiasing |
        QPainter::TextAntialiasing);
    paintHeader(&painter, height());
    painter.drawRect(0, 0, width(), height());
    painter.end();
  }


  void CnetTreeViewHeader::nullify()
  {
    headerText = NULL;
  }


  void CnetTreeViewHeader::paintHeader(QPainter * painter, int rowHeight)
  {
    QRect rect(0, 0, width(), rowHeight);

    int x = rect.center().x();
    QLinearGradient gradient(x, rect.top(), x, rect.bottom());

    //FIXME: selected needs to be member variable
    bool selected = false;
    QColor color = selected ? palette().highlight().color() :
        palette().button().color();

    // create gradient and fill header area with it
    int adjustment = 100;
    active ? adjustment += 7 : adjustment -= 3;
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
    paintProgress(painter, rect, filterProgressMin, filterProgressMax,
        filterProgress);

    // draw rebuild progress (if not at 100%)
    painter->setBrush(QBrush(QColor(100, 70, 0, 30)));
    paintProgress(painter, rect, rebuildProgressMin, rebuildProgressMax,
        rebuildProgress);

    // draw the header's text
    painter->setBrush(brush);
    painter->setCompositionMode(compMode);
    painter->setPen(selected ? palette().highlightedText().color() :
        palette().buttonText().color());

    QString title(*headerText);

    if (visibleCount >= 0)
    {
      title += " (";

      title += QLocale().toString(visibleCount);

      title += " / ";
      title += QLocale().toString(totalCount);
      title += ")";
    }

    painter->drawText(rect, title, QTextOption(Qt::AlignCenter));
  }


  void CnetTreeViewHeader::paintProgress(QPainter * painter, const QRect & rect,
      int min, int max, int value)
  {
    // draw filter progress if not at 100%
    double progressPercent = 1.0;
    int progressRange = max - min;
    if (progressRange > 0)
      progressPercent = ((double)(value - min)) / progressRange;

    if (progressPercent < 1.0)
    {
      QRect progressRect(rect);
      progressRect.setWidth((int)(progressRect.width() * progressPercent));
      painter->fillRect(progressRect, painter->brush());
    }
  }


  void CnetTreeViewHeader::updateFilterProgress(int newProgress)
  {
    filterProgress = newProgress;
    update();
  }


  void CnetTreeViewHeader::updateFilterProgressRange(int min, int max)
  {
    filterProgressMin = min;
    filterProgressMax = max;
    update();
  }


  void CnetTreeViewHeader::updateRebuildProgress(int newProgress)
  {
    rebuildProgress = newProgress;
    update();
  }


  void CnetTreeViewHeader::updateRebuildProgressRange(int min, int max)
  {
    rebuildProgressMin = min;
    rebuildProgressMax = max;
    update();
  }
}


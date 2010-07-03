#include "HistogramItem.h"

#include <QPainter>
#include <QString>

#include <qwt_plot.h>
#include <qwt_interval_data.h>
#include <qwt_painter.h>
#include <qwt_scale_map.h>

#include <iostream>

namespace Qisis {

  class HistogramItem::PrivateData {
    public:
      int attributes;
      QwtIntervalData data;
      QColor color;
      double reference;
  };

  /**
   * Constructor 1
   *
   *
   * @param title
   */
  HistogramItem::HistogramItem(const QwtText &title): QwtPlotItem() {
    init();
  }


  /**
   * Constructor 2
   *
   *
   * @param title
   */
  HistogramItem::HistogramItem(const QString &title): QwtPlotItem(QwtText(title)) {
    init();
  }


  /**
   * Destructor
   *
   */
  HistogramItem::~HistogramItem() {
    delete d_data;
  }


  /**
   * Initialization method
   *
   */
  void HistogramItem::init() {
    d_data = new PrivateData();
    d_data->reference = 0.0;
    d_data->attributes = HistogramItem::Auto;

    setItemAttribute(QwtPlotItem::AutoScale, true);
    setItemAttribute(QwtPlotItem::Legend, true);

    setZ(20.0);
  }


  /**
   *
   *
   *
   * @param reference
   */
  void HistogramItem::setBaseline(double reference) {
    if(d_data->reference != reference) {
      d_data->reference = reference;
      itemChanged();
    }
  }


  /**
   * Returns the baseline.
   *
   *
   * @return double
   */
  double HistogramItem::baseline() const {
    return d_data->reference;
  }


  /**
   * Overridden method to set the data in the histogram.
   *
   *
   * @param data
   */
  void HistogramItem::setData(const QwtIntervalData &data) {
    d_data->data = data;
    itemChanged();
  }


  /**
   * Returns this item's data
   *
   *
   * @return const QwtIntervalData&
   */
  const QwtIntervalData &HistogramItem::data() const {
    return d_data->data;
  }


  /**
   * Set the color of the hist. item.
   *
   *
   * @param color
   */
  void HistogramItem::setColor(const QColor &color) {
    if(d_data->color != color) {
      d_data->color = color;
      itemChanged();
    }
  }


  /**
   * Return the color of the item.
   *
   *
   * @return QColor
   */
  QColor HistogramItem::color() const {
    return d_data->color;
  }


  /**
   * Returns the bounding rectangle of the item.
   *
   *
   * @return QwtDoubleRect
   */
  QwtDoubleRect HistogramItem::boundingRect() const {
    QwtDoubleRect rect = d_data->data.boundingRect();
    if(!rect.isValid())
      return rect;

    if(d_data->attributes & Xfy) {
      rect = QwtDoubleRect(rect.y(), rect.x(),
                           rect.height(), rect.width());

      if(rect.left() > d_data->reference)
        rect.setLeft(d_data->reference);
      else if(rect.right() < d_data->reference)
        rect.setRight(d_data->reference);
    }
    else {
      if(rect.bottom() < d_data->reference)
        rect.setBottom(d_data->reference);
      else if(rect.top() > d_data->reference)
        rect.setTop(d_data->reference);
    }

    return rect;
  }


  /**
   *
   *
   *
   * @return int
   */
  int HistogramItem::rtti() const {
    return QwtPlotItem::Rtti_PlotHistogram;
  }


  /**
   *
   *
   *
   * @param attribute
   * @param on
   */
  void HistogramItem::setHistogramAttribute(HistogramAttribute attribute, bool on) {
    if(bool(d_data->attributes & attribute) == on)
      return;

    if(on)
      d_data->attributes |= attribute;
    else
      d_data->attributes &= ~attribute;

    itemChanged();
  }


  /**
   *
   *
   *
   * @param attribute
   *
   * @return bool
   */
  bool HistogramItem::testHistogramAttribute(HistogramAttribute attribute) const {
    return d_data->attributes & attribute;
  }


  /**
   *
   *
   *
   * @param painter
   * @param xMap
   * @param yMap
   */
  void HistogramItem::draw(QPainter *painter, const QwtScaleMap &xMap,
                           const QwtScaleMap &yMap, const QRect &) const {
    const QwtIntervalData &iData = d_data->data;

    painter->setPen(QPen(d_data->color));

    const int x0 = xMap.transform(baseline());
    const int y0 = yMap.transform(baseline());

    for(int i = 0; i < (int)iData.size(); i++) {
      if(d_data->attributes & HistogramItem::Xfy) {
        const int x2 = xMap.transform(iData.value(i));
        if(x2 == x0)
          continue;

        int y1 = yMap.transform(iData.interval(i).minValue());
        int y2 = yMap.transform(iData.interval(i).maxValue());
        if(y1 > y2)
          qSwap(y1, y2);

        if(i < (int)iData.size() - 2) {
          const int yy1 = yMap.transform(iData.interval(i + 1).minValue());
          const int yy2 = yMap.transform(iData.interval(i + 1).maxValue());

          if(y2 == qwtMin(yy1, yy2)) {
            const int xx2 = xMap.transform(
                              iData.interval(i + 1).minValue());
            if(xx2 != x0 && ((xx2 < x0 && x2 < x0) ||
                             (xx2 > x0 && x2 > x0))) {
              // One pixel distance between neighboured bars
              y2++;
            }
          }
        }

        drawBar(painter, Qt::Horizontal,
                QRect(x0, y1, x2 - x0, y2 - y1));
      }
      else {
        const int y2 = yMap.transform(iData.value(i));
        if(y2 == y0)
          continue;

        int x1 = xMap.transform(iData.interval(i).minValue());
        int x2 = xMap.transform(iData.interval(i).maxValue());
        if(x1 > x2)
          qSwap(x1, x2);

        if(i < (int)iData.size() - 2) {
          const int xx1 = xMap.transform(iData.interval(i + 1).minValue());
          const int xx2 = xMap.transform(iData.interval(i + 1).maxValue());

          if(x2 == qwtMin(xx1, xx2)) {
            const int yy2 = yMap.transform(iData.value(i + 1));
            if(yy2 != y0 && ((yy2 < y0 && y2 < y0) ||
                             (yy2 > y0 && y2 > y0))) {
              // One pixel distance between neighboured bars
              x2--;
            }
          }
        }
        drawBar(painter, Qt::Vertical,
                QRect(x1, y0, x2 - x1, y2 - y0));
      }
    }
  }


  /**
   * This method draws the bars of the bar graph.
   *
   *
   * @param painter
   * @param rect
   */
  void HistogramItem::drawBar(QPainter *painter,
                              Qt::Orientation, const QRect &rect) const {
    painter->save();

    const QColor color(painter->pen().color());
#if QT_VERSION >= 0x040000
    const QRect r = rect.normalized();
#else
    const QRect r = rect.normalize();
#endif

    const int factor = 125;
    const QColor light(color.light(factor));
    const QColor dark(color.dark(factor));

    painter->setBrush(color);
    painter->setPen(Qt::NoPen);
    QwtPainter::drawRect(painter, r.x() + 1, r.y() + 1,
                         r.width() - 2, r.height() - 2);
    painter->setBrush(Qt::NoBrush);

    painter->setPen(QPen(light, 2));
#if QT_VERSION >= 0x040000
    QwtPainter::drawLine(painter,
                         r.left() + 1, r.top() + 2, r.right() + 1, r.top() + 2);
#else
    QwtPainter::drawLine(painter,
                         r.left(), r.top() + 2, r.right() + 1, r.top() + 2);
#endif

    painter->setPen(QPen(dark, 2));
#if QT_VERSION >= 0x040000
    QwtPainter::drawLine(painter,
                         r.left() + 1, r.bottom(), r.right() + 1, r.bottom());
#else
    QwtPainter::drawLine(painter,
                         r.left(), r.bottom(), r.right() + 1, r.bottom());
#endif

    painter->setPen(QPen(light, 1));

#if QT_VERSION >= 0x040000
    QwtPainter::drawLine(painter,
                         r.left(), r.top() + 1, r.left(), r.bottom());
    QwtPainter::drawLine(painter,
                         r.left() + 1, r.top() + 2, r.left() + 1, r.bottom() - 1);
#else
    QwtPainter::drawLine(painter,
                         r.left(), r.top() + 1, r.left(), r.bottom() + 1);
    QwtPainter::drawLine(painter,
                         r.left() + 1, r.top() + 2, r.left() + 1, r.bottom());
#endif

    painter->setPen(QPen(dark, 1));

#if QT_VERSION >= 0x040000
    QwtPainter::drawLine(painter,
                         r.right() + 1, r.top() + 1, r.right() + 1, r.bottom());
    QwtPainter::drawLine(painter,
                         r.right(), r.top() + 2, r.right(), r.bottom() - 1);
#else
    QwtPainter::drawLine(painter,
                         r.right() + 1, r.top() + 1, r.right() + 1, r.bottom() + 1);
    QwtPainter::drawLine(painter,
                         r.right(), r.top() + 2, r.right(), r.bottom());
#endif

    painter->restore();
  }


  /**
   * This method returns a list of points which are the vertices
   * of the selected area (by the rubberband) on the cvp.
   *
   * @return QList<QPoint>
   */
  QList <QPointF > HistogramItem::getVertices()  const {
    return p_pointList;
  }


  /**
   * This method sets the vertices of the selected area on the
   * cvp.
   * @param points
   */
  void HistogramItem::setVertices(const QList <QPoint> &points) {
    double sample, line;
    p_pointList.clear();
    if(this->getViewPort() == NULL) {
      std::cout << "Please be sure to set the view port first!" << std::endl;
    }
    for(int i = 0; i < points.size(); i++) {
      this->getViewPort()->viewportToCube(points[i].x(), points[i].y(), sample, line);
      p_pointList.push_back(QPointF((sample), (line)));
    }
  }


  /**
   * This method returns the cube view port associated with the
   * curve.
   *
   * @return CubeViewport*
   */
  CubeViewport *HistogramItem::getViewPort() const {
    return p_cvp;
  }


  /**
   * This method sets the view port.
   * @param cvp
   */
  void HistogramItem::setViewPort(CubeViewport *cvp) {
    p_cvp = cvp;
  }


  /**
   * This method allows the users to copy all of the given curves
   * properties into the current curve.  This is an overriden
   * function from the PlotCurve class.
   *
   * @param pc
   */
  void HistogramItem::copyCurveProperties(HistogramItem *hi) {
    this->setAxis(hi->xAxis(), hi->yAxis());
    this->setVisible(hi->isVisible());
    //this->setPen(hi->pen());
    this->setTitle(hi->title().text());
    this->setData(hi->data());
    this->setColor(hi->color());

    p_cvp = hi->getViewPort();
    p_pointList = hi->getVertices();
  }

}

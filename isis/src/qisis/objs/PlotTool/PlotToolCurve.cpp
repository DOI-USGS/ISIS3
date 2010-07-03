#include "PlotToolCurve.h"

#include <iostream>


namespace Qisis {
  /**
   * This constructs a PlotToolCurve... a subclass of PlotCurve.
   * This class was created specifically for use withthe plot
   * tool. With this class the programmer can set the cube view
   * port that the curve is associated with along with the
   * vertices on the cvp of which the curve gets it data.
   */
  PlotToolCurve::PlotToolCurve()  : PlotCurve() {
    p_cvp = NULL;
  }


  /**
   * This method returns a list of points which are the vertices
   * of the selected area (by the rubberband) on the cvp.
   *
   * @return QList<QPoint>
   */
  QList <QPointF > PlotToolCurve::getVertices()  const {
    return p_pointList;
  }


  /**
   * This method sets the vertices of the selected area on the
   * cvp.
   * @param points
   */
  void PlotToolCurve::setVertices(const QList <QPoint> &points) {
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
  CubeViewport *PlotToolCurve::getViewPort() const {
    return p_cvp;
  }


  /**
   * This method sets the view port.
   * @param cvp
   */
  void PlotToolCurve::setViewPort(CubeViewport *cvp) {
    p_cvp = cvp;
  }


  /**
   * This method allows the users to copy all of the given curves
   * properties into the current curve.  This is an overriden
   * function from the PlotCurve class.
   * @param pc
   */
  void PlotToolCurve::copyCurveProperties(PlotToolCurve *pc) {
    PlotCurve::copyCurveProperties(pc);

    p_cvp = pc->getViewPort();
    p_pointList = pc->getVertices();
  }

}


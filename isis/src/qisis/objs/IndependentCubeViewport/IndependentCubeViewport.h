#ifndef IndependentCubeViewport_h
#define IndependentCubeViewport_h

/** This is free and unencumbered software released into the public domain.

The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */

// parent class
#include "CubeViewport.h"


class QEvent;
class QObject;
class QPaintEvent;
class QPoint;
class QRect;
class QWidget;


template < class T > class QList;

namespace Isis
{
  class Brick;
  class Cube;
  class CubeDataThread;
  class ViewportBuffer;

  /**
   * @brief General purpose Cube display widget
   *
   * @ingroup Visualization Tools
   *
   * @author ????-??-?? Eric Hyer
   *
   * @internal
   *
   * @see CubeViewport
   *   @history 2013-02-20 Debbie A. Cook - Changed to use new projection types.  References #775.
   */
  class IndependentCubeViewport : public CubeViewport
  {
      Q_OBJECT

    public:
      IndependentCubeViewport(Cube * cube, CubeDataThread * cdt = 0,
                              QWidget * parent = 0);
      ~IndependentCubeViewport();

      bool eventFilter(QObject * o, QEvent * e);
      void paintEvent(QPaintEvent * e);
      void restretch(ViewportBuffer *);
      virtual void showEvent(QShowEvent * e);


    public slots:
      void resetKnownGlobal();


    protected slots:
      virtual void cubeDataChanged(int cubeId, const Brick *);


    signals:
      void synchronize(IndependentCubeViewport *);
      void trackingChanged(double sample, double line, double lat, double lon,
          double dn, IndependentCubeViewport *);
      void trackingChanged(double samp, double line, double dn,
          IndependentCubeViewport *);
      void cantTrack(QString, IndependentCubeViewport *);



    private slots:
      void handleMouseMove(QPoint);
      void handleMousePress(QPoint, Qt::MouseButton b);
      void handleMouseRelease(QPoint);
      void handleSynchronization(IndependentCubeViewport *);


    private: // methods
      QRect bandingRect();
      void stretch();
      void track(const QPoint &);
      void zoom();
      bool trackBuffer(ViewportBuffer *, const QPoint &, double &);


    private:
      bool banding;
      bool panning;
      bool stretching;
      bool zooming;
      bool leftClick;
      QPoint * bandingPoint1;
      QPoint * bandingPoint2;
      QPoint * panningPrevPoint;
  };
}

#endif

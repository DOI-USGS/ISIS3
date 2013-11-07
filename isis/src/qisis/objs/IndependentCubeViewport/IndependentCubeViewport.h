#ifndef IndependentCubeViewport_h
#define IndependentCubeViewport_h

/**
 * @file
 * $Date: 2010/06/28 08:42:54 $
 * $Revision: 1.1 $
 *
 *   Unless noted otherwise, the portions of Isis written by the USGS are public
 *   domain. See individual third-party library and package descriptions for
 *   intellectual property information,user agreements, and related information.
 *
 *   Although Isis has been used by the USGS, no warranty, expressed or implied,
 *   is made by the USGS as to the accuracy and functioning of such software
 *   and related material nor shall the fact of distribution constitute any such
 *   warranty, and no responsibility is assumed by the USGS in connection
 *   therewith.
 *
 *   For additional information, launch
 *   $ISISROOT/doc//documents/Disclaimers/Disclaimers.html in a browser or see
 *   the Privacy &amp; Disclaimers page on the Isis website,
 *   http://isis.astrogeology.usgs.gov, and the USGS privacy and disclaimers on
 *   http://www.usgs.gov/privacy.html.
 */

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

#ifndef PlotToolWindow_h
#define PlotToolWindow_h

/**
 * @file
 * $Date: 2010/06/28 09:07:50 $
 * $Revision: 1.8 $
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

#include "PlotWindow.h"


class QString;
template< class T > class QVector;

class QwtPlotMarker;

namespace Isis {
  class PvlKeyword;
}

namespace Isis {
  /**
   * @brief Widget to display Isis cubes for qt apps
   *
   * @ingroup Visualization Tools
   *
   * @author ???
   *
   * @internal
   *  @history 2010-06-26 - Eric Hyer - Now uses MdiCubeViewport instead of
   *           CubeViewport.  Fixed multiple include problems.
   */
  class PlotToolWindow : public PlotWindow {
      Q_OBJECT

    public:
      PlotToolWindow(QString title, QWidget *parent);
      PlotToolWindow(const PlotToolWindow &other);
      virtual ~PlotToolWindow();
      bool bandMarkersVisible();
      void setViewport(CubeViewport *cvp);
      void setPlotType(QString plotType);
      bool p_markersVisible;
      const PlotToolWindow &operator=(PlotToolWindow other);


    public slots:
      void drawBandMarkers();
      void setBandMarkersVisible(bool visible);
      void showHideLines();
      void fillTable();
      void setStdDev(QVector< double > stdDevArray);


    private:
      void setAutoScaleOption(bool autoScale = false);
      QwtPlotMarker *p_grayBandLine;
      QwtPlotMarker *p_redBandLine;
      QwtPlotMarker *p_greenBandLine;
      QwtPlotMarker *p_blueBandLine;
      CubeViewport   *p_cvp;
      QString *p_plotType;
      PvlKeyword *p_wavelengths;
      bool p_autoScale;
      QVector< double > * p_stdDevArray;
  };
};

#endif


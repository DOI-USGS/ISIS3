#ifndef SpectralPlotWindow_h
#define SpectralPlotWindow_h

/**
 * @file
 * $Revision: 1.17 $
 * $Date: 2009/04/07 16:19:40 $
 *
 *   Unless noted otherwise, the portions of Isis written by the USGS are
 *   public domain. See individual third-party library and package descriptions
 *   for intellectual property information, user agreements, and related
 *   information.
 *
 *   Although Isis has been used by the USGS, no warranty, expressed or
 *   implied, is made by the USGS as to the accuracy and functioning of such
 *   software and related material nor shall the fact of distribution
 *   constitute any such warranty, and no responsibility is assumed by the
 *   USGS in connection therewith.
 *
 *   For additional information, launch
 *   $ISISROOT/doc//documents/Disclaimers/Disclaimers.html
 *   in a browser or see the Privacy &amp; Disclaimers page on the Isis website,
 *   http://isis.astrogeology.usgs.gov, and the USGS privacy and disclaimers on
 *   http://www.usgs.gov/privacy.html.
 */

#include "PlotWindow.h"

namespace Isis {
  class PvlGroup;

  /**
   * @author ????-??-?? Tracie Sucharski and Steven Lambright
   *
   * @ingroup Visualization Tools
   *
   * @internal
   */
  class SpectralPlotWindow : public PlotWindow {
      Q_OBJECT

    public:
      SpectralPlotWindow(PlotCurve::Units xUnits, QWidget *parent);
      ~SpectralPlotWindow();

      bool bandMarkersVisible() const;

      void setViewport(MdiCubeViewport *cvp);
      void update(MdiCubeViewport *activeViewport);

    public slots:
      void setBandMarkersVisible(bool visible);

    private:
      void nullify();
      QwtPlotMarker *createMarker(QColor color);
      void drawBandMarkers();

      //! The viewport to be used as a reference for band markers
      MdiCubeViewport *m_cvp;
      //! True if the visibile state of the active markers should be true
      bool m_markersVisible;
      //! The band marker for the gray band
      QwtPlotMarker *m_grayBandLine;
      //! The band marker for the red band
      QwtPlotMarker *m_redBandLine;
      //! The band marker for the green band
      QwtPlotMarker *m_greenBandLine;
      //! The band marker for the blue band
      QwtPlotMarker *m_blueBandLine;
      //! This action toggles band marker visibility
      QAction *m_showHideBandMarkers;
   };
};

#endif


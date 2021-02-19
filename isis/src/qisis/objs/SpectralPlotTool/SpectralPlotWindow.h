#ifndef SpectralPlotWindow_h
#define SpectralPlotWindow_h

/** This is free and unencumbered software released into the public domain.

The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */

#include "PlotWindow.h"

namespace Isis {
  class PvlGroup;

  /**
   * @author ????-??-?? Tracie Sucharski and Steven Lambright
   *
   * @ingroup Visualization Tools
   *
   * @internal
   *  @history 2017-10-18 Summer Stapleton - Member variable m_cvp can now be set to NULL in
   *                        ::setViewport() to represent that there are no open cube viewports.
   *                        This means that ::drawBandMarkers will no longer attempt to access a
   *                        non-existant cube. Fixes: #2142.
   *
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

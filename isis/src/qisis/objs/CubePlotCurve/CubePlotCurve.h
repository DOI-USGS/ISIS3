#ifndef CubePlotCurve_h
#define CubePlotCurve_h

/**
 * @file
 * $Revision: 1.5 $
 * $Date: 2008/06/19 15:54:03 $
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

#include <QObject>
#include <QPointer>

// This is needed for the QVariant macro
#include <QMetaType>

#include "PlotCurve.h"

class QMimeData;
class QMouseEvent;

namespace Isis {
  class CubeViewport;

  /**
   * This is a plot curve with information relating it to a particular cube or
   *   region of a cube. This class was created specifically for use with the
   *   qview plot tools (AbstractPlotTool). This was formerly known as
   *   PlotToolCurve. With this class the programmer can set the cube view
   *   port that the curve is associated with along with the
   *   vertices on the viewport of which the curve gets it data. With this
   *   information, the plot curve can rename itself and paint its origin points
   *   onto cube viewports.
   *
   * @author ????-??-?? Stacy Alley
   *
   * @internal
   *   @history 2012-01-18 Steven Lambright and Tracie Sucharski - Renamed
   *                           class to CubePlotCurve. Changed
   *                           functionality dramatically to support a new
   *                           plotting infrastructure.
   *   @history 2012-01-20 Steven Lambright and Jai Rideout - Completed
   *                           documentation.
   *   @history 2012-01-30 Steven Lambright - Fixed a bug added when the
   *                           legendItem() method was removed. This method is
   *                           necessary. Fixes #688.
   */
  class CubePlotCurve : public QObject, public PlotCurve {
      Q_OBJECT

    public:
      CubePlotCurve(Units xUnits, Units yUnits);
      CubePlotCurve(const QByteArray &parentAndChildData);

      bool eventFilter(QObject *o, QEvent *e);
      void paint(CubeViewport *vp, QPainter *painter);
      QList <QPointF > sourceVertices() const;
      virtual QWidget *legendItem() const;
      QString sourceCube() const;

      void enableAutoRenaming(bool);
      void copySource(const CubePlotCurve &other);
      void setSource(CubeViewport *cvp, QList<QPoint> screenPoints,
                     int band = -1);

    signals:
      /**
       * This is emitted when the curve is modified in such a way that it would
       *   paint a viewport differently.
       */
      void needsRepaint();
      /**
       * This is emitted just before the cube plot curve is deleted. This is
       *   used to queue the deletes to happen at a safe time.
       */
      void removing();

    private slots:
      void updateLegendItemWidget();

    private:
      QMimeData *createMimeData() const;
      QByteArray toByteArray() const;
      void mousePressEvent(QMouseEvent *e);

    private:
      //! This is the widget legend item associated with this curve.
      QPointer<QWidget> m_legendItem;

      //! This is the curve's name before any automatic renaming happened.
      QString m_originalName;

      /**
       * This indicates if we are allowed to modify the title of this curve when
       *   the source (origin) data changes.
       */
      bool m_renameAutomatically;

      //! List of vertices in sample,line coordinates from the rubber band
      QList <QPointF> m_pointList;

      //! The cube that the data is coming from
      QString m_sourceCube;
  };
};

//! This allows CubePlotCurves to be stored inside of QVariants.
Q_DECLARE_METATYPE(Isis::CubePlotCurve *);

#endif


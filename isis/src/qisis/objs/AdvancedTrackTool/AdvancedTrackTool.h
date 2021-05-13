#ifndef Qisis_AdvancedTrackTool_h
#define Qisis_AdvancedTrackTool_h

/** This is free and unencumbered software released into the public domain.

The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */

// The only includes allowed in this file are the direct parents of this class!
#include "Tool.h"
#include <QList>
#include <QString>

class QAction;

namespace Isis {

  class TableMainWindow;
  class MdiCubeViewport;

  /**
   * @brief Tool to display info for a point on a cube
   *
   * This tool is part of the Qisis namespace and allows the user to view and
   * record information from a point on a cube such as line, sample, band, lats,
   * longs, etc.
   *
   * @ingroup Visualization Tools
   *
   * @author ????-??-?? Unknown
   *
   * @internal
   *  @history 2008-06-25 Noah Hilt - Added enumeration for different column values.
   *  @history 2008-10-14 Noah Hilt - Added Projected X and Y values to the table.
   *  @history 2008-10-17 Noah Hilt - Added tooltips to certain items in the table
   *                          that did not have descriptive names.
   *  @history 2010-02-17 Sharmila Prasad -Added the attributes TRACK_MOSAIC_INDEX,
   *                          TRACK_MOSAIC_FILENAME, TRACK_MOSAIC_SERIAL_NUM to track
   *                          mosaic origin
   *  @history 2010-03-08 Jeannie Walldren - Added record() slot to be able to record a
   *                          QPoint passed from the FindTool to the current row.
   *  @history 2010-05-07 Eric Hyer - record() SLOT now shows the table as well
   *  @history 2010-06-26 Eric Hyer - Class now uses MdiCubeViewport, also fixed
   *                          include issues
   *  @history 2011-02-16 Sharmila Prasad - Added columns for Local Emission and
   *                          Incidence Angles
   *  @history 2012-06-22 Kimberly Oyama and Steven Lambright - Added a help menu to
   *                          the menu bar and a help dialog that displays when the
   *                          tool is opened the first time and when the user opens
   *                          it through the help menu. Fixes #772.
   *  @history 2012-07-06 Debbie A. Cook, Updated Spice members to be more compliant with Isis
   *                          coding standards. References #972.
   *  @history 2012-11-30 Debbie A. Cook - Changed to use TProjection instead of Projection.
   *                          References #775.
   *  @history 2013-04-24 Jeannie Backer - Modified to print "N/A" for NorthAzimuth if
   *                          projection is not triaxial since this value is meaningless for
   *                          ring plane projections. References #775.
   *  @history 2014-06-17 Jeannie Backer - Modified to print set value to empty strings for
   *                          photometric angles, azimuths, resolutions if not valid.
   *                          References #1659.
   *  @history 2015-05-13 Makayla Shepherd - Modified to improve handling of
   *                          undefined slot behavior. References #2210.
   *  @history 2015-06-19 Makayla Shepherd - Added distorted Focal Plane and undistorted
   *                          Focal Plane to the table. References #1953.
   *  @history 2015-12-21 Makayla Shepherd - Changed the ordering of the enum to match the order
   *                          of when the columns are added, in order to fix the incidence and
   *                          emission angle columns from reporting incorrect numbers. Fixes #2381.
   *  @history 2017-11-13 Adam Goins - Updated the record(QPoint p) function to call showTable()
   *                          before it attempts to record a point so that a table is created
   *                          to record the point into so that the first recorded point is drawn.
   *                          Fixes #5143.
   *  @history 2018-07-18 Kristin Berry and Kaitlyn Lee - Updated TrackMosaicOrigin to work with
   *                          an external tracking cube.
   *  @history 2018-07-31 Kaitlyn Lee - Updated TrackMosaicOrigin to use a TrackingTable object
   *                          to get the file name, serial number, and index of the image associated
   *                          with the current pixel. Moved code opening the tracking cube to
   *                          CubeViewport. If the cursor is over a pixel with no tracking info,
   *                          file name and serial number display N/A now.
   *  @history 2018-08-13 Summer Stapleton - Added logic to trackingMosaicOrigin() for the tracking
   *                          cube (in addition to the mosaic cube) and added logic to track the
   *                          serial number of all other cubes. Fixes #4899
   *  @history 2019-04-22 Kaitlyn Lee - Added column for oblique pixel resolution. Added
   *                          checkBoxItems and loop to add elments to the AdvancedTrackTool,
   *                          instead of hardcoded method calls. Instead of using the enum,
   *                          added a method getIndex() that calculates what column the
   *                          element should be added to. Fixes #2205.
   */
  class AdvancedTrackTool : public Tool {
      Q_OBJECT

    public:
      AdvancedTrackTool(QWidget *parent);
      void addTo(QMenu *menu);
      void addToPermanent(QToolBar *perm);
      bool eventFilter(QObject *o, QEvent *e);

    public slots:
      virtual void mouseMove(QPoint p);
      virtual void mouseLeave();
      void record(QPoint p);

    protected:
      /**
       * This method returns the menu name associated with this tool.
       *
       *
       * @return QString
       */
      QString menuName() const {
        return "&Options";
      };

    private slots:
      void updateRow(QPoint p);
      void updateRow(MdiCubeViewport *cvp, QPoint p, int row);
      void record();
      void updateID();
      void TrackMosaicOrigin(MdiCubeViewport *cvp, int piLine, int piSample,
                             int &piOrigin, QString &psSrcFileName,
                             QString &psSrcSerialNum);
      void helpDialog();

    private:
      void readSettings();
      void writeSettings();
      QString settingsFilePath() const;
      int getIndex(QString keyword);

      // Used to store information about each check box to later add to the table
      // New entries can be added anywhere in the QList.
      // Format: QList<QString>({<Header>, <onByDefault>, <menuText>, <toolTip>}) <<
      // If a toolTip is not needed, use "".
      QList<QList<QString> > checkBoxItems = QList< QList<QString> >() <<
        QList<QString>({"Id", "false", "Id", ""}) <<
        QList<QString>({"Sample:Line", "true", "Sample:Line", ""}) <<
        QList<QString>({"Band", "false", "Band", ""}) <<
        QList<QString>({"Pixel", "true", "Pixel", ""}) <<
        QList<QString>({"Planetocentric Latitude", "true", "Planetocentric Lat", ""}) <<
        QList<QString>({"Planetographic Latitude", "false", "Planetographic Lat", ""}) <<
        QList<QString>({"360 Positive East Longitude", "true", "360 East Longitude", ""}) <<
        QList<QString>({"360 Positive West Longitude", "false", "360 West Longitude", ""}) <<
        QList<QString>({"180 Positive East Longitude", "true", "180 East Longitude", ""}) <<
        QList<QString>({"180 Positive West Longitude", "false", "180 West Longitude", ""}) <<
        QList<QString>({"Projected X:Projected Y", "false", "Projected X:Projected Y",
                         "X and Y values for a projected image"}) <<
        QList<QString>({"Local Radius", "false", "Radius", ""}) <<
        QList<QString>({"Point X:Point Y:Point Z", "false", "XYZ",
                         "The X, Y, and Z of surface intersection in body-fixed coordinates"}) <<
        QList<QString>({"Right Ascension:Declination", "false", "Ra:Dec",
                         "Right Ascension and Declination"}) <<
        QList<QString>({"Resolution", "false", "Resolution", ""}) <<
        QList<QString>({"Oblique Pixel Resolution", "false", "Oblique Pixel Res", ""}) <<
        QList<QString>({"Phase", "false", "Phase", ""}) <<
        QList<QString>({"Incidence", "false", "Incidence", ""}) <<
        QList<QString>({"Emission", "false", "Emission", ""}) <<
        QList<QString>({"LocalIncidence", "false", "LocalIncidence", ""}) <<
        QList<QString>({"LocalEmission", "false", "LocalEmission", ""}) <<
        QList<QString>({"North Azimuth", "false", "North Azimuth", ""}) <<
        QList<QString>({"Sun Azimuth", "false", "Sun Azimuth", ""}) <<
        QList<QString>({"Solar Longitude", "false", "Solar Longitude", ""}) <<
        QList<QString>({"Spacecraft X:Spacecraft Y:Spacecraft Z", "false", "Spacecraft Position",
                           "The X, Y, and Z of the spacecraft position"}) <<
        QList<QString>({"Spacecraft Azimuth", "false", "Spacecraft Azimuth", ""}) <<
        QList<QString>({"Slant Distance", "false", "Slant Distance", ""}) <<
        QList<QString>({"Focal Plane X:Focal Plane Y", "false", "Focal Plane X:Y", ""}) <<
        QList<QString>({"Undistorted Focal X:Undistorted Focal Y:Undistorted Focal Z",
                           "false", "Undistorted Focal Plane X:Y:Z", ""}) <<
        QList<QString>({"Ephemeris Time", "false", "Ephemeris iTime", ""}) <<
        QList<QString>({"Local Solar Time", "false", "Local Solar iTime", ""}) <<
        QList<QString>({"UTC", "false", "UTC", "Internal time in UTC format"}) <<
        QList<QString>({"Path", "false", "Path", ""}) <<
        QList<QString>({"FileName", "false", "FileName", ""}) <<
        QList<QString>({"Serial Number", "false", "Serial Number", ""}) <<
        QList<QString>({"Track Mosaic Index", "false", "Track Mosaic Index", ""}) <<
        QList<QString>({"Track Mosaic FileName", "false", "Track Mosaic FileName", ""}) <<
        QList<QString>({"Track Mosaic Serial Number", "false", "Track Mosaic Serial Number", ""}) <<
        QList<QString>({"Notes", "false", "Notes", ""});

      QAction *p_action;                   //!< Action to bring up the track tool
      int p_numRows;                       //!< The number of rows in the table
      int p_id;                            //!< The record id
      TableMainWindow *p_tableWin;         //!< The table window
      bool m_showHelpOnStart;              //!< True to show dialog When tool is started
  };
};

#endif

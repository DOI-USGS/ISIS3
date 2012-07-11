#ifndef Qisis_AdvancedTrackTool_h
#define Qisis_AdvancedTrackTool_h
/**
 * @file
 * $Revision: 1.18 $
 * $Date: 2010/06/28 08:47:51 $
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

// The only includes allowed in this file are the direct parents of this class!
#include "Tool.h"

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
   *  @history 2008-06-25 Noah Hilt - Added enumeration for different column
   *           values.
   *  @history 2008-10-14 Noah Hilt - Added Projected X and Y values to the table.
   *  @history 2008-10-17 Noah Hilt - Added tooltips to certain items in the table
   *           that did not have descriptive names.
   *  @history 2010-02-17 Sharmila Prasad -Added the attributes
   *           TRACK_MOSAIC_INDEX, TRACK_MOSAIC_FILENAME,
   *           TRACK_MOSAIC_SERIAL_NUM to track mosaic origin
   *  @history 2010-03-08 Jeannie Walldren - Added
   *           record() slot to be able to record a
   *           QPoint passed from the FindTool to the current row.
   *  @history 2010-05-07 Eric Hyer - record() SLOT now shows the table as well
   *  @history 2010-06-26 Eric Hyer - Class now uses MdiCubeViewport, also fixed
   *           include issues
   *  @history 2011-02-16 Sharmila Prasad - Added columns for Local Emission and
   *           Incidence Angles
   *  @history 2012-06-22 Kimberly Oyama and Steven Lambright - Added a help menu to
   *                          the menu bar and a help dialog that displays when the
   *                          tool is opened the first time and when the user opens
   *                          it through the help menu. Fixes #772.
   *  @history 2012-07-06 Debbie A. Cook, Updated Spice members to be more compliant with Isis 
   *           coding standards. References #972.
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
                             int &piOrigin, std::string &psSrcFileName,
                             std::string &psSrcSerialNum);
      void helpDialog();

    private:
      void readSettings();
      void writeSettings();
      QString settingsFilePath() const;
      
      /**
       * Enum for column values
       */
      enum {
        ID,                     //!< The record ID
        SAMPLE,                 //!< The current sample
        LINE,                   //!< The current line
        BAND,                   //!< The current band
        PIXEL,                  //!< The current pixel
        PLANETOCENTRIC_LAT,     //!< The planetocentric latitude for this point
        PLANETOGRAPHIC_LAT,     //!< The planetographic latitude for this point
        EAST_LON_360,           //!< The 360 east longitude for this point
        WEST_LON_360,           //!< The 360 west longitude for this point
        EAST_LON_180,           //!< The 180 east longitude for this point
        WEST_LON_180,           //!< The 180 west longitude for this point
        PROJECTED_X,            //!< Projected X value for valid projections
        PROJECTED_Y,            //!< Projected Y value for valid projections
        RADIUS,                 //!< The radius for this point
        POINT_X,                //!< The x value for this point
        POINT_Y,                //!< The y value for this point
        POINT_Z,                //!< The z value for this point
        RIGHT_ASCENSION,        //!< The right ascension for this point
        DECLINATION,            //!< The declination for this point
        RESOLUTION,             //!< The resoultion for this point
        PHASE,                  //!< The phase for this point
        INCIDENCE,              //!< The incidence for this point
        EMISSION,               //!< The emission for this point
        LOCAL_INCIDENCE,        //!< The local incidence for this point
        LOCAL_EMISSION,         //!< The local emission for this point
        NORTH_AZIMUTH,          //!< The north azimuth for this cube
        SUN_AZIMUTH,            //!< The sun azimuth for this cube
        SOLAR_LON,              //!< The solar longitude for this point
        SPACECRAFT_X,           //!< The spacecraft x position for this cube
        SPACECRAFT_Y,           //!< The spacecraft y position for this cube
        SPACECRAFT_Z,           //!< The spacecraft z position for this cube
        SPACECRAFT_AZIMUTH,     //!< The spacecraft azimuth for this cube
        SLANT,                  //!< The slant for this cube
        EPHEMERIS_TIME,         //!< The ephemeris time for this cube
        SOLAR_TIME,             //!< The local solar time for this cube
        UTC,                    //!< The UTC for this cube
        PATH,                   //!< The path for this cube
        FILENAME,               //!< The filename for this cube
        SERIAL_NUMBER,          //!< The serial number for this cube
        TRACK_MOSAIC_INDEX,     //!< Track the origin of the Mosaic, display the zero based index
        TRACK_MOSAIC_FILENAME,  //!< Track the origin of the Mosaic, display file name
        TRACK_MOSAIC_SERIAL_NUM,//!< Track the origin of the Mosaic, display file name
        NOTES                   //!< Any notes for this record
      };
      QAction *p_action;                   //!< Action to bring up the track tool
      int p_numRows;                       //!< The number of rows in the table
      int p_id;                            //!< The record id
      TableMainWindow *p_tableWin;  //!< The table window
      bool m_showHelpOnStart;              //!< True to show dialog When tool is started

  };

};

#endif

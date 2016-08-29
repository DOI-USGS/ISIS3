#ifndef CameraStatistics_h
#define CameraStatistics_h

/**
 * @file
 * $Revision: 1.16 $
 * $Date: 2010/06/15 18:27:43 $
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

#include <QString>

namespace Isis {
  class Camera;
  class Pvl;
  class PvlKeyword;
  class Statistics;

  /**
   * @brief Calculates a series of statistics pertaining to a Camera.
   *
   * Given a Camera pointer--or the filename of a Cube whose Camera is to be
   * used--this class will calculate a series of statistics at initialization
   * on the Camera.  After construction, the user can retrieve statistics,
   * compiled for every line/sample of the Camera, for the Camera's latitude,
   * longitude, pixel resolution, sample resolution, line resolution, phase
   * angle, emission angle, incidence angle, local solar time, meters, north
   * azimuth, and aspect ratio.
   *
   * @ingroup SpiceInstrumentsAndCameras
   *
   * @author 2011-06-14 Travis Addair
   *
   * @internal
   * @history 2011-06-14 Travis Addair - Extracted logic from "camstats"
   *                     application to create this class.
   * @history 2016-08-17 Tyler Wilson - Added Statistics objects for
   *                     ObliquePixelResolution,ObliqueSampleResolution, and
   *                     ObliqueLineResolution.  References #476, #4100.
   */
  class CameraStatistics {
    public:
      CameraStatistics(QString filename, int sinc, int linc);
      CameraStatistics(Camera *cam, int sinc, int linc);
      CameraStatistics(Camera *cam, int sinc, int linc, QString filename);
      virtual ~CameraStatistics();

      void addStats(Camera *cam, int &sample, int &line);
      PvlKeyword constructKeyword(QString keyname, double value,
          QString unit) const;
      Pvl toPvl() const;


      /**
       * Accessor method for inspecting the statistics gathered on the
       * Universal Latitudes of the input Camera.
       *
       * @return Statistics * Constant pointer to Latitude Statistics
       */
      const Statistics * getLatStat() const {
        return m_latStat;
      };


      /**
       * Accessor method for inspecting the statistics gathered on the
       * Universal Longitudes of the input Camera.
       *
       * @return Statistics * Constant pointer to Longitude Statistics
       */
      const Statistics * getLonStat() const {
        return m_lonStat;
      };



      /**
       * Accessor method for inspecting the statistics gathered on the
       * Pixel Resolutions of the input Camera.
       *
       * @return Statistics * Constant pointer to Pixel Resolution Statistics
       */
      const Statistics * getResStat() const {
        return m_resStat;
      };



      /**
       * Accessor method for inspecting the statistics gathered on the
       * oblique pixel resolutions of the input Camera.
       *
       * @return Statistics * Constant pointer to oblique pixel resolution statistics
       */
      const Statistics * getObliqueResStat() const {
        return m_obliqueResStat;
      };

      /**
       * Accessor method for inspecting the statistics gathered on the
       * oblique sample resolutions of the input Camera.
       *
       * @return Statistics * Constant pointer to oblique sample resolution statistics
       */
      const Statistics * getObliqueSampleResStat() const {
        return m_obliqueSampleResStat;
      };



      /**
       * Accessor method for inspecting the statistics gathered on the
       * oblique line resolution of the input Camera.
       *
       * @return Statistics * Constant pointer to oblique line resolution statistics
       */
      const Statistics * getObliqueLineResStat() const {
        return m_obliqueLineResStat;
      };



      /**
       * Accessor method for inspecting the statistics gathered on the
       * Sample Resolutions of the input Camera.
       *
       * @return Statistics * Constant pointer to Sample Resolution Statistics
       */
      const Statistics * getSampleResStat() const {
        return m_sampleResStat;
      };


      /**
       * Accessor method for inspecting the statistics gathered on the
       * Line Resolution of the input Camera.
       *
       * @return Statistics * Constant pointer to Line Resolution Statistics
       */
      const Statistics * getLineResStat() const {
        return m_lineResStat;
      };


      /**
       * Accessor method for inspecting the statistics gathered on the
       * Aspect Ratios of the input Camera.
       *
       * @return Statistics * Constant pointer to Aspect Ratio Statistics
       */
      const Statistics * getAspectRatioStat() const {
        return m_aspectRatioStat;
      };


      /**
       * Accessor method for inspecting the statistics gathered on the
       * Phase Angles of the input Camera.
       *
       * @return Statistics * Constant pointer to Phase Angle Statistics
       */
      const Statistics * getPhaseStat() const {
        return m_phaseStat;
      };


      /**
       * Accessor method for inspecting the statistics gathered on the
       * Emission Angles of the input Camera.
       *
       * @return Statistics * Constant pointer to Emission Angle Statistics
       */
      const Statistics * getEmissionStat() const {
        return m_emissionStat;
      };


      /**
       * Accessor method for inspecting the statistics gathered on the
       * Incidence Angles of the input Camera.
       *
       * @return Statistics * Constant pointer to Incidence Angle Statistics
       */
      const Statistics * getIncidenceStat() const {
        return m_incidenceStat;
      };


      /**
       * Accessor method for inspecting the statistics gathered on the
       * Local Solar Times of the input Camera.
       *
       * @return Statistics * Constant pointer to Local Solar Time Statistics
       */
      const Statistics * getLocalSolarTimeStat() const {
        return m_localSolarTimeStat;
      };


      /**
       * Accessor method for inspecting the statistics gathered on the
       * Local Radii (in meters) of the input Camera.
       *
       * @return Statistics * Constant pointer to Local Radius Statistics
       */
      const Statistics * getLocalRaduisStat() const {
        return m_localRaduisStat;
      };


      /**
       * Accessor method for inspecting the statistics gathered on the
       * North Azimuths of the input Camera.
       *
       * @return Statistics * Constant pointer to North Azimuth Statistics
       */
      const Statistics * getNorthAzimuthStat() const {
        return m_northAzimuthStat;
      };

    private:
      void init(Camera *cam, int sinc, int linc, QString filename);

      //!< FileName of the Cube the Camera was derived from.
      QString m_filename;
      int m_sinc; //!< Sample increment for composing statistics.
      int m_linc; //!< Line increment for composing statistics.

      Statistics *m_latStat; //!< Universal latitude statistics.
      Statistics *m_lonStat; //!< Universal longitude statistics.



      Statistics *m_obliqueResStat;  //!< Oblique pixel resolution statistics.
      Statistics *m_obliqueSampleResStat; //!< Oblique sample resolution statistics.
      Statistics *m_obliqueLineResStat; //!< Oblique line resolution statistics.

      Statistics *m_resStat; //!< Pixel resolution statistics.
      Statistics *m_sampleResStat; //!< Sample resolution statistics.
      Statistics *m_lineResStat; //!< Line resolution statistics.
      Statistics *m_aspectRatioStat; //!< Aspect ratio statistics.
      Statistics *m_phaseStat; //!< Phase angle statistics.
      Statistics *m_emissionStat; //!< Emission angle statistics.
      Statistics *m_incidenceStat; //!< Incidence angle statistics.
      Statistics *m_localSolarTimeStat; //!< Local solar time statistics.
      Statistics *m_localRaduisStat; //!< Local radius statistics (in meters).
      Statistics *m_northAzimuthStat; //!< North azimuth statistics.
  };
};

#endif


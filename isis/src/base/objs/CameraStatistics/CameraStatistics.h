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

#ifndef CameraStatistics_h
#define CameraStatistics_h

#include <string>

namespace Isis {
  class Camera;
  class Pvl;
  class PvlKeyword;
  class Statistics;

  class CameraStatistics {
    public:
      CameraStatistics(std::string filename, int sinc, int linc);
      CameraStatistics(Camera *cam, int sinc, int linc);
      CameraStatistics(Camera *cam, int sinc, int linc, std::string filename);
      virtual ~CameraStatistics();

      void addStats(Camera *cam, int &sample, int &line);
      PvlKeyword constructKeyword(std::string keyname, double value,
          std::string unit) const;
      Pvl toPvl() const;

      const Statistics * getLatStat() const {
        return m_latStat;
      };

      const Statistics * getLonStat() const {
        return m_lonStat;
      };

      const Statistics * getResStat() const {
        return m_resStat;
      };

      const Statistics * getSampleResStat() const {
        return m_sampleResStat;
      };

      const Statistics * getLineResStat() const {
        return m_lineResStat;
      };

      const Statistics * getAspectRatioStat() const {
        return m_aspectRatioStat;
      };

      const Statistics * getPhaseStat() const {
        return m_phaseStat;
      };

      const Statistics * getEmissionStat() const {
        return m_emissionStat;
      };

      const Statistics * getIncidenceStat() const {
        return m_incidenceStat;
      };

      const Statistics * getLocalSolarTimeStat() const {
        return m_localSolarTimeStat;
      };

      const Statistics * getLocalRaduisStat() const {
        return m_localRaduisStat;
      };

      const Statistics * getNorthAzimuthStat() const {
        return m_northAzimuthStat;
      };

    private:
      void init(Camera *cam, int sinc, int linc, std::string filename);

      std::string m_filename;
      int m_sinc;
      int m_linc;

      Statistics *m_latStat;
      Statistics *m_lonStat;
      Statistics *m_resStat;
      Statistics *m_sampleResStat;
      Statistics *m_lineResStat;
      Statistics *m_aspectRatioStat;
      Statistics *m_phaseStat;
      Statistics *m_emissionStat;
      Statistics *m_incidenceStat;
      Statistics *m_localSolarTimeStat;
      Statistics *m_localRaduisStat;
      Statistics *m_northAzimuthStat;
  };
};

#endif

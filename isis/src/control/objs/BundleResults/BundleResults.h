#ifndef BundleResults_h
#define BundleResults_h

/**
 * @file
 * $Revision: 1.20 $
 * $Date: 2009/10/15 01:35:17 $
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

class QDataStream;

namespace Isis {
  class FileName;
  class BundleSettings;
  class BundleStatistics;
  class PvlObject;
  /**
   * @brief Container class for BundleAdjustment results. 
   * This class includes the settings used to run the bundle adjustment, the resulting statistics 
   * values, and the name of the control network used.
   *  
   * @ingroup ControlNetworks
   *
   * @author 2014-07-08 Jeannie Backer
   *
   * @internal
   *   @history 2014-07-08 Jeannie Backer - Original version.
   *   @history 2014-07-23 Jeannie Backer - Added implementation for the QDataStream << and >>
   *                           operators and the read/write methods.
   *  
   */
  class BundleResults {
    public:
      BundleResults(BundleSettings inputSettings, FileName controlNetworkFileName);      
      BundleResults(const BundleResults &other);
      ~BundleResults();
      BundleResults &operator=(const BundleResults &other);
      void setOutputStatistics(BundleStatistics statisticsResults);
      PvlObject pvlObject(QString resultsName = "BundleResults",
                          QString settingsName = "InputSettings",
                          QString statisticsName = "StatisticsResults");
      QDataStream &write(QDataStream &stream) const;
      QDataStream &read(QDataStream &stream);
    private:
      BundleResults();
      FileName         *m_controlNetworkFileName;
      BundleSettings   *m_settings;
      BundleStatistics *m_statisticsResults;
  };
  // operators to read/write BundleResults to/from binary data
  QDataStream &operator<<(QDataStream &stream, const BundleResults &bundleResults);
  QDataStream &operator>>(QDataStream &stream, BundleResults &bundleResults);
};
#endif // BundleResults_h

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

#include "BundleSettings.h"
#include "BundleStatistics.h"
#include "ControlNet.h"
#include "PvlObject.h"

namespace Isis {

  class BundleResults {
    public:
      BundleResults(BundleSettings inputSettings, ControlNet outputControlNet, 
                    QString controlNetworkFileName = "");      
      BundleResults(const BundleResults &other);
      ~BundleResults();
      BundleResults &operator=(const BundleResults &other);
      void setOutputStatistics(BundleStatistics statisticsResults);
      PvlObject pvlObject(QString resultsName = "BundleResults",
                          QString settingsName = "InputSettings",
                          QString statisticsName = "StatisticsResults");
    private:
      BundleResults();
      QString    m_controlNetworkFileName;
      ControlNet *m_outputCNet;
      BundleSettings   *m_settings;
      BundleStatistics *m_statisticsResults;
  };
  // operators to read/write SparseBlockColumnMatrix to/from binary disk file
  // operator to write SparseBlockColumnMatrix to QDebug stream
  QDataStream&operator<<(QDataStream &stream, const BundleResults&);
  QDataStream&operator>>(QDataStream &stream, BundleResults&);
  QDebug operator<<(QDebug dbg, const BundleResults &bundleResults);
};
#endif // BundleResults_h

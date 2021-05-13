#ifndef _CONTROLNETSTATISTICS_H_
#define _CONTROLNETSTATISTICS_H_

/** This is free and unencumbered software released into the public domain.

The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */

#include <QMap>
#include <QVector>

#include "Progress.h"
#include "PvlGroup.h"
#include "SerialNumberList.h"
#include "Statistics.h"

namespace Isis {
  class ControlNet;
  class Progress;
  class PvlGroup;

  /**
   * @brief Control Network Stats
   *
   * This class is used to get statistics of Control Network by Image or by Point
   *
   * @ingroup ControlNetwork
   *
   * @author 2010-08-24 Sharmila Prasad
   *
   * @see ControlNetwork ControlPoint ControlMeasure
   *
   * @internal
   *   @history 2010-08-24 Sharmila Prasad Original version
   *   @history 2010-09-16 Sharmila Prasad Added individual image std::maps for each
   *                                       Point stats to correct segmentation
   *                                       faults.
   *   @history 2010-10-26 Tracie Sucharski Added missing includes to cpp after
   *                                       removing includes from ControlNet.h.
   *   @history 2011-05-03 Debbie A. Cook Added type "Constrained" to sPointType values
   *   @history 2011-06-07 Debbie A. Cook and Tracie Sucharski - Modified point types
   *                          Ground ------> Fixed
   *                          Tie----------> Free
   *   @history 2011-07-19 Sharmila Prasad - Modified for new keywords in binary control net
   *   @history 2011-11-03 Sharmila Prasad - Used ControlNet's CubeGraphNodes to get Image stats
   *                                         including Convex Hull Ratio
   *   @history 2011-12-21 Sharmila Prasad Fixed #634 to include stats of images not in the  ControlNet
   *   @history 2011-12-29 Sharmila Prasad Fixed #652 to include stats of ControlMeasure Log data
   *   @history 2012-04-26 Jai Rideout, Steven Lambright and Stuart Sides - Fixed results of min,
   *                           max, average computations. Verified results of convex hull
   *                           computation. Minor refactoring but needs a lot more. References #619
   *                           because fixing the order of the cnet caused us to fix some cnet graph
   *                           node bugs which caused the convex hull tests to fail.
   *  @history 2015-06-04 Kristin Berry - Now throws an error when unable to open an output file or
   *                           finish writing to an output file.
   *                           Fixes #996.
   *  @history 2017-12-12 Kristin Berry - Updated std::map to QMap and std::vector to QVector. Fixes
   *                           #5259.
   */
  class ControlNetStatistics {
    public:
      //! Constructor
      ControlNetStatistics(ControlNet *pCNet, const QString &psSerialNumFile, Progress *pProgress = 0);

      //! Constructor
      ControlNetStatistics(ControlNet *pCNet, Progress *pProgress = 0);

      //! Destructor
      ~ControlNetStatistics();

      //! Enumeration for Point Statistics
      enum ePointDetails { total, ignore, locked, fixed, constrained, freed };
      static const int numPointDetails = 6;

      //! Enumeration for Point int stats for counts such as valid points, measures etc.
      enum ePointIntStats { totalPoints, validPoints, ignoredPoints, fixedPoints, constrainedPoints, freePoints, editLockedPoints,
                            totalMeasures, validMeasures, ignoredMeasures, editLockedMeasures };
      static const int numPointIntStats = 11;

      //! Enumeration for Point stats like Tolerances, PixelShifts which have double data
      enum ePointDoubleStats { avgResidual, minResidual, maxResidual, minLineResidual, maxLineResidual, minSampleResidual, maxSampleResidual,
                               avgPixelShift, minPixelShift, maxPixelShift, minLineShift, maxLineShift, minSampleShift, maxSampleShift,
                               minGFit, maxGFit, minEccentricity, maxEccentricity, minPixelZScore, maxPixelZScore};
      static const int numPointDblStats = 20;

      //! Enumeration for image stats
      enum ImageStats { imgSamples, imgLines, imgTotalPoints, imgIgnoredPoints, imgFixedPoints, imgLockedPoints, imgLocked,
                        imgConstrainedPoints, imgFreePoints, imgConvexHullArea, imgConvexHullRatio };
      static const int numImageStats = 11;

      //! Generate stats like Total, Ignored, Fixed Points in an Image
      void GenerateImageStats();

      //! Print the Image Stats into specified output file
      void PrintImageStats(const QString &psImageFile);

      //! Returns the Image Stats by Serial Number
      QVector<double> GetImageStatsBySerialNum(QString psSerialNum) const;

      //! Generate stats like Ignored, Fixed, Total Measures, Ignored by Control Point
      void GeneratePointStats(const QString &psPointFile);

      //! Generate the Control Net Stats into the PvlGroup
      void GenerateControlNetStats(PvlGroup &pStatsGrp);

      //! Returns the Number of Valid (Not Ignored) Points in the Control Net
      int NumValidPoints() const {
        return (*mPointIntStats.find(validPoints));
      }

      //! Returns the Number of Fixed Points in the Control Net
      int NumFixedPoints() const {
        return (*mPointIntStats.find(fixedPoints));
      }

      //! Returns the number of Constrained Points in Control Net
      int NumConstrainedPoints() const {
        return (*mPointIntStats.find(constrainedPoints));
      }

      //! Returns the number of Constrained Points in Control Net
      int NumFreePoints() const {
        return (*mPointIntStats.find(freePoints));
      }

      //! Returns the number of ignored points
      int NumIgnoredPoints() const {
        return (*mPointIntStats.find(ignoredPoints));
      }

      //! Returns total number of edit locked points
      int NumEditLockedPoints() const {
        return (*mPointIntStats.find(editLockedPoints));
      }

      //! Returns the total Number of Measures in the Control Net
      int NumMeasures() const {
        return (*mPointIntStats.find(totalMeasures));
      }

      //! Returns the total Number of valid Measures in the Control Net
      int NumValidMeasures() const {
        return (*mPointIntStats.find(validMeasures));
      }

      //! Returns the total Number of Ignored Measures in the Control Net
      int NumIgnoredMeasures() const {
        return (*mPointIntStats.find(ignoredMeasures));
      }

      //! Returns total number of edit locked measures in the network
      int NumEditLockedMeasures() const {
        return (*mPointIntStats.find(editLockedMeasures));
      }

      //! Determine the average error of all points in the network
      double GetAverageResidual() const {
        return (*mPointDoubleStats.find(avgResidual));
      }

      //! Determine the minimum error of all points in the network
      double GetMinimumResidual() const {
        return (*mPointDoubleStats.find(minResidual));
      }

      //! Determine the maximum error of all points in the network
      double GetMaximumResidual() const {
        return (*mPointDoubleStats.find(maxResidual));
      }

      //! Determine the minimum line error of all points in the network
      double GetMinLineResidual() const {
        return (*mPointDoubleStats.find(minLineResidual));
      }

      //! Determine the minimum sample error of all points in the network
      double GetMinSampleResidual() const {
        return (*mPointDoubleStats.find(minSampleResidual));
      }

      //! Determine the maximum line error of all points in the network
      double GetMaxLineResidual() const {
        return (*mPointDoubleStats.find(maxLineResidual));
      }

      //! Determine the maximum sample error of all points in the network
      double GetMaxSampleResidual() const {
        return (*mPointDoubleStats.find(maxSampleResidual));
      }

      //! Get Min and Max LineShift
      double GetMinLineShift() const {
        return (*mPointDoubleStats.find(minLineShift));
      }

      //! Get network Max LineShift
      double GetMaxLineShift() const {
        return (*mPointDoubleStats.find(maxLineShift));
      }

      //! Get network Min SampleShift
      double GetMinSampleShift() const {
        return (*mPointDoubleStats.find(minSampleShift));
      }

      //! Get network Max SampleShift
      double GetMaxSampleShift() const {
        return (*mPointDoubleStats.find(maxSampleShift));
      }

      //! Get network Min PixelShift
      double GetMinPixelShift() const {
        return (*mPointDoubleStats.find(minPixelShift));
      }

      //! Get network Max PixelShift
      double GetMaxPixelShift() const {
        return (*mPointDoubleStats.find(maxPixelShift));
      }

      //! Get network Avg PixelShift
      double GetAvgPixelShift() const {
        return (*mPointDoubleStats.find(avgPixelShift));
      }

    protected:
      SerialNumberList mSerialNumList;           //!< Serial Number List
      ControlNet *mCNet;                         //!< Control Network
      Progress *mProgress;                       //!< Progress state

    private:
      QMap<int, int> mPointIntStats;           //!< Contains QMap of different count stats
      QMap<int, double> mPointDoubleStats;     //!< Contains QMap of different computed stats
      QMap<QString, QVector<double> > mImageMap; //!< Contains stats by Image/Serial Num
      QMap<QString, bool> mSerialNumMap;        //!< Whether serial# is part of ControlNet

      //! Get point count stats
      void GetPointIntStats();

      //! Get Point stats for Residuals and Shifts
      void GetPointDoubleStats();

      void UpdateMinMaxStats(const Statistics & stats,
                             ePointDoubleStats min,
                             ePointDoubleStats max);

      //! Init Pointstats std::vector
      void InitPointDoubleStats();

      //! Init SerialNum std::map
      void InitSerialNumMap();

      int numCNetImages;

      Statistics mConvexHullStats, mConvexHullRatioStats; //!< min, max, average convex hull stats
  };
}
#endif

#ifndef _CONTROLNETFILTER_H_
#define _CONTROLNETFILTER_H_

#include "ControlNetStatistics.h"
#include <fstream>

/** This is free and unencumbered software released into the public domain.

The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */

namespace Isis {
  class ControlNet;
  class ControlPoint;
  class ControlMeasure;

  /**
   * @brief Filter Control Network
   *
   * This class is used to filter Control Network based on
   * different options
   *
   * @ingroup ControlNetwork
   *
   * @author 2010-08-10 Sharmila Prasad
   *
   * @see ControlNetwork ControlPoint ControlMeasure
   *
   * @internal
   *  @history 2010-08-10 Sharmila Prasad - Original version
   *  @history 2010-09-16 Sharmila Prasad - Modified prototype for GetImageStatsBySerialNum API
   *                                        in sync with the ControlNetStatistics class
   *  @history 2010-09-27 Sharmila Prasad - Moved ParseExpression functionality to QString class
   *                                        Verify the DefFile in the PVL Class
   *  @history 2010-09-27 Sharmila Prasad - Made changes for the Binary Control Network
   *  @history 2010-10-04 Sharmila Prasad - Use QString's Token method instead of ParseExpression
   *  @history 2010-10-15 Sharmila Prasad - Display error on bad filter values
   *  @history 2010-11-09 Sharmila Prasad - Point_MeasureProperties,process 'All' measuretype
   *  @history 2011-01-17 Eric Hyer - Fixed breakages caused by ControlNet api
   *                          changes
   *   @history 2011-06-07 Debbie A. Cook and Tracie Sucharski - Modified point types
   *                          Ground ------> Fixed
   *                          Tie----------> Free
   *  @history 2011-07-22 Sharmila Prasad - Modified for new keywords in binary control net and added new
   *                               filters for ResidualTolerance, PixelShift and EditLock(Point & Measure)
   *  @history 2011-10-05 Sharmila Prasad - Report double values with 10 digit precision
   *  @history 2011-11-03 Sharmila Prasad - Added functionality to filter by Convex Hull Ratio
   *  @history 2011-12-29 Sharmila Prasad - Updated GoodnessOfFit Filter. Fixes Mantis #652
   *  @history 2017-08-08 Adam Goins - Changed references to SerialNumberList::Delete() to
   *                           SerialNumberList::remove()
   *  @history 2017-12-12 Kristin Berry - Updated to use QVector instead of std::vector. Fixes
   *                           #5259.
   */
  class ControlNetFilter : public ControlNetStatistics {
    public:
      //! Constructor
      ControlNetFilter(ControlNet *pCNet, QString &psSerialNumFile, Progress *pProgress = 0);

      //! Destructor
      ~ControlNetFilter();

      // Point Filters
      //! Filter Points by Pixel Shift
      void PointPixelShiftFilter(const PvlGroup &pvlGrp, bool pbLastFilter);

      //! Filter Points by Edit Lock
      void PointEditLockFilter(const PvlGroup &pvlGrp, bool pbLastFilter);

      //! Filter Points by Measure Edit Lock number
      void PointNumMeasuresEditLockFilter(const PvlGroup &pvlGrp, bool pbLastFilter);

      //! Filter Points by Residual Magnitude
      void PointResMagnitudeFilter(const PvlGroup &pvlGrp, bool pbLastFilter);

      //! Filter Points by GoodnessOfFit
      void PointGoodnessOfFitFilter(const PvlGroup & pvlGrp, bool pbLastFilter);

      //! Filter Points by Point ID Expression
      void PointIDFilter(const PvlGroup &pvlGrp, bool pbLastFilter);

      //! Filter Points by Number of measures
      void PointMeasuresFilter(const PvlGroup &pvlGrp, bool pbLastFilter);

      //! Filter Points by properties
      void PointPropertiesFilter(const PvlGroup &pvlGrp, bool pbLastFilter);

      //! Filter Points by Lat Lon Range
      void PointLatLonFilter(const PvlGroup &pvlGrp, bool pbLastFilter);

      //! Filter Points by distance between points
      void PointDistanceFilter(const PvlGroup &pvlGrp, bool pbLastFilter);

      //! Filter Points by Measure properties
      void PointMeasurePropertiesFilter(const PvlGroup &pvlGrp, bool pbLastFilter);

      //! Filter Points by Cube names
      void PointCubeNamesFilter(const PvlGroup &pvlGrp, bool pbLastFilter);

      //! Standard Point stats Header
      void PointStatsHeader(void);

      //! Standard Point Stats
      void PointStats(const ControlPoint &pcPoint);

      // Cube Filters
      //! Filter Cubes by Cube name expression
      void CubeNameExpressionFilter(const PvlGroup &pvlGrp, bool pbLastFilter);

      //! Filter Cubes by number of points in the cube
      void CubeNumPointsFilter(const PvlGroup &pvlGrp, bool pbLastFilter);

      //! Filter Cubes by Distance between points in a Cube
      void CubeDistanceFilter(const PvlGroup &pvlGrp, bool pbLastFilter);

      //! Filter Cubes by its ConvexHull Ratio
      void CubeConvexHullFilter(const PvlGroup &pvlGrp, bool pbLastFilter);

      //! Print the standard cube stats Header
      void CubeStatsHeader(void);

      //! Set the output print file
      void SetOutputFile(QString psPrintFile);

      void PrintCubeFileSerialNum(const ControlMeasure &pcMeasure);

    private:
      std::ofstream mOstm;                     //!< output stream for printing to output file
      SerialNumberList mSerialNumFilter;  //!< Serial Number List file

      void FilterOutPoint(int pindex);
      void FilterOutMeasuresBySerialNum(QString serialNum);
  };
}
#endif

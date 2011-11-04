#ifndef _CONTROLNETSTATISTICS_H_
#define _CONTROLNETSTATISTICS_H_

#include <map>
#include <iostream>
#include <vector>
#include "Progress.h"
#include "PvlGroup.h"
#include "SerialNumberList.h"
#include "Statistics.h"

using namespace std;

/**
 * @file
 * $Revision: $
 * $Date: $
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

namespace Isis {
  class ControlNet;
  class ControlCubeGraphNode;
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
   *   @history 2010-09-16 Sharmila Prasad Added individual image maps for each
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
   */
  class ControlNetStatistics {
    public:
      //! Constructor
      ControlNetStatistics(ControlNet *pCNet, const string &psSerialNumFile, Progress *pProgress = 0);

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
      enum ePointDoubleStats { avgResidual, maxResidual, minResidual, minLineResidual, maxLineResidual, minSampleResidual, maxSampleResidual,
                               avgPixelShift, maxPixelShift, minPixelShift, minLineShift, maxLineShift, minSampleShift, maxSampleShift};
      static const int numPointDblStats = 14;

      //! Enumeration for image stats
      enum ImageStats { imgSamples, imgLines, imgTotalPoints, imgIgnoredPoints, imgFixedPoints, imgLockedPoints, imgLocked, imgConstrainedPoints, imgFreePoints, imgConvexHullArea, imgConvexHullRatio };
      static const int numImageStats = 11;
      
      //! Generate stats like Total, Ignored, Fixed Points in an Image
      void GenerateImageStats(void);

      //! Print the Image Stats into specified output file
      void PrintImageStats(const string &psImageFile);

      //! Returns the Image Stats by Serial Number
      const vector<double> GetImageStatsBySerialNum(string psSerialNum);

      //! Generate stats like Ignored, Fixed, Total Measures, Ignored by Control Point
      void GeneratePointStats(const string &psPointFile);

      //! Generate the Control Net Stats into the PvlGroup
      void GenerateControlNetStats(PvlGroup &pStatsGrp);

      //! Returns the Number of Valid (Not Ignored) Points in the Control Net
      int NumValidPoints(){
          return mPointIntStats[validPoints];
      };

      //! Returns the Number of Fixed Points in the Control Net
      int NumFixedPoints(){
          return mPointIntStats[fixedPoints];
      };
      
      //! Returns the number of Constrained Points in Control Net
      int NumConstrainedPoints(){
          return mPointIntStats[constrainedPoints];
      };
      
      //! Returns the number of Constrained Points in Control Net
      int NumFreePoints(){
          return mPointIntStats[freePoints];
      };
      
      //! Returns the number of ignored points
      int NumIgnoredPoints(){
          return mPointIntStats[ignoredPoints];
      };
      
      //! Returns total number of edit locked points
      int NumEditLockedPoints() {
          return mPointIntStats[editLockedPoints];
      };

      //! Returns the total Number of Measures in the Control Net
      int NumMeasures(){
          return mPointIntStats[totalMeasures];
      };

      //!< Returns the total Number of valid Measures in the Control Net
      int NumValidMeasures(){
          return mPointIntStats[validMeasures];
      };

      //! Returns the total Number of Ignored Measures in the Control Net
      int NumIgnoredMeasures(){
          return mPointIntStats[ignoredMeasures];
      };

      //! Returns total number of edit locked measures in the network
      int NumEditLockedMeasures() {
          return mPointIntStats[editLockedMeasures];
      };
      
      //! Determine the average error of all points in the network
      double GetAverageResidual(){
          return mPointDoubleStats[avgResidual];
      };

      //! Determine the minimum error of all points in the network
      double GetMinimumResidual(){
          return mPointDoubleStats[minResidual];
      };

      //! Determine the maximum error of all points in the network
      double GetMaximumResidual(){
          return mPointDoubleStats[maxResidual];
      };

      //! Determine the minimum line error of all points in the network
      double GetMinLineResidual(){
          return mPointDoubleStats[minLineResidual];
      };

      //! Determine the minimum sample error of all points in the network
      double GetMinSampleResidual(){
          return mPointDoubleStats[minSampleResidual];
      };

      //! Determine the maximum line error of all points in the network
      double GetMaxLineResidual(){
          return mPointDoubleStats[maxLineResidual];
      };

      //! Determine the maximum sample error of all points in the network
      double GetMaxSampleResidual(){
          return mPointDoubleStats[maxSampleResidual]; 
      };
      
      //! Get Min and Max LineShift
      double GetMinLineShift(void) {
          return mPointDoubleStats[minLineShift];
      };
      
      //! Get network Max LineShift
      double GetMaxLineShift(void) {
      return mPointDoubleStats[maxLineShift];
      };      
      
      //! Get network Min SampleShift
      double GetMinSampleShift(){
          return mPointDoubleStats[minSampleShift]; 
      };
            
      //! Get network Max SampleShift
      double GetMaxSampleShift(){
          return mPointDoubleStats[maxSampleShift]; 
      };
      
      //! Get network Min PixelShift
      double GetMinPixelShift(){
          return mPointDoubleStats[minPixelShift]; 
      };

      //! Get network Max PixelShift
      double GetMaxPixelShift(){
          return mPointDoubleStats[maxPixelShift]; 
      };
      
      //! Get network Avg PixelShift
      double GetAvgPixelShift(){
          return mPointDoubleStats[avgPixelShift]; 
      };
      
    protected:
      SerialNumberList mSerialNumList;    //!< Serial Number List
      ControlNet *mCNet;                  //!< Control Network
      Progress *mProgress;                //!< Progress state
      QList< ControlCubeGraphNode * > mCubeGraphNodes;
      
    private:
      map <int, int> mPointIntStats;           //!< Contains map of different count stats
      map <int, double> mPointDoubleStats;     //!< Contains map of different computed stats
      map <string, vector<double> > mImageMap;
      
      //! Get point count stats
      void GetPointIntStats(void);
      
      //! Get Point stats for Residuals and Shifts 
      void GetPointDoubleStats(void);
      
      Statistics mConvexHullStats, mConvexHullRatioStats; //!< min, max, average convex hull stats
  };
}
#endif

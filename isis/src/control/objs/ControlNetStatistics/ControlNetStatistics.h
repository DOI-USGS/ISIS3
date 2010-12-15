#ifndef _CONTROLNETSTATISTICS_H_
#define _CONTROLNETSTATISTICS_H_

#include <map>
#include <iostream>
#include "Progress.h"
#include "PvlGroup.h"
#include "SerialNumberList.h"

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
  class Progress;
  class PvlGroup;

  //! Size of the PointDetails Array 
  #define IMAGE_POINT_SIZE 3

  //! Enumeration for Point Statistics
  enum ePointDetails { total, ignore, ground };
  
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
   */
  class ControlNetStatistics{
  public:
    //! Constructor
    ControlNetStatistics(ControlNet * pCNet, const string &psSerialNumFile, Progress *pProgress=0);
    
    //! Constructor
    ControlNetStatistics(ControlNet * pCNet, Progress *pProgress=0);

    //! Destructor
    ~ControlNetStatistics();
    
    //! Generate stats like Total, Ignored, Ground Points in an Image
    void GenerateImageStats(void);
    
    //! Print the Image Stats into specified output file
    void PrintImageStats(const string & psImageFile);
    
    //! Returns the Image Stats by Serial Number
    void GetImageStatsBySerialNum(string psSerialNum, int* piPointDetail, int piSize);
    
    //! Generate stats like Ignored, Ground, Total Measures, Ignored by Control Point
    void GeneratePointStats(const string & psPointFile);
    
    //! Generate the Control Net Stats into the PvlGroup
    void GenerateControlNetStats(PvlGroup & pStatsGrp);
    
    //! Returns the Number of Valid (Not Ignored) Points in the Control Net 
    int NumValidPoints();
    
    //! Returns the Number of Ground Points in the Control Net
    int NumGroundPoints();

    //! Returns the total Number of Measures in the Control Net
    int NumMeasures();
    
    //!< Returns the total Number of valid Measures in the Control Net
    int NumValidMeasures();
    
    //! Returns the total Number of Ignored Measures in the Control Net
    int NumIgnoredMeasures();
    
    //! Determine the average error of all points in the network
    double AverageResidual();
    
    //! Determine the minimum error of all points in the network
    double MinimumResidual();
    
    //! Determine the maximum error of all points in the network
    double MaximumResidual();
    
    //! Determine the minimum line error of all points in the network
    double MinimumLineResidual();
    
    //! Determine the minimum sample error of all points in the network
    double MinimumSampleResidual();
    
    //! Determine the maximum line error of all points in the network
    double MaximumLineResidual();
    
    //! Determine the maximum sample error of all points in the network
    double MaximumSampleResidual();
    
  protected:
    SerialNumberList mSerialNumList;    //!< Serial Number List
    ControlNet * mCNet;                 //!< Control Network
    Progress *mProgress;                //!< Progress state
    
  private:
    map <string, int> mImageTotalPointMap;   //!< Contains map of serial num and Total points
    map <string, int> mImageIgnorePointMap;  //!< Contains map of serial num and Ignored points
    map <string, int> mImageGroundPointMap;  //!< Contains map of serial num and Ground points
  };
}
#endif

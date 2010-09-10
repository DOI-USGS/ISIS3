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

  #define IMAGE_POINT_SIZE 4

  //!< Enumeration for Point Statistics
  enum ePointDetails { total, ignore, held, ground };
  
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
   */
  class ControlNetStatistics{
  public:
    //! Constructor
    ControlNetStatistics(ControlNet * pCNet, const string &psSerialNumFile, Progress *pProgress=0);
    
    //! Destructor
    ~ControlNetStatistics() {};
    
    //! Generate stats like Total Points, Ignored, Held by Image
    void GenerateImageStats(void);
    
    //! Print the Image Stats into specified output file
    void PrintImageStats(const string & psImageFile);
    
    //! Returns the Image Stats by Serial Number
    int* GetImageStatsBySerialNum(string psSerialNum) {
      return mImagePointMap[psSerialNum];
    }
    
    //! Generate stats like Ignored, Ground, Held, Total Measures, Ignored by Control Point
    void GeneratePointStats(const string & psPointFile);
    
    //! Generate the Control Net Stats into the PvlGroup
    void GenerateControlNetStats(PvlGroup & pStatsGrp);
    
    //! Returns the Number of Valid (Not Ignored) Points in the Control Net 
    int NumValidPoints();
    
    //! Returns the Number of Ground Points in the Control Net
    int NumGroundPoints();
    
    //! Returns the Number of Held Points in the Control Net
    int NumHeldPoints();
    
    //! Returns the total Number of Measures in the Control Net
    int NumMeasures();
    
    //!< Returns the total Number of valid Measures in the Control Net
    int NumValidMeasures();
    
    //! Returns the total Number of Ignored Measures in the Control Net
    int NumIgnoredMeasures();
    
    //! Determine the average error of all points in the network
    double AverageError();
    
    //! Determine the minimum error of all points in the network
    double MinimumError();
    
    //! Determine the maximum error of all points in the network
    double MaximumError();
    
    //! Determine the minimum line error of all points in the network
    double MinimumErrorLine();
    
    //! Determine the minimum sample error of all points in the network
    double MinimumErrorSample();
    
    //!< Determine the maximum line error of all points in the network
    double MaximumErrorLine();
    
    //! Determine the maximum sample error of all points in the network
    double MaximumErrorSample();
    
  protected:
    SerialNumberList mSerialNumList;    //!< Serial Number List
    ControlNet * mCNet;                 //!< Control Network
    Progress *mProgress;                //!< Progress state
    
  private:
    map <string, int*> mImagePointMap;  //!< Contains map of serial num and point stats
  };
}
#endif

#include "ControlNetStatistics.h"

#include "ControlNet.h"
#include "ControlPoint.h"
#include "ControlMeasure.h"
#include "Filename.h"
#include "iString.h"
#include "Progress.h"
#include "Pvl.h"
#include "SpecialPixel.h"

using namespace std;

namespace Isis {
  
  //! String names for Point Type
  string sPointType [] = { "Ground", "Tie" };

  //! String values for Boolean
  string sBoolean[]    = { "False", "True" };
  
  /**
   * ControlNetStatistics Constructor has ctor to it's base Control Network
   * 
   * @author Sharmila Prasad (8/24/2010)
   *  
   * @param pCNet - Input Control network
   * @param psSerialNumFile - Serial Number List file
   * @param pProgress - Check Progress if not Null
   */
  ControlNetStatistics::ControlNetStatistics(ControlNet * pCNet, const string & psSerialNumFile, Progress *pProgress)
  {
    mCNet = pCNet;
    mSerialNumList = SerialNumberList(psSerialNumFile);
    mProgress = pProgress;
  }
  
  /**
   * Constructor with  ControlNet
   * 
   * @author Sharmila Prasad (11/5/2010)
   * 
   * @param pCNet 
   * @param pProgress 
   */
  ControlNetStatistics::ControlNetStatistics(ControlNet * pCNet, Progress *pProgress)
  {
    mCNet = pCNet;
    mProgress = pProgress;
  }
  /**
   * Destructor
   * 
   * @author Sharmila Prasad (9/17/2010)
   */
  ControlNetStatistics::~ControlNetStatistics()
  {
    
  }
  
  /**
   * Generates the summary stats for the entire control network. 
   * Stats include Total images, Total, Valid, Ignored, Ground Points, 
   * Total, Valid, Ignored Measures and also Average, Min, Max Error, 
   * Min, Max Line and Sample Errors
   * 
   * @author Sharmila Prasad (8/25/2010)
   * 
   * @param pStatsGrp - Output Control Net Stats in Pvl format 
   */
  void ControlNetStatistics::GenerateControlNetStats(PvlGroup & pStatsGrp)
  {
    pStatsGrp = PvlGroup("ControlNetSummary");
    if (mSerialNumList.Size()) {
      pStatsGrp += PvlKeyword("TotalImages",     mSerialNumList.Size());
    }
    pStatsGrp += PvlKeyword("TotalPoints",       mCNet->Size());
    pStatsGrp += PvlKeyword("ValidPoints",       NumValidPoints());
    pStatsGrp += PvlKeyword("IgnoredPoints",     mCNet->Size() - NumValidPoints());
    pStatsGrp += PvlKeyword("GroundPoints",      NumGroundPoints());
    pStatsGrp += PvlKeyword("AverageResidual",   AverageResidual());

    double dError = MinimumResidual();
    pStatsGrp += PvlKeyword("MinimumResidual",   (dError == VALID_MAX4 ? "N/A" : iString(dError)));

    dError = MaximumResidual();
    pStatsGrp += PvlKeyword("MaximumResidual",   (dError == 0 ? "N/A" : iString(dError)));
    
    pStatsGrp += PvlKeyword("TotalMeasures",     NumMeasures());
    pStatsGrp += PvlKeyword("ValidMeasures",     NumValidMeasures());
    pStatsGrp += PvlKeyword("IgnoredMeasures",   NumIgnoredMeasures());
    
    dError = MinimumLineResidual();
    pStatsGrp += PvlKeyword("MinLineResidual",   (dError == 0 ? "N/A" : iString(dError)));
    
    dError = MinimumSampleResidual();
    pStatsGrp += PvlKeyword("MinSampleResidual", (dError == 0 ? "N/A" : iString(dError)));
    
    dError = MaximumLineResidual();
    pStatsGrp += PvlKeyword("MaxLineResidual",   (dError == 0 ? "N/A" : iString(dError)));
    
    dError = MaximumSampleResidual();
    pStatsGrp += PvlKeyword("MaxSampleResidual", (dError == 0 ? "N/A" : iString(dError)));
  }
  
  /**
   * Generate the statistics of a Control Network by Image 
   * Stats include Filename, Serial Num, and 
   * Total, Valid, Ignored, Ground Points in each Image 
   * 
   * @author Sharmila Prasad (8/24/2010)
   * 
   */
  void ControlNetStatistics::GenerateImageStats(void)
  {
    map<string,int>::iterator it;
    int iNumPoints = mCNet->Size();
    
    // Sort the ControlNet by PointID    
    //mCNet->SortControlNet();
    
    // Initialise the Progress object
    if(mProgress != NULL && iNumPoints > 0) {
      mProgress->SetText("Image Stats: Loading Control Points...");
      mProgress->SetMaximumSteps(iNumPoints);
      mProgress->CheckStatus();
    }
    
    for (int i=0; i<iNumPoints; i++) {
      const ControlPoint & cPoint = (*mCNet)[i];
      int iNumMeasures = cPoint.Size();
      bool bIgnore = cPoint.Ignore();
      bool bGround = (cPoint.Type()==ControlPoint::Ground ? true : false);
      
      for (int j=0; j<iNumMeasures; j++) {
        const ControlMeasure & cMeasure = cPoint[j];
        string sMeasureSN = cMeasure.GetCubeSerialNumber();
        it = mImageTotalPointMap.find(sMeasureSN);
        // initialize the maps
        if (mImageTotalPointMap.find(sMeasureSN) == mImageTotalPointMap.end()){
          mImageTotalPointMap [sMeasureSN]  = 0;
          mImageIgnorePointMap[sMeasureSN]  = 0;
          mImageGroundPointMap[sMeasureSN]  = 0;
        }        
        mImageTotalPointMap[sMeasureSN]++;
        if (bIgnore)  mImageIgnorePointMap[sMeasureSN]++;
        if (bGround)  mImageGroundPointMap[sMeasureSN]++;
      }
      // Update Progress
      if(mProgress != NULL) 
        mProgress->CheckStatus();
    }
  }

  /**
   * Print the Image Stats into specified output file
   * 
   * @author Sharmila Prasad (9/1/2010)
   * 
   * @param psImageFile - Output Image Stats File
   */
  void ControlNetStatistics::PrintImageStats(const string & psImageFile)
  {
    // Check if the image list has been provided
    if (!mSerialNumList.Size()) {
      string msg = "Serial Number of Images has not been provided to get Image Stats";
      throw Isis::iException::Message(Isis::iException::User, msg, _FILEINFO_);
    }
    
    Filename outFile(psImageFile);
    ofstream ostm;
    string outName(outFile.Expanded());
    ostm.open(outName.c_str(), std::ios::out);
    
    map<string,int>::iterator it;
    
    // Log into the output file
    ostm << "Filename" << ", " << "SerialNumber" << ", " << "Total Points" << ", " << "Ignore" << ", " << "Ground" << ", " <<  endl;
    for ( it=mImageTotalPointMap.begin(); it != mImageTotalPointMap.end(); it++ ){
      ostm << mSerialNumList.Filename((*it).first) << ", " << (*it).first << ", ";
      ostm << (*it).second << ", " << mImageIgnorePointMap[(*it).first] << ", " << mImageGroundPointMap[(*it).first] <<endl;
    }
    ostm.close();
  }
  
  /**
   * Returns the Image Stats by Serial Number
   * 
   * @author Sharmila Prasad (9/17/2010)
   * 
   * @param psSerialNum   - Image Serial Number File 
   * @param piPointDetail - Calculated Points stats (total, ignore, ground)
   * @param piSize        - array size
   */
  void ControlNetStatistics::GetImageStatsBySerialNum(string psSerialNum, int* piPointDetail, int piSize)
  {
    if (piSize < IMAGE_POINT_SIZE) {
      return;
    }
    piPointDetail[total]  = mImageTotalPointMap[psSerialNum];
    piPointDetail[ignore] = mImageIgnorePointMap[psSerialNum];
    piPointDetail[ground] = mImageGroundPointMap[psSerialNum];
  }
  
  /**
   * Generate the statistics of a Control Network by Point 
   * Stats include ID, Type of each Control Point and 
   * Total, Ignored measures in each Control Point
   * 
   * @author Sharmila Prasad (8/24/2010)
   * 
   * @param psPointFile - Output Point Statisitics File
   */
  void ControlNetStatistics::GeneratePointStats(const string & psPointFile)
  {
    Isis::Filename outFile(psPointFile);

    ofstream ostm;
    string outName(outFile.Expanded());
    ostm.open(outName.c_str(), std::ios::out);
    ostm << "Point Id, " << "Type, " << " Ignore, " << "Num Measures, " << "Ignored Measures" << endl;
    
    int iNumPoints = mCNet->Size();
    
    // Initialise the Progress object
    if(mProgress != NULL && iNumPoints > 0) {
      mProgress->SetText("Point Stats: Loading Control Points...");
      mProgress->SetMaximumSteps(iNumPoints);
      mProgress->CheckStatus();
    }
    
    for (int i=0; i<iNumPoints; i++) {
      const ControlPoint & cPoint = (*mCNet)[i];
      int iNumMeasures = cPoint.Size();
      int iIgnored = 0;
      for (int j=0; j<iNumMeasures; j++) {
        const ControlMeasure & cMeasure = cPoint[j];
        if (cMeasure.IsIgnored()) {
          iIgnored++;
        }
      }
      // Log into the output file
      ostm << cPoint.Id()   << ", " << sPointType[(int)cPoint.Type()] << ", " <<  sBoolean[(int)cPoint.Ignore()] << ", " ;
      ostm << iNumMeasures << ", " << iIgnored << endl;
      
      // Update Progress
      if(mProgress != NULL) 
        mProgress->CheckStatus();
    }
    ostm.close();
  }
  
  /**
   * Returns the Number of Valid (Not Ignored) Points in the Control Net 
   * Moved from ControlNet class 
   * 
   * @author Sharmila Prasad (9/8/2010)
   * 
   * @return int - Total Valid Points
   */
  int ControlNetStatistics::NumValidPoints()
  {
    int iCount = 0;
    int iNumPoints = mCNet->Size();
    
    for(int i = 0; i < iNumPoints; i++) {
      if(!(*mCNet)[i].Ignore())
        iCount ++;
    }
    
    return iCount;
  }

  /**
   * Returns the total number of Ground Points in the Control Network
   * Moved from ControlNet class 
   *  
   * @author Sharmila Prasad (9/8/2010)
   * 
   * @return int - Total Ground Points
   */
  int ControlNetStatistics::NumGroundPoints()
  {
    int iCount = 0;
    int iNumPoints = mCNet->Size();
    
    for(int i= 0; i < iNumPoints; i++) {
      if((*mCNet)[i].Type() == ControlPoint::Ground)
        iCount ++;
    }
    return iCount;
  }
  
  /**
   * Return the total number of measures for all control points 
   * in the network
   * Moved from ControlNet class 
   *  
   * @author Sharmila Prasad (9/8/2010)
   * 
   * @return int - Total Measures
   */
  int ControlNetStatistics::NumMeasures()
  {
    int iNumMeasures = 0;
    int iNumPoints = mCNet->Size();
    
    for(int i = 0; i < iNumPoints; i++) {
      iNumMeasures += (*mCNet)[i].Size();
    }
    return iNumMeasures;
  }

  /**
   * Return the number of valid (non-ignored) measures for 
   * all control points in the network 
   *  
   * Moved from ControlNet class 
   *  
   * @author Sharmila Prasad (9/8/2010)
   * 
   * @return int - Total Valid Measures
   */
  int ControlNetStatistics::NumValidMeasures()
  {
    int iNumValidMeasures = 0;
    int iNumPoints = mCNet->Size();
    
    for(int i = 0; i < iNumPoints; i++) {
      iNumValidMeasures += (*mCNet)[i].NumValidMeasures();
    }
    
    return iNumValidMeasures;
  }

  /**
   * Return the total number of ignored measures for all 
   * control points in the network 
   *  
   * Moved from ControlNet class 
   *  
   * @author Sharmila Prasad (9/8/2010)
   * 
   * @return int - Total Ignored Measures
   */
  int ControlNetStatistics::NumIgnoredMeasures()
  {
    int iNumIgnoredMeasures = 0;
    int iNumPoints = mCNet->Size();
    
    for(int i = 0; i < iNumPoints; i++) {
      const ControlPoint &cPoint = (*mCNet)[i];
      iNumIgnoredMeasures += cPoint.Size() - cPoint.NumValidMeasures();
    }
    
    return iNumIgnoredMeasures;
  }

  /**
   * Compute the average error of all points in the network
   *  
   * Moved from ControlNet class 
   *  
   * @author Sharmila Prasad (9/8/2010)
   * 
   * @return double - Average Error
   */
  double ControlNetStatistics::AverageResidual()
  {
    double dAvgError = 0.0;
    int iPointsCount = 0;
    int iNumPoints = mCNet->Size();
    
    for(int i = 0; i < iNumPoints; i++) {
      if((*mCNet)[i].Ignore()) continue;
      dAvgError += (*mCNet)[i].AverageResidual();
      iPointsCount++;
    }
    
    if(iPointsCount == 0) 
      return dAvgError;
    
    return dAvgError / iPointsCount;
  }

  /**
   * Determine the minimum error of all points in the network
   * Moved from ControlNet class 
   *  
   * @author Sharmila Prasad (9/8/2010)
   * 
   * @return double - Minimum Error
   */
  double ControlNetStatistics::MinimumResidual()
  {
    double dMinError = VALID_MAX4;
    int iNumPoints = mCNet->Size();
    
    for(int i = 0; i < iNumPoints; i++) {
      double dError = (*mCNet)[i].MinimumResidual();
      if(dError < dMinError) 
        dMinError = dError;
    }
    
    return dMinError;
  }

  /**
   * Determine the maximum error of all points in the network
   * Moved from ControlNet class 
   *  
   * @author Sharmila Prasad (9/8/2010)
   * 
   * @return double - Max Error
   */
  double ControlNetStatistics::MaximumResidual()
  {
    double dMaxError = 0.0;
    int iNumPoints = mCNet->Size();
    
    for(int i = 0; i < iNumPoints; i++) {
      double dError = (*mCNet)[i].MaximumResidual();
      if(dError > dMaxError)
        dMaxError = dError;
    }
    return dMaxError;
  }

  /**
   * Get the Minimum ErrorLine for the Control Network
   * 
   * @author Sharmila Prasad (9/8/2010)
   * 
   * @return double - Min Line Error
   */
  double ControlNetStatistics::MinimumLineResidual()
  {
    double dMinError = VALID_MAX4;
    int iNumPoints = mCNet->Size();
    
    for(int i = 0; i < iNumPoints; i++) {
       double dError = (*mCNet)[i].MinimumLineResidual();
       if(dError < dMinError)
         dMinError = dError;
     }
    
     return dMinError;
  }

  /**
   * Get the Minimum ErrorSample for the Control Network
   * 
   * @author Sharmila Prasad (9/8/2010)
   * 
   * @return double - Min Sample Error
   */
  double ControlNetStatistics::MinimumSampleResidual()
  {
    double dMinError = VALID_MAX4;
    int iNumPoints = mCNet->Size();
    
    for(int i = 0; i < iNumPoints; i++) {
       double dError = (*mCNet)[i].MinimumSampleResidual();
       if(dError < dMinError)
         dMinError = dError;
     }
    
     return dMinError;
  }

  /**
   * Get the Maximum ErrorLine for the Control Network
   * 
   * @author Sharmila Prasad (9/8/2010)
   * 
   * @return double - Max Line Error
   */
  double ControlNetStatistics::MaximumLineResidual()
  {
    double dMaxError = 0.0;
    int iNumPoints = mCNet->Size();
    
    for(int i = 0; i < iNumPoints; i++) {
       double dError = (*mCNet)[i].MaximumLineResidual();
       if(dError > dMaxError) 
         dMaxError = dError;
     }
     
     return dMaxError;
  }

  /**
   * Get the Maximum ErrorSample for the Control Network
   * 
   * @author Sharmila Prasad (9/8/2010)
   * 
   * @return double - Max Sample Error
   */
  double ControlNetStatistics::MaximumSampleResidual()
  {
    double dMaxError = 0.0;
    int iNumPoints = mCNet->Size();
    
    for(int i = 0; i < iNumPoints; i++) {
      double dError = (*mCNet)[i].MaximumSampleResidual();
      if(dError > dMaxError)
        dMaxError = dError;
    }
    
    return dMaxError;
  }
}

#include "ControlNet.h"
#include "ControlNetStatistics.h"
#include "Filename.h"
#include "iString.h"
#include "Progress.h"
#include "Pvl.h"

using namespace std;

namespace Isis {
  
  //!< String names for Point Type
  string sPointType [] = { "Ground", "Tie" };

  //!< String values for Boolean
  string sBoolean[]    = { "False", "True" };
  
  /**
   * ControlNetStatistics Constructor has ctor to it's base Control Network
   * 
   * @author Sharmila Prasad (8/24/2010)
   *  
   * @param pCNet - Input Control network
   * @param psSerialNumFile - Serial Number List file
   * @param progress - Check Progress if not Null
   */
  ControlNetStatistics::ControlNetStatistics(ControlNet * pCNet, const string & psSerialNumFile, Progress *pProgress)
  {
    mCNet = pCNet;
    mSerialNumList = SerialNumberList(psSerialNumFile);
    mProgress = pProgress;
  }
  
  /**
   * Generates the summary stats for the entire control network. 
   * Stats include Total images, Total, Valid, Ignored, Held, Ground Points, 
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
    
    pStatsGrp += PvlKeyword("TotalImages",       mSerialNumList.Size());
    pStatsGrp += PvlKeyword("TotalPoints",       mCNet->Size());
    pStatsGrp += PvlKeyword("ValidPoints",       NumValidPoints());
    pStatsGrp += PvlKeyword("IgnoredPoints",     mCNet->Size() - NumValidPoints());
    pStatsGrp += PvlKeyword("GroundPoints",      NumGroundPoints());
    pStatsGrp += PvlKeyword("HeldPoints",        NumHeldPoints());
    pStatsGrp += PvlKeyword("AverageError",      AverageError());

    double dError = MinimumError();
    pStatsGrp += PvlKeyword("MinErrorMagnitude", (dError == VALID_MAX4 ? "N/A" : iString(dError)));

    dError = MaximumError();
    pStatsGrp += PvlKeyword("MaxErrorMagnitude", (dError == 0 ? "N/A" : iString(dError)));
    
    pStatsGrp += PvlKeyword("TotalMeasures",     NumMeasures());
    pStatsGrp += PvlKeyword("ValidMeasures",     NumValidMeasures());
    pStatsGrp += PvlKeyword("IgnoredMeasures",   NumIgnoredMeasures());
    
    dError = MinimumErrorLine();
    pStatsGrp += PvlKeyword("MinLineError",      (dError == 0 ? "N/A" : iString(dError)));
    
    dError = MinimumErrorSample();
    pStatsGrp += PvlKeyword("MinSampleError",    (dError == 0 ? "N/A" : iString(dError)));
    
    dError = MaximumErrorLine();
    pStatsGrp += PvlKeyword("MaxLineError",      (dError == 0 ? "N/A" : iString(dError)));
    
    dError = MaximumErrorSample();
    pStatsGrp += PvlKeyword("MaxSampleError",    (dError == 0 ? "N/A" : iString(dError)));
  }
  
  /**
   * Generate the statistics of a Control Network by Image 
   * Stats include Filename, Serial Num, and 
   * Total, Valid, Ignored, Held, Ground Points in each Image 
   * 
   * @author Sharmila Prasad (8/24/2010)
   * 
   */
  void ControlNetStatistics::GenerateImageStats(void)
  {
    map<string,int*>::iterator it;
    int iNumPoints = mCNet->Size();
    int *iPointDetail;
    
    // Sort the ControlNet by PointID    
    mCNet->SortControlNet();
    
    // Initialise the Progress object
    if(mProgress != NULL) {
      mProgress->SetText("Image Stats: Loading Control Points...");
      mProgress->SetMaximumSteps(iNumPoints);
      mProgress->CheckStatus();
    }
    
    for (int i=0; i<iNumPoints; i++) {
      ControlPoint & cPoint = (*mCNet)[i];
      int iNumMeasures = cPoint.Size();
      bool bIgnore = cPoint.Ignore();
      bool bHeld   = cPoint.Held();
      bool bGround = (cPoint.Type()==ControlPoint::Ground ? true : false);
      
      for (int j=0; j<iNumMeasures; j++) {
        ControlMeasure & cMeasure = cPoint[j];
        string sMeasureSN = cMeasure.CubeSerialNumber();
        it = mImagePointMap.find(sMeasureSN);
        if (mImagePointMap.find(sMeasureSN) == mImagePointMap.end()){
          iPointDetail = new int(IMAGE_POINT_SIZE);
          iPointDetail[total]  = 0;
          iPointDetail[ignore] = 0;
          iPointDetail[held]   = 0;
          iPointDetail[ground] = 0;
          mImagePointMap[sMeasureSN] = iPointDetail;
        }
        iPointDetail = mImagePointMap[sMeasureSN];
        iPointDetail[total]++;
        if (bIgnore)  iPointDetail[ignore]++;
        if (bHeld)    iPointDetail[held]++;
        if (bGround)  iPointDetail[ground]++;
        
        iPointDetail = NULL;
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
    Filename outFile(psImageFile);
    ofstream ostm;
    string outName(outFile.Expanded());
    ostm.open(outName.c_str(), std::ios::out);
    
    map<string,int*>::iterator it;
    int *iPointDetail;
    
    // Log into the output file
    ostm << "Filename" << ", " << "SerialNumber" << ", " << "Total Points" << ", " << "Ignore" << ", " << "Ground" << ", " << "Held" << endl;
    for ( it=mImagePointMap.begin(); it != mImagePointMap.end(); it++ ){
      ostm << mSerialNumList.Filename((*it).first) << ", " << (*it).first << ", ";
      iPointDetail = (*it).second;
      ostm << iPointDetail[total] << ", " << iPointDetail[ignore] << ", " << iPointDetail[ground] << ", " << iPointDetail[held] <<endl;
    }
    ostm.close();
  }
  
  /**
   * Generate the statistics of a Control Network by Point 
   * Stats include ID, Type, Held of each Control Point and 
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
    ostm << "Point Id, " << "Type, " << " Ignore, " << "Held, " << "Num Measures, " << "Ignored Measures" << endl;
    ostm << endl;
    
    int iNumPoints = mCNet->Size();
    
    // Initialise the Progress object
    if(mProgress != NULL) {
      mProgress->SetText("Point Stats: Loading Control Points...");
      mProgress->SetMaximumSteps(iNumPoints);
      mProgress->CheckStatus();
    }
    
    for (int i=0; i<iNumPoints; i++) {
      ControlPoint & cPoint = (*mCNet)[i];
      int iNumMeasures = cPoint.Size();
      int iIgnored = 0;
      for (int j=0; j<iNumMeasures; j++) {
        ControlMeasure & cMeasure = cPoint[j];
        if (cMeasure.Ignore()) {
          iIgnored++;
        }
      }
      // Log into the output file
      ostm << cPoint.Id()   << ", " << sPointType[(int)cPoint.Type()] << ", " <<  sBoolean[(int)cPoint.Ignore()] << ", " ;
      ostm << sBoolean[(int)cPoint.Held()] << ", " << iNumMeasures << ", " << iIgnored << endl;
      
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
   * Returns the total number of Held Points in the Control Network
   * Moved from ControlNet class 
   *  
   * @author Sharmila Prasad (9/8/2010)
   * 
   * @return int - Total Held Points
   */
  int ControlNetStatistics::NumHeldPoints()
  {
    int iCount = 0;
    int iNumPoints = mCNet->Size();
    
    for(int i = 0; i < iNumPoints; i++) {
      if((*mCNet)[i].Held())
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
      ControlPoint &cPoint = (*mCNet)[i];
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
  double ControlNetStatistics::AverageError()
  {
    double dAvgError = 0.0;
    int iPointsCount = 0;
    int iNumPoints = mCNet->Size();
    
    for(int i = 0; i < iNumPoints; i++) {
      if((*mCNet)[i].Ignore()) continue;
      dAvgError += (*mCNet)[i].AverageError();
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
  double ControlNetStatistics::MinimumError()
  {
    double dMinError = VALID_MAX4;
    int iNumPoints = mCNet->Size();
    
    for(int i = 0; i < iNumPoints; i++) {
      double dError = (*mCNet)[i].MinimumError();
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
  double ControlNetStatistics::MaximumError()
  {
    double dMaxError = 0.0;
    int iNumPoints = mCNet->Size();
    
    for(int i = 0; i < iNumPoints; i++) {
      double dError = (*mCNet)[i].MaximumError();
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
  double ControlNetStatistics::MinimumErrorLine()
  {
    double dMinError = VALID_MAX4;
    int iNumPoints = mCNet->Size();
    
    for(int i = 0; i < iNumPoints; i++) {
       double dError = (*mCNet)[i].MinimumErrorLine();
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
  double ControlNetStatistics::MinimumErrorSample()
  {
    double dMinError = VALID_MAX4;
    int iNumPoints = mCNet->Size();
    
    for(int i = 0; i < iNumPoints; i++) {
       double dError = (*mCNet)[i].MinimumErrorSample();
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
  double ControlNetStatistics::MaximumErrorLine()
  {
    double dMaxError = VALID_MAX4;
    int iNumPoints = mCNet->Size();
    
    for(int i = 0; i < iNumPoints; i++) {
       double dError = (*mCNet)[i].MaximumErrorLine();
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
  double ControlNetStatistics::MaximumErrorSample()
  {
    double dMaxError = 0.0;
    int iNumPoints = mCNet->Size();
    
    for(int i = 0; i < iNumPoints; i++) {
      double dError = (*mCNet)[i].MaximumErrorSample();
      if(dError > dMaxError)
        dMaxError = dError;
    }
    
    return dMaxError;
  }
}

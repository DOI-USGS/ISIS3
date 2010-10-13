#include "Camera.h"
#include "CameraFactory.h"
#include "ControlMeasure.h"
#include "ControlNet.h"
#include "ControlNetFilter.h"
#include "ControlPoint.h"
#include "Filename.h"
#include "iString.h"
#include "PvlGroup.h"
#include "SerialNumberList.h"

using namespace std;
//#define _DEBUG_
#ifdef _DEBUG_
  //debugging
  fstream odb;

  void StartDebug() {
    odb.open("Debug.log", std::ios::out | std::ios::app);
    odb << "\n*************************\n";
  }

  void CloseDebug() {
    odb << "\n*************************\n";
    odb.close();
  }
#endif

#define UNDEFINED_STATUS 2

namespace Isis {
  
  //! String names for Point Type
  extern string sPointType [];

  //! String values for Boolean
  extern string sBoolean[];

  /**
   * ControlNetFilter Constructor
   * 
   * @author Sharmila Prasad (8/27/2010)
   * 
   * @param pCNet  - input Control Net
   * @param psSerialNumFile - Corresponding Serial Num List
   * @param pProgress - Progress of the processing
   */
  ControlNetFilter::ControlNetFilter(ControlNet *pCNet, string & psSerialNumFile, Progress *pProgress) :
                                                            ControlNetStatistics(pCNet, psSerialNumFile, pProgress)
  {
    mSerialNumFilter  = SerialNumberList(psSerialNumFile);    
    
    #ifdef _DEBUG_
    StartDebug();
    /*for (int i=0; i<mCNet->Size(); i++) {
      ControlPoint cp = (*mCNet)[i];
      for (int j=0; j<cp.Size(); j++) {
        ControlMeasure cm = cp[j];
        string sn = cm.CubeSerialNumber();
        odb << "serial num=" << sn << "  File=" << mSerialNumList.Filename(sn) << endl;
      }
    }*/
    #endif
  }
  
  /**
   * Get the output file and the file format (pvl / cvs)
   *  
   * @author Sharmila Prasad (9/7/2010)
   * 
   * @param psPrintFile 
   * @param pbPvl 
   */
  void ControlNetFilter::SetOutputFile(string psPrintFile)
  {
    Isis::Filename outFile(psPrintFile);
    string outName(outFile.Expanded());
    mOstm.open(outName.c_str(), std::ios::out);
  }
  
  /**
   * ControlNetFilter Destructor
   * 
   * @author Sharmila Prasad (8/27/2010)
   */
  ControlNetFilter::~ControlNetFilter() 
  {
    mOstm.close();
  }

  /**
   * Print the Standard Point Stats Header into Output File
   * 
   * @author Sharmila Prasad (8/31/2010)
   */
  void ControlNetFilter::PointStatsHeader(void)
  {
    mOstm << "PointID, Type, Ignore, Held, NumMeasures, NumIgnoredMeasures, ";
  }

  /**
   * Print the Standard Point Stats into Output file given the Control Point
   * 
   * @author Sharmila Prasad (8/31/2010)
   * 
   * @param pcPoint 
   */
  void ControlNetFilter::PointStats(ControlPoint & pcPoint)
  {
    mOstm << pcPoint.Id()   << ", " << sPointType[(int)pcPoint.Type()] << ", " <<  sBoolean[(int)pcPoint.Ignore()] << ", " ;
    mOstm << sBoolean[(int)pcPoint.Held()] << ", " << pcPoint.Size() << ", " << pcPoint.Size()-pcPoint.NumValidMeasures() << ", ";
  }
  
  /**
   * Print Cube's File and Serial Number into the Output File
   * 
   * @author Sharmila Prasad (8/31/2010)
   * 
   * @param pcMeasure - Measure's Cube and Serial #
   */
  void ControlNetFilter::PrintCubeFileSerialNum(ControlMeasure & pcMeasure)
  {
    mOstm << mSerialNumList.Filename(pcMeasure.CubeSerialNumber()) << ", ";
    mOstm << pcMeasure.CubeSerialNumber();
  }
  
  /**
   * Print the Standard Cube Stats Header into Output File
   * 
   * @author Sharmila Prasad (8/31/2010)
   */
  void ControlNetFilter::CubeStatsHeader(void)
  {
    mOstm << "FileName, SerialNum, Total Points, Ignore, Ground, Held, ";
  }

  /**
   * Filters out the Control Network based on Error Criteria. 
   * Group by Points 
   * 
   * @author Sharmila Prasad (8/11/2010)
   * 
   * @param pvlGrp - Pvl Group containing the filter info
   * @param pbLastFilter - Flag to indicate whether this is the last filter to print the stats
   */
  void ControlNetFilter::PointErrorFilter(const PvlGroup & pvlGrp, bool pbLastFilter)
  {
    double dLesser=Isis::ValidMinimum;
    double dGreater=Isis::ValidMaximum;
    bool bLessThan=false, bGreaterThan=false;
    
    if (pvlGrp.HasKeyword("LessThan")) {
      dLesser   = pvlGrp["LessThan"][0];
      bLessThan = true;
    }
    
    if (pvlGrp.HasKeyword("GreaterThan")) {
      dGreater     = pvlGrp["GreaterThan"][0];
      bGreaterThan = true;
    }
    
    if (pbLastFilter) {
      mOstm << "PointID, Type, Ignore, Filename, SerialNum, ErrorMagnitude, MeasureIgnore, Reference" << endl << endl;
    }
    
    int iNumPoints = mCNet->Size();
    for (int i=(iNumPoints-1); i >= 0; i--) {
      ControlPoint cPoint = (*mCNet)[i];
      double dMaxErr = cPoint.MaximumError();
      if (bLessThan && bGreaterThan) {
        if (!(dMaxErr < dLesser && dMaxErr > dGreater)){
          mCNet->Delete(i);
          continue;
        }
      }
      else if (bLessThan) {
        if (!(dMaxErr < dLesser)){
          mCNet->Delete(i);
          continue;
        }
      }
      else if (bGreaterThan) {
        if (!(dMaxErr > dGreater)){
          mCNet->Delete(i);
          continue;
        }
      }      
      // Print into output, if it is the last Filter
      if (pbLastFilter){
        int iNumMeasures = cPoint.Size();
        for (int j=0; j<iNumMeasures; j++) {
          mOstm << cPoint.Id() << ", " << sPointType[cPoint.Type()] << ", " << sBoolean[cPoint.Ignore()] << ", " ;
          PrintCubeFileSerialNum(cPoint[j]);
          mOstm << ", " << cPoint[j].ErrorMagnitude() << ", ";
          mOstm << sBoolean[cPoint[j].Ignore()] << ", ";
          mOstm << sBoolean[cPoint[j].IsReference()] << endl;
        }
      }
    }
  }

  /**
   * Filter PointID based on regular expression 
   * Group by Points 
   * 
   * @author Sharmila Prasad (8/11/2010)
   * 
   * @param pvlGrp - Pvl Group containing the filter info
   * @param pbLastFilter - Flag to indicate whether this is the last filter to print the stats
   */
  void ControlNetFilter::PointIDFilter(const PvlGroup & pvlGrp, bool pbLastFilter)
  {
    vector<string> strTokens;
    iString sPointIDExpr = pvlGrp["Expression"][0];
    iString sSeparator("*");
    
    string strToken = sPointIDExpr.Token(sSeparator);
    while (strToken != "") {
      strTokens.push_back(strToken);
      if (!sPointIDExpr.size()) {
        break;
      }
      strToken = sPointIDExpr.Token(sSeparator);
    }

    int iTokenSize = (int)strTokens.size();
    int iNumPoints = mCNet->Size();
    #ifdef _DEBUG_
    odb << "Net Size="<< iNumPoints << endl;
    #endif
    
    if (pbLastFilter) {
      PointStatsHeader();
      mOstm << endl << endl;
    }
    
    for (int i=(iNumPoints-1); i >= 0; i--){
      ControlPoint cPoint = (*mCNet)[i];
      string sPointID = cPoint.Id();
      int iPosition = 0;      
      for (int j=(iTokenSize-1); j>=0; j--) {
        int iLen = strTokens[j].length();
        if (iLen > 0) {
          size_t found = sPointID.find(strTokens[j], iPosition);
          if (found != string::npos) {
            iPosition = found + iLen;
            // End of the expression
            if (j == (iTokenSize-1)) {
              // Log into the output file
              PointStats(cPoint);
              mOstm << endl;
            }
          }
          else {
            mCNet->Delete(i);
            break;
          }
        }
      }
    }
  }
  
  /**
   * Filters the Control Network based on the user specified number of 
   * measures in a Control Point. Group by Points
   * 
   * @author Sharmila Prasad (8/12/2010)
   * 
   * @param pvlGrp - Pvl Group containing the filter info
   * @param pbLastFilter - Flag to indicate whether this is the last filter to print the stats
   */
  void ControlNetFilter::PointMeasuresFilter(const PvlGroup & pvlGrp, bool pbLastFilter)
  {
    int  iLesser=0, iGreater=0;
    bool bLesserFlag=false, bGreaterFlag=false;
    
    if (pvlGrp.HasKeyword("LessThan")){
      iLesser = pvlGrp["LessThan"][0];
      bLesserFlag = true;
    }
    
    if (pvlGrp.HasKeyword("GreaterThan")){
      iGreater = pvlGrp["GreaterThan"][0];
      bGreaterFlag = true;
    }
    
    if (iLesser < 0  || iGreater < 0) {
      string sErrMsg = "Invalid Deffile - Check Point_NumMeasures Group\n";
      throw Isis::iException::Message(Isis::iException::User, sErrMsg, _FILEINFO_); 
      return;
    }
    
    if (pbLastFilter) {
      PointStatsHeader();
      mOstm << "Filename, SerialNum, MeasureIgnore, Reference" << endl << endl;
    }
    
    int iNumPoints = mCNet->Size();
    
    for (int i=(iNumPoints-1); i>=0; i--) {
      ControlPoint cPoint = (*mCNet)[i];
      int iNumMeasures = cPoint.Size();
      if (bLesserFlag && bGreaterFlag) {
        if (!(iNumMeasures < iLesser && iNumMeasures > iGreater)) {
          mCNet->Delete(i);
          continue;
        }
      }
      if (bLesserFlag && iNumMeasures >= iLesser) {
        mCNet->Delete(i);
        continue;
      }
      if (bGreaterFlag && iNumMeasures <= iGreater) {
        mCNet->Delete(i);
        continue;
      }
      
      if (pbLastFilter) {
        for (int j=0; j<iNumMeasures; j++) {
          PointStats(cPoint);
          PrintCubeFileSerialNum(cPoint[j]);
          mOstm << ", "  << sBoolean[(int)cPoint[j].Ignore()];
          mOstm << ", "  << sBoolean[(int)cPoint[j].IsReference()] << endl;
        }
      }
    }
  }
  
  /**
   * Filter the Control Network based on Ignored, Held, Ground Point Properties
   * Group by Points 
   *  
   * @author Sharmila Prasad (8/12/2010)
   * 
   * @param pvlGrp - Pvl Group containing the filter info
   * @param pbLastFilter - Flag to indicate whether this is the last filter to print the stats 
   */
  void ControlNetFilter::PointPropertiesFilter(const PvlGroup & pvlGrp, bool pbLastFilter)
  {
    bool bIgnoredFlag=false, bHeldFlag=false, bGroundFlag=false;
    string sTemp="";
    
    if (pvlGrp.HasKeyword("Ground")){
      sTemp = pvlGrp["Ground"][0];
      if (sTemp == "true") {
        bGroundFlag = true;
      }
    }
    
    if (pvlGrp.HasKeyword("Ignore")){
      sTemp = pvlGrp["Ignore"][0];
      if (sTemp == "true") {
        bIgnoredFlag = true;
      }
    }
    
    if (pvlGrp.HasKeyword("Held")){
      sTemp = pvlGrp["Held"][0];
      if (sTemp == "true") {
        bHeldFlag = true;
      }
    }
    
    if (pbLastFilter) {
      PointStatsHeader();
      mOstm << endl << endl;
    }
    
    int iNumPoints = mCNet->Size();
    
    for (int i=(iNumPoints-1); i>=0; i--) {
      ControlPoint cPoint = (*mCNet)[i];
      bool bIgnored = cPoint.Ignore();
      bool bHeld    = cPoint.Held();
      bool bGround  = (cPoint.Type()==ControlPoint::Ground ? true : false);
      
      if (bIgnoredFlag && !bIgnored) {
        mCNet->Delete(i);
        continue;
      }
      
      if (bGroundFlag && !bGround) {
        mCNet->Delete(i);
        continue;
      }
      
      if (bHeldFlag && !bHeld) {
        mCNet->Delete(i);
        continue;
      }
      
      // Output the Point Stats
      if (pbLastFilter) {
        PointStats(cPoint);
        mOstm << endl;
      }
    }
  }
  
  /**
   * Filters the Control Network based on Lat,Lon Range 
   * Group by Points 
   * 
   * @author Sharmila Prasad (8/13/2010)
   * 
   * @param pvlGrp - Pvl Group containing the filter info
   * @param pbLastFilter - Flag to indicate whether this is the last filter to print the stats 
   */
  void ControlNetFilter::PointLatLonFilter(const PvlGroup & pvlGrp, bool pbLastFilter)
  {
    double dMinLat=0, dMaxLat=0;
    double dMinLon=0, dMaxLon=0;
    
    if (pvlGrp.HasKeyword("MinLat")){
      dMinLat = pvlGrp["MinLat"][0];
    }
    
    if (pvlGrp.HasKeyword("MaxLat")){
      dMaxLat = pvlGrp["MaxLat"][0];
    }
    
    if (pvlGrp.HasKeyword("MinLon")){
      dMinLon = pvlGrp["MinLon"][0];
    }
    
    if (pvlGrp.HasKeyword("MaxLon")){
      dMaxLon = pvlGrp["MaxLon"][0];
    }
    
    if (dMinLat > dMaxLat || dMinLon > dMaxLon) {
      string sErrMsg = "Invalid Deffile - Check Point_LatLon Group\n";
      throw Isis::iException::Message(Isis::iException::User, sErrMsg, _FILEINFO_); 
      return;
    }
    
    if (pbLastFilter) {
      PointStatsHeader();
      mOstm << "Latitude, Longitude, Radius" << endl << endl;
    }
    
    int iNumPoints = mCNet->Size();
    
    for (int i=(iNumPoints-1); i>=0; i--) {
      ControlPoint & cPoint = (*mCNet)[i];
      double dUnivLat = cPoint.UniversalLatitude();
      double dUnivLon = cPoint.UniversalLongitude();
      double dRadius  = cPoint.Radius();
      
      if (dUnivLat==Isis::Null || dUnivLon == Isis::Null) {
        string sn = cPoint[cPoint.ReferenceIndex()].CubeSerialNumber();
        string filename = mSerialNumList.Filename(sn);
        Pvl pvl(filename);
        
        Camera *camera = CameraFactory::Create(pvl);
        dUnivLat = cPoint.LatitudeByReference(camera);
        dUnivLon = cPoint.LongitudeByReference(camera);
        dRadius  = cPoint.RadiusByReference(camera);
      }
      
      if (!(dUnivLat >= dMinLat && dUnivLat <= dMaxLat) ||
          !(dUnivLon >= dMinLon && dUnivLon <= dMaxLon)) {
        mCNet->Delete(i);
        continue;
      }
      
      if (pbLastFilter) {
        PointStats(cPoint);
        mOstm << dUnivLat << ", " << dUnivLon << ", " << dRadius << endl;
      }
    }
  }
  
  /**
   * Filter points that are within given distance of some other point 
   * Group by Points 
   * 
   * @author Sharmila Prasad (8/13/2010)
   * 
   * @param pvlGrp - Pvl Group containing the filter info
   * @param pbLastFilter - Flag to indicate whether this is the last filter to print the stats 
   */
  void ControlNetFilter::PointDistanceFilter(const PvlGroup & pvlGrp, bool pbLastFilter)
  {
    double dMaxDistance=0;
    string sUnits = "pixels";
    
    if (pvlGrp.HasKeyword("MaxDistance")){
      dMaxDistance = pvlGrp["MaxDistance"][0];
    }
    
    if (pvlGrp.HasKeyword("Units")){
      sUnits = pvlGrp["Units"][0];
    }
    
    if (pbLastFilter) {
      PointStatsHeader();
      mOstm << "Point#Distance >>, " << endl;
    }
    
    bool bMinDistance=false;
    int iNumPoints = mCNet->Size();
    for (int i=(iNumPoints-1); i>=0; i--) {
      ControlPoint cp1 = (*mCNet)[i];
      double dUnivLat1=0, dUnivLon1=0, dRadius1=0;
      Camera *cam1;
      
      double dSample1=Isis::Null, dLine1=Isis::Null;
      
      int iRefIndex1 = cp1.ReferenceIndex();
      
      if (sUnits == "meters") {
        dUnivLat1 = cp1.UniversalLatitude();
        dUnivLon1 = cp1.UniversalLongitude(); 
        dRadius1  = cp1.Radius();
      
        if ((dUnivLat1 == Isis::Null) || (dUnivLon1 == Isis::Null)) {
          string sn1 = cp1[iRefIndex1].CubeSerialNumber();
          string filename1 = mSerialNumList.Filename(sn1);
          Pvl pvl1(filename1);
          cam1 = CameraFactory::Create(pvl1);
          if (cam1->SetImage(cp1[iRefIndex1].Sample(),cp1[iRefIndex1].Line()) ) {
            dRadius1  = cam1->UniversalLatitude();
            dUnivLat1 = cam1->UniversalLongitude();
            dUnivLon1 = cam1->LocalRadius();
          }
        }
      }
      else { // pixels
        dSample1 = cp1[iRefIndex1].Sample();
        dLine1 = cp1[iRefIndex1].Line();
      }
      
      for (int j=(mCNet->Size()-1); j>=0; j--) {
        if (i==j) {
          continue;
        }
        ControlPoint cp2 = (*mCNet)[j];
        double dUnivLat2=0, dUnivLon2=0;
        Camera *cam2;
        double dDist=0;
        
        double dSample2=Isis::Null, dLine2=Isis::Null;
      
        int iRefIndex2 = cp2.ReferenceIndex();
        
        if (sUnits == "meters") {
          dUnivLat2 = cp2.UniversalLatitude();
          dUnivLon2 = cp2.UniversalLongitude();
        
          if ((dUnivLat2 == Isis::Null) || (dUnivLon2 == Isis::Null)) {
            string sn2 = cp2[cp2.ReferenceIndex()].CubeSerialNumber();
            string filename2 = mSerialNumList.Filename(sn2);
            Pvl pvl2(filename2);
            cam2 = CameraFactory::Create(pvl2);
          
            if (cam2->SetImage(cp2[iRefIndex2].Sample(),cp2[iRefIndex2].Line())) {
              dUnivLat2 = cam2->UniversalLatitude();
              dUnivLon2 = cam2->UniversalLongitude();
            }
          }
          
          // Get the distance from the camera class
          dDist = Camera::Distance(dUnivLat1, dUnivLon1, dUnivLat2, dUnivLon2, dRadius1);
        }
        else { // pixels
          dSample2 = cp2[iRefIndex2].Sample();
          dLine2 = cp2[iRefIndex2].Line();
          
          double dDeltaSamp = dSample1 - dSample2;
          double dDeltaLine = dLine1 - dLine2;
          // use the distance formula for cartesian coordinates
          dDist = sqrt((dDeltaSamp * dDeltaSamp) + (dDeltaLine * dDeltaLine));
        }
        
        if (dDist <= dMaxDistance) {
          if (pbLastFilter){
            if (!bMinDistance) {
              PointStats(cp1);
            }
            mOstm << cp2.Id() << "#" << dDist << ", ";
          }
          bMinDistance = true;
        }
        else 
          continue;
      }
      if (!bMinDistance) {
        mCNet->Delete(i);
      }
      if (pbLastFilter && bMinDistance) {
        mOstm << endl;
      }
      bMinDistance = false;
    }
  }
  
  /**
   * Filter the PoibMinDistancents which have measures of specified Measure type
   * Group by Points 
   *  
   * @author Sharmila Prasad (8/13/2010)
   * 
   * @param pvlGrp - Pvl Group containing the filter info
   * @param pbLastFilter - Flag to indicate whether this is the last filter to print the stats 
   */
  void ControlNetFilter::PointMeasurePropertiesFilter(const PvlGroup & pvlGrp, bool pbLastFilter)
  {
    int iIgnoredFlag=UNDEFINED_STATUS;
    string sType = "";
    iString isType;
    
    if (pvlGrp.HasKeyword("Ignore")){
      iIgnoredFlag = 0;
      if (pvlGrp["Ignored"][0]=="true")
        iIgnoredFlag = 1;
    }

    if (pvlGrp.HasKeyword("MeasureType")){
      sType = pvlGrp["MeasureType"][0];
      sType = isType.DownCase(sType);
    }
    
    if (pbLastFilter) {
      PointStatsHeader();
      mOstm << "FileName, SerialNum, MeasureIgnore, MeasureType, Reference" << endl;
    }
    
    int iNumPoints = mCNet->Size();
    for (int i=(iNumPoints-1); i>=0; i--) {
      ControlPoint cPoint = (*mCNet)[i];

      int iNumMeasures = cPoint.Size();
      int iNotMeasureType = 0;
      for (int j=0; j<iNumMeasures; j++) {
        ControlMeasure cMeasure=cPoint[j];
        bool bMeasureIgnored = cMeasure.Ignore();
        bool bMeasureFound = false;
        if (iIgnoredFlag == UNDEFINED_STATUS || bMeasureIgnored == iIgnoredFlag) {
          if (sType == "unmeasured" && cMeasure.Type() == ControlMeasure::Unmeasured) {
            bMeasureFound = true;
          }
          else if (sType == "manual" && cMeasure.Type() == ControlMeasure::Manual) {
            bMeasureFound = true;
          }
          else if (sType == "estimated" && cMeasure.Type() == ControlMeasure::Estimated) {
            bMeasureFound = true;
          }
          else if (sType == "autoregistered" && cMeasure.Type() == ControlMeasure::Unmeasured) {
            bMeasureFound = true;
          }
          else if (sType == "manualvalidated" && cMeasure.Type() == ControlMeasure::ValidatedManual) {
            bMeasureFound = true;
          }
          else if (sType == "autoregvalidated" && cMeasure.Type() == ControlMeasure::ValidatedAutomatic) {
            bMeasureFound = true;
          }
        }
        if (bMeasureFound) {
          if (pbLastFilter) {
            PointStats(cPoint);
            string sn = cMeasure.CubeSerialNumber();
            mOstm << mSerialNumList.Filename(sn) << ", " << sn << ",";
            mOstm << sBoolean[(int)cMeasure.Ignore()] << ", " << cMeasure.PrintableMeasureType() << ", ";
            mOstm << sBoolean[(int)cMeasure.IsReference()] << endl;
          }
        }
        else
          iNotMeasureType++;
      }
      if (iNotMeasureType == iNumMeasures) {
        mCNet->Delete(i);
        continue;
      }
    }
  }
  
  /**
   * Filter the Points based on the Measures Goodness ofFit value 
   * Group by Points 
   * 
   * @author Sharmila Prasad (8/16/2010)
   * 
   * @param pvlGrp - Pvl Group containing the filter info
   * @param pbLastFilter - Flag to indicate whether this is the last filter to print the stats
   */
  void ControlNetFilter::PointGoodnessOfFitFilter(const PvlGroup & pvlGrp, bool pbLastFilter)
  {
    bool bLesserFlag=false, bGreaterFlag=false;
    double dLesserValue=0, dGreaterValue=0;
    
    if (pvlGrp.HasKeyword("LessThan")){
      dLesserValue = (double) pvlGrp["LessThan"][0];
      bLesserFlag = true;
    }
    
    if (pvlGrp.HasKeyword("GreaterThan")){
      dGreaterValue = (double) pvlGrp["GreaterThan"][0];
      bGreaterFlag = true;
    }
    
    if (pbLastFilter) {
      mOstm << "PointID, Type, Ignore, Filename, SerialNum, GoodnessOfFit, MeasureIgnore, Reference" << endl << endl;
    }
    #ifdef _DEBUG_
    odb << "Lessthan=" << bLesserFlag << "  value=" << dLesserValue << endl;
    odb << "GreaterThan=" << bGreaterFlag << "  value=" << dGreaterValue << endl;
    #endif
    
    int iNumPoints = mCNet->Size();
    for (int i=(iNumPoints-1); i>=0; i--) {
      ControlPoint cPoint = (*mCNet)[i];

      int iNumMeasures = cPoint.Size();
      bool bMatchFlag=false;
              
      for (int j=0; j<iNumMeasures; j++) {
        double dGFit = cPoint[j].GoodnessOfFit();
        #ifdef _DEBUG_
        odb << " Point." << i << "  Measure." << j << "   GoodnessFit=" << dGFit << endl;
        #endif
        if (dGFit != Isis::Null) {
          if (bLesserFlag && bGreaterFlag) {
            if (dGFit < dLesserValue && dGFit > dGreaterValue) {
              bMatchFlag = true;
            }
          }
          else if (bLesserFlag && dGFit < dLesserValue) {
            bMatchFlag = true;
          }
          else if (bGreaterFlag && dGFit > dGreaterValue) {
            bMatchFlag = true;
          }
        }
      }
      
      if (!bMatchFlag) {
        mCNet->Delete(i);
      }
      else {
        if (pbLastFilter){
          for (int j=0; j<iNumMeasures; j++) {
            ControlMeasure cMeasure=cPoint[j];
            double dGFit = cMeasure.GoodnessOfFit();
            
            mOstm << cPoint.Id() << ", " << sPointType[cPoint.Type()] << ", " << sBoolean[cPoint.Ignore()] << ", " ;
            PrintCubeFileSerialNum(cMeasure);
            mOstm << ", " << (dGFit==Isis::Null ? "Null" : iString(dGFit)) << ", ";
            mOstm << sBoolean[cMeasure.Ignore()] << ", ";
            mOstm << sBoolean[cMeasure.IsReference()] << endl;
          }
        }
      }
    }
  }


  /**
   * Filter points based on the image serial # - Group by Point 
   * 
   * @author Sharmila Prasad (8/16/2010)
   * 
   * @param pvlGrp - Pvl Group containing the filter info
   * @param pbLastFilter - Flag to indicate whether this is the last filter to print the stats 
   */
  void ControlNetFilter::PointCubeNamesFilter(const PvlGroup & pvlGrp, bool pbLastFilter)
  {
    vector <string> sCubeNames;
    
    // Store the Cubenames from the PvlGroup
    for (int i=0; i<pvlGrp.Keywords(); i++) {
      sCubeNames.push_back(pvlGrp[i][0]);
    }
    
    int size = sCubeNames.size();

    if (pbLastFilter) {
      PointStatsHeader();
      CubeStatsHeader();
      mOstm << endl;
    }
    
    int iNumPoints = mCNet->Size();
    for (int i=(iNumPoints-1); i>=0; i--) {
      ControlPoint cPoint =(*mCNet)[i];
      int iNumMeasures = cPoint.Size();
      int iNumNoMatch=0;
      bool bMatch=false;
      for (int j=0; j<iNumMeasures; j++) {
        ControlMeasure cMeasure = cPoint[j];
        //odb << "Point" << i << ". Measure"  << j << ". Cube =" << cMeasure.CubeSerialNumber() << endl;
        for (int k=0; k<size; k++) {
          if (cMeasure.CubeSerialNumber() == sCubeNames[k] ) {
            bMatch = true;
            break;
          }
        }
        if (!bMatch) {
          iNumNoMatch++;
        }
      }
      if (iNumNoMatch == iNumMeasures) {
        mCNet->Delete(i);
      }
    } //end point loop
    
    // If Last filter print to the output file in the required format
    if (pbLastFilter) {
      GenerateImageStats();
      iNumPoints = mCNet->Size();
      for (int i=0; i<iNumPoints; i++) {
        ControlPoint cPoint =(*mCNet)[i];
        int iNumMeasures = cPoint.Size();
        for (int j=0; j<iNumMeasures; j++) {
          ControlMeasure cMeasure = cPoint[j];

          // Point Details
          mOstm << cPoint.Id() << ", " << sPointType[cPoint.Type()] << ", " << sBoolean[cPoint.Ignore()] << ", ";
          mOstm << iNumMeasures << ", " << iNumMeasures - cPoint.NumValidMeasures() << ", " << sBoolean[cPoint.Held()] << ", ";
          
          // Image Details
          string sn = cMeasure.CubeSerialNumber();
          int iPointDetails[IMAGE_POINT_SIZE], *iPntDetailsPtr = iPointDetails ;
          GetImageStatsBySerialNum(sn, iPntDetailsPtr, IMAGE_POINT_SIZE);
          mOstm << mSerialNumList.Filename(sn) << ", " << sn << ", ";
          mOstm << iPntDetailsPtr[total] << ", " << iPntDetailsPtr[ignore] << ", " ;
          mOstm << iPntDetailsPtr[ground] << ", " << iPntDetailsPtr[held] << endl;
        }
      }
    }
  }
  
  /**
   * Filter Cube names in Control Network by cube name expression
   * Group by Image 
   *  
   * @author Sharmila Prasad (8/16/2010)
   * 
   * @param pvlGrp - Pvl Group containing the filter info
   * @param pbLastFilter - Flag to indicate whether this is the last filter to print the stats 
   */
  void ControlNetFilter::CubeNameExpressionFilter(const PvlGroup & pvlGrp, bool pbLastFilter)
  {
    iString sCubeExpr("");
    if (pvlGrp.HasKeyword("Expression")){
      sCubeExpr = iString(pvlGrp["Expression"][0]);
    }
    
    vector <string> strTokens;
    iString sSeparator("*");
    string strToken = sCubeExpr.Token(sSeparator);
    while (strToken != "") {
      strTokens.push_back(strToken);
      //odb << "Expr=" << sCubeExpr << "   Token=" << strToken << endl;
      if (!sCubeExpr.size()) {
        break;
      }
      strToken = sCubeExpr.Token(sSeparator);
    }
    
    int iTokenSize = (int)strTokens.size();
    int iNumCubes  = mSerialNumFilter.Size();
    
    if (pbLastFilter) {
      CubeStatsHeader();
      mOstm << endl;
    }

    //odb << "Token Size=" << iTokenSize << endl;
    for (int i=(iNumCubes-1); i >= 0;  i--){
      string sCubeName = mSerialNumFilter.Filename(i);
      string sSerialNum= mSerialNumFilter.SerialNumber(i);
      int iPosition = 0;
      for (int j=(iTokenSize-1); j>=0; j--) {
        int iLen = strTokens[j].length();
        if (iLen > 0) {
          size_t found = sSerialNum.find(strTokens[j], iPosition);
          if (found != string::npos) {
            iPosition = found + iLen;
            // End of the expression - Found
            if (j == (iTokenSize-1)) {
              break;
            }
          }
          else
          {
            mSerialNumFilter.Delete(sSerialNum);
            break;
          }
        }
      }
    }
    if (pbLastFilter) {
      GenerateImageStats();
      iNumCubes = mSerialNumFilter.Size();
      for (int i=0; i<iNumCubes; i++) {
        string sn = mSerialNumFilter.SerialNumber(i);
        mOstm << mSerialNumFilter.Filename(i) << ", " << sn << ", ";
        int iPointDetails[IMAGE_POINT_SIZE], *iPntDetailsPtr=iPointDetails;
        GetImageStatsBySerialNum(sn, iPntDetailsPtr, IMAGE_POINT_SIZE);
      
        mOstm << iPntDetailsPtr[total]  << ", " << iPntDetailsPtr[ignore] << ", ";
        mOstm << iPntDetailsPtr[ground] << ", " << iPntDetailsPtr[held]   << endl;
      }
    }
  }
  
  /**
   * Filter the cube by the number of points in each cube
   * Group by Image 
   *  
   * @author Sharmila Prasad (8/16/2010)
   * 
   * @param pvlGrp - Pvl Group containing the filter info
   * @param pbLastFilter - Flag to indicate whether this is the last filter to print the stats 
   */
  void ControlNetFilter::CubeNumPointsFilter(const PvlGroup & pvlGrp, bool pbLastFilter)
  {
    bool bLessFlag=false, bGreaterFlag=false;
    int iLessPoints=0, iGreaterPoints=0;
    if (pvlGrp.HasKeyword("LessThan")){
      iLessPoints = pvlGrp["LessThan"][0];
      bLessFlag = true;
    }
    if (pvlGrp.HasKeyword("GreaterThan")){
      iGreaterPoints = pvlGrp["GreaterThan"][0];
      bGreaterFlag = true;
    }
    
    if (iLessPoints < 0 || iGreaterPoints < 0) {
      string sErrMsg = "Invalid Deffile - Check Cube_NumPoints Group\n";
      throw Isis::iException::Message(Isis::iException::User, sErrMsg, _FILEINFO_); 
      return;
    }
    
    if (pbLastFilter) {
      CubeStatsHeader();
      mOstm << endl;
    }
    int iNumCubes=mSerialNumFilter.Size();
    int iNumPoints = mCNet->Size();
    
    for (int sn=(iNumCubes-1); sn>=0; sn--) {
      int iPointsTotal=0;
      int iPointsIgnored=0;
      int iPointsGround=0;
      int iPointsHeld=0;
      string sSerialNum = mSerialNumFilter.SerialNumber(sn);

      for (int i=0; i<iNumPoints; i++) {
        ControlPoint cPoint = (*mCNet)[i];
        int iNumMeasures = cPoint.Size();
        for (int j=0; j<iNumMeasures; j++) {
          ControlMeasure cMeasure = cPoint[j];
          if (cMeasure.CubeSerialNumber() == sSerialNum) {
            iPointsTotal++;
            if (cPoint.Ignore()) {
              iPointsIgnored++;
            }
            if (cPoint.Type() == ControlPoint::Ground) {
              iPointsGround++;
            }
            if (cPoint.Held()) {
              iPointsHeld++;
            }
            break;
          }
        }
      }
      
      if ((bGreaterFlag && bLessFlag && !(iPointsTotal > iGreaterPoints && iPointsTotal < iLessPoints)) ||
          (bGreaterFlag && !(iPointsTotal > iGreaterPoints)) ||
          (bLessFlag && !(iPointsTotal < iLessPoints)) ) {
        mSerialNumFilter.Delete(sSerialNum);
      }
      else if (pbLastFilter) {
        mOstm << mSerialNumFilter.Filename(sSerialNum) << ", " << sSerialNum << ", ";
        mOstm << iPointsTotal << ", " << iPointsIgnored << ", " << iPointsGround << ", ";
        mOstm << iPointsHeld << endl;
      }
    }
  }
  
  /**
   * Filter by distance between points in Cube
   * Group by Image 
   *  
   * @author Sharmila Prasad (8/17/2010)
   * 
   * @param pvlGrp - Pvl Group containing the filter info
   * @param pbLastFilter - Flag to indicate whether this is the last filter to print the stats 
   */
  void ControlNetFilter::CubeDistanceFilter(const PvlGroup & pvlGrp, bool pbLastFilter)
  {
    double dDistance = 0;
    string sUnits = "pixels";
    
    if (pvlGrp.HasKeyword("MaxDistance")){
      dDistance = pvlGrp["MaxDistance"][0];
    }
    
    if (pvlGrp.HasKeyword("Units")){
      sUnits = pvlGrp["Units"][0];
    }
    
    if (dDistance <= 0) {
      string sErrMsg = "Invalid Deffile - Check Cube_Distance Group\n";
      throw Isis::iException::Message(Isis::iException::User, sErrMsg, _FILEINFO_); 
      return;
    }
    
    if (pbLastFilter) {
      CubeStatsHeader();
      mOstm << "Distance_PointIDs >>, " << endl;
    }
    
    int iNumCubes=mSerialNumFilter.Size();
    for (int sn=(iNumCubes-1); sn>=0; sn--) {
      string sSerialNum = mSerialNumFilter.SerialNumber(sn);
      Pvl pvl(mSerialNumList.Filename(sSerialNum));
      Camera *cam = CameraFactory::Create(pvl);
      double dDist = 0;
      bool bMatchDistance = false;
      
      vector <int> sPointIndex1;
      vector <int> sPointIndex2;
      vector <double> dPointDistance;
      
      // Point stats
      int iPointsTotal   = 0;
      int iPointsIgnored = 0;
      int iPointsGround  = 0;
      int iPointsHeld    = 0;
      
      // Reset the vectors
      sPointIndex1.clear();
      sPointIndex2.clear();
      dPointDistance.clear();
      
      int iNumPoints = mCNet->Size();
      for (int i=0; i<iNumPoints; i++) {
        ControlPoint cPoint1 = (*mCNet)[i];
        ControlMeasure cMeasure1;        
        bool bImageFound = false;
        int iNumMeasures1 = cPoint1.Size();
        for (int j=0; j<iNumMeasures1; j++) {
          cMeasure1   = cPoint1[j];
          if (cMeasure1.CubeSerialNumber() == sSerialNum) {
            iPointsTotal++;
            if (cPoint1.Ignore()) {
              iPointsIgnored++;
            }
            if (cPoint1.Type() == ControlPoint::Ground) {
              iPointsGround++;
            }
            if (cPoint1.Held()) {
              iPointsHeld++;
            }
            bImageFound = true;
            break;
          }
        }
        if (!bImageFound){
          continue;        
        }
        
        //if(cMeasure1.Sample()==0 && cMeasure1.Line()==0) continue;
        
        // if the user chooses distance in meters, create camera to find lat/lon for this measure
        double dRadius=0, dLat1=0, dLon1=0;
        if (sUnits == "meters") {
          // try to set image using sample/line values
          if (cam->SetImage(cMeasure1.Sample(),cMeasure1.Line())) {
            dRadius = cam->LocalRadius();
            dLat1 = cam->UniversalLatitude();
            dLon1 = cam->UniversalLongitude();
          }
          else
            continue;
        }
        
        for (int k=(i+1); k<iNumPoints; k++) {
          ControlPoint cPoint2 = (*mCNet)[k];
          int iNumMeasures2 = cPoint2.Size();
          ControlMeasure cMeasure2;
          bool bImageFound2 = false;
          
          for (int j=0; j<iNumMeasures2; j++) {
            if (cPoint2[j].CubeSerialNumber() == sSerialNum) {
              cMeasure2 = cPoint2[j];
              bImageFound2 = true;
              break;
            }
          }
          if(!bImageFound2 || (cMeasure2.Sample() == 0 && cMeasure2.Line() == 0)) continue;
          
          if (sUnits == "pixels"){
            double dDeltaSamp = cMeasure1.Sample() - cMeasure2.Sample();
            double dDeltaLine = cMeasure1.Line() - cMeasure2.Line();
            // use the distance formula for cartesian coordinates
            dDist = sqrt((dDeltaSamp * dDeltaSamp) + (dDeltaLine * dDeltaLine));
          }
          else { // calculate distance in meters
            double dLat2=0, dLon2=0;
            if (cam->SetImage(cMeasure2.Sample(),cMeasure2.Line())) {
              dLat2 = cam->UniversalLatitude();
              dLon2 = cam->UniversalLongitude();
            }
            else
              continue;
            // Calculate the distance between the two points
            dDist = Camera::Distance(dLat1, dLon1, dLat2, dLon2, dRadius);
          }
          if (!dDist || dDist >= dDistance) {
            continue;
          }
          else {
            bMatchDistance = true;
            sPointIndex1.push_back(i);
            sPointIndex2.push_back(k);
            dPointDistance.push_back(dDist);
            //break;
          }
        }// end Loop Point2
        //if (bMatchDistance) {
        // break;
        //}
      } //end Loop Point1
      if (!bMatchDistance) {
        mSerialNumFilter.Delete(sSerialNum);
      }
      else if (pbLastFilter) {
        mOstm << mSerialNumList.Filename(sSerialNum) << ", " << sSerialNum << ", ";
        mOstm << iPointsTotal << ", " << iPointsIgnored << ", " << iPointsGround << ", " << iPointsHeld << ", ";
        for (int j=0; j<(int)sPointIndex1.size(); j++) {
          iString sPointIDDist(dPointDistance[j]);
          sPointIDDist += "#";
          sPointIDDist += (*mCNet)[sPointIndex1[j]].Id();
          sPointIDDist += "#";
          sPointIDDist += (*mCNet)[sPointIndex2[j]].Id();
          
          mOstm << (string)sPointIDDist << ",";
        }
        mOstm << endl;
      }
      delete (cam);
    } // end cube loop
  }
  
}


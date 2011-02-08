#include "ControlNetFilter.h"

#include "Angle.h"
#include "Camera.h"
#include "CameraFactory.h"
#include "ControlMeasure.h"
#include "ControlNet.h"
#include "ControlPoint.h"
#include "Filename.h"
#include "iString.h"
#include "Latitude.h"
#include "Longitude.h"
#include "PvlGroup.h"
#include "SerialNumberList.h"

using namespace std;

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
  ControlNetFilter::ControlNetFilter(ControlNet *pCNet, string &psSerialNumFile, Progress *pProgress) :
    ControlNetStatistics(pCNet, psSerialNumFile, pProgress) {
    mSerialNumFilter  = SerialNumberList(psSerialNumFile);
  }

  /**
   * Get the output file and open the output file stream
   *
   * @author Sharmila Prasad (9/7/2010)
   *
   * @param psPrintFile
   * @param pbPvl
   */
  void ControlNetFilter::SetOutputFile(string psPrintFile) {
    Isis::Filename outFile(psPrintFile);
    string outName(outFile.Expanded());
    mOstm.open(outName.c_str(), std::ios::out);
  }

  /**
   * ControlNetFilter Destructor
   *
   * @author Sharmila Prasad (8/27/2010)
   */
  ControlNetFilter::~ControlNetFilter() {
    mOstm.close();
  }

  /**
   * Print the Standard Point Stats Header into Output File
   *
   * @author Sharmila Prasad (8/31/2010)
   */
  void ControlNetFilter::PointStatsHeader(void) {
    mOstm << "PointID, Type, Ignore, NumMeasures, NumIgnoredMeasures, ";
  }

  /**
   * Print the Standard Point Stats into Output file given the Control Point
   *
   * @author Sharmila Prasad (8/31/2010)
   *
   * @param pcPoint
   */
  void ControlNetFilter::PointStats(const ControlPoint &pcPoint) {
    mOstm << pcPoint.GetId()   << ", " << sPointType[(int)pcPoint.GetType()]
          << ", " << sBoolean[(int)pcPoint.IsIgnored()] << ", "
          << sBoolean[(int)pcPoint.IsEditLocked()] << ", "
          << pcPoint.GetNumMeasures() << ", "
          << pcPoint.GetNumMeasures() - pcPoint.GetNumValidMeasures() << ", ";
  }

  /**
   * Print Cube's File and Serial Number into the Output File
   *
   * @author Sharmila Prasad (8/31/2010)
   *
   * @param pcMeasure - Measure's Cube and Serial #
   */
  void ControlNetFilter::PrintCubeFileSerialNum(const ControlMeasure &pcMeasure) {
    mOstm << mSerialNumList.Filename(pcMeasure.GetCubeSerialNumber()) << ", ";
    mOstm << pcMeasure.GetCubeSerialNumber();
  }

  /**
   * Print the Standard Cube Stats Header into Output File
   *
   * @author Sharmila Prasad (8/31/2010)
   */
  void ControlNetFilter::CubeStatsHeader(void) {
    mOstm << "FileName, SerialNum, Total Points, Ignore, Ground, ";
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
  void ControlNetFilter::PointErrorFilter(const PvlGroup &pvlGrp, bool pbLastFilter) {
    double dLesser = Isis::ValidMinimum;
    double dGreater = Isis::ValidMaximum;
    bool bLessThan = false, bGreaterThan = false;

    if (pvlGrp.HasKeyword("LessThan")) {
      dLesser   = pvlGrp["LessThan"][0];
      bLessThan = true;
    }

    if (pvlGrp.HasKeyword("GreaterThan")) {
      dGreater     = pvlGrp["GreaterThan"][0];
      bGreaterThan = true;
    }

    if (!bLessThan && !bGreaterThan) {
      return;
    }

    if (pbLastFilter) {
      mOstm << "PointID, Type, Ignore, Filename, SerialNum, ErrorMagnitude, MeasureIgnore, Reference";
      mOstm << endl << endl;
    }

    int iNumPoints = mCNet->GetNumPoints();
    for (int i = (iNumPoints - 1); i >= 0; i--) {
      ControlPoint *cPoint = mCNet->GetPoint(i);
      double dMaxErr = cPoint->GetMaximumResidual();
      if (bLessThan && bGreaterThan) {
        if (!(dMaxErr < dLesser && dMaxErr > dGreater)) {
          mCNet->DeletePoint(i);
          continue;
        }
      }
      else if (bLessThan) {
        if (!(dMaxErr < dLesser)) {
          mCNet->DeletePoint(i);
          continue;
        }
      }
      else if (bGreaterThan) {
        if (!(dMaxErr > dGreater)) {
          mCNet->DeletePoint(i);
          continue;
        }
      }
      // Print into output, if it is the last Filter
      if (pbLastFilter) {
        int iNumMeasures = cPoint->GetNumMeasures();
        for (int j = 0; j < iNumMeasures; j++) {
          mOstm << cPoint->GetId() << ", " << sPointType[cPoint->GetType()]
                << ", " << sBoolean[cPoint->IsIgnored()] << ", ";

          const ControlMeasure *measure = cPoint->GetMeasure(j);
          PrintCubeFileSerialNum(*measure);
          mOstm << ", " << measure->GetResidualMagnitude() << ", "
                << sBoolean[measure->IsIgnored()] << ", "
                << sBoolean[measure->GetType() == ControlMeasure::Reference]
                << endl;
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
  void ControlNetFilter::PointIDFilter(const PvlGroup &pvlGrp, bool pbLastFilter) {
    std::vector<string> strTokens;
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
    int iNumPoints = mCNet->GetNumPoints();
#ifdef _DEBUG_
    odb << "Net Size=" << iNumPoints << endl;
#endif

    if (pbLastFilter) {
      PointStatsHeader();
      mOstm << endl << endl;
    }

    for (int i = (iNumPoints - 1); i >= 0; i--) {
      const ControlPoint *cPoint = mCNet->GetPoint(i);
      string sPointID = cPoint->GetId();
      int iPosition = 0;
      for (int j = (iTokenSize - 1); j >= 0; j--) {
        int iLen = strTokens[j].length();
        if (iLen > 0) {
          size_t found = sPointID.find(strTokens[j], iPosition);
          if (found != string::npos) {
            iPosition = found + iLen;
            // End of the expression
            if (j == (iTokenSize - 1)) {
              // Log into the output file
              PointStats(*cPoint);
              mOstm << endl;
            }
          }
          else {
            mCNet->DeletePoint(i);
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
  void ControlNetFilter::PointMeasuresFilter(const PvlGroup &pvlGrp, bool pbLastFilter) {
    int  iLesser = 0, iGreater = 0;
    bool bLesserFlag = false, bGreaterFlag = false;

    if (pvlGrp.HasKeyword("LessThan")) {
      iLesser = pvlGrp["LessThan"][0];
      bLesserFlag = true;
    }

    if (pvlGrp.HasKeyword("GreaterThan")) {
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

    int iNumPoints = mCNet->GetNumPoints();

    for (int i = (iNumPoints - 1); i >= 0; i--) {
      const ControlPoint *cPoint = mCNet->GetPoint(i);
      int iNumMeasures = cPoint->GetNumMeasures();
      if (bLesserFlag && bGreaterFlag) {
        if (!(iNumMeasures < iLesser && iNumMeasures > iGreater)) {
          mCNet->DeletePoint(i);
          continue;
        }
      }
      if (bLesserFlag && iNumMeasures >= iLesser) {
        mCNet->DeletePoint(i);
        continue;
      }
      if (bGreaterFlag && iNumMeasures <= iGreater) {
        mCNet->DeletePoint(i);
        continue;
      }

      if (pbLastFilter) {
        for (int j = 0; j < iNumMeasures; j++) {
          PointStats(*cPoint);
          PrintCubeFileSerialNum(*cPoint->GetMeasure(j));
          mOstm << ", "  << sBoolean[(int)cPoint[j].IsIgnored()];
          mOstm << ", "  << sBoolean[(int)cPoint[j].GetType() == ControlMeasure::Reference] << endl;
        }
      }
    }
  }

  /**
   * Filter the Control Network based on Ignored, Ground Point Properties
   * Group by Points
   *
   * @author Sharmila Prasad (8/12/2010)
   *
   * @param pvlGrp - Pvl Group containing the filter info
   * @param pbLastFilter - Flag to indicate whether this is the last filter to print the stats
   */
  void ControlNetFilter::PointPropertiesFilter(const PvlGroup &pvlGrp, bool pbLastFilter) {
    bool bIgnoredFlag = false, bGroundFlag = false;
    string sTemp = "";

    if (pvlGrp.HasKeyword("Ground")) {
      sTemp = pvlGrp["Ground"][0];
      if (sTemp == "true") {
        bGroundFlag = true;
      }
    }

    if (pvlGrp.HasKeyword("Ignore")) {
      sTemp = pvlGrp["Ignore"][0];
      if (sTemp == "true") {
        bIgnoredFlag = true;
      }
    }

    if (!bGroundFlag && !bIgnoredFlag) {
      return;
    }

    if (pbLastFilter) {
      PointStatsHeader();
      mOstm << endl << endl;
    }

    int iNumPoints = mCNet->GetNumPoints();

    for (int i = (iNumPoints - 1); i >= 0; i--) {
      const ControlPoint *cPoint = mCNet->GetPoint(i);
      bool bIgnored = cPoint->IsIgnored();
      bool bGround  = (cPoint->GetType() == ControlPoint::Ground ? true : false);

      if (bIgnoredFlag && !bIgnored) {
        mCNet->DeletePoint(i);
        continue;
      }

      if (bGroundFlag && !bGround) {
        mCNet->DeletePoint(i);
        continue;
      }

      // Output the Point Stats
      if (pbLastFilter) {
        PointStats(*cPoint);
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
  void ControlNetFilter::PointLatLonFilter(const PvlGroup &pvlGrp, bool pbLastFilter) {
    double dMinLat = 0, dMaxLat = 0;
    double dMinLon = 0, dMaxLon = 0;

    if (pvlGrp.HasKeyword("MinLat")) {
      dMinLat = pvlGrp["MinLat"][0];
    }

    if (pvlGrp.HasKeyword("MaxLat")) {
      dMaxLat = pvlGrp["MaxLat"][0];
    }

    if (pvlGrp.HasKeyword("MinLon")) {
      dMinLon = pvlGrp["MinLon"][0];
    }

    if (pvlGrp.HasKeyword("MaxLon")) {
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

    int iNumPoints = mCNet->GetNumPoints();

    for (int i = (iNumPoints - 1); i >= 0; i--) {
      const ControlPoint *cPoint = mCNet->GetPoint(i);
      SurfacePoint cPointSurfPt = cPoint->GetSurfacePoint();

      if (!cPointSurfPt.Valid()) {
        const ControlMeasure *cm = cPoint->GetReferenceMeasure();

        string sn = cm->GetCubeSerialNumber();
        string filename = mSerialNumList.Filename(sn);
        Pvl pvl(filename);

        Camera *camera = CameraFactory::Create(pvl);
        if (camera->SetImage(cm->GetSample(), cm->GetLine())) {
          cPointSurfPt.SetSpherical(
            Latitude(camera->UniversalLatitude(), Angle::Degrees),
            Longitude(camera->UniversalLongitude(), Angle::Degrees),
            Distance(camera->LocalRadius()));
        }
      }

      if (!(cPointSurfPt.GetLatitude().GetDegrees() >= dMinLat &&
            cPointSurfPt.GetLatitude().GetDegrees() <= dMaxLat) ||
          !(cPointSurfPt.GetLongitude().GetDegrees() >= dMinLon &&
            cPointSurfPt.GetLongitude().GetDegrees() <= dMaxLon)) {
        mCNet->DeletePoint(i);
        continue;
      }

      if (pbLastFilter) {
        PointStats(*cPoint);
        mOstm << cPointSurfPt.GetLatitude().GetDegrees() << ", " <<
              cPointSurfPt.GetLongitude().GetDegrees() << ", " <<
              cPointSurfPt.GetLocalRadius().GetMeters() << endl;
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
  void ControlNetFilter::PointDistanceFilter(const PvlGroup &pvlGrp, bool pbLastFilter) {
    double dMaxDistance = 0;
    string sUnits = "pixels";

    if (pvlGrp.HasKeyword("MaxDistance")) {
      dMaxDistance = pvlGrp["MaxDistance"][0];
    }

    if (pvlGrp.HasKeyword("Units")) {
      sUnits = pvlGrp["Units"][0];
    }

    if (pbLastFilter) {
      PointStatsHeader();
      mOstm << "Point#Distance >>, " << endl;
    }

    bool bMinDistance = false;
    int iNumPoints = mCNet->GetNumPoints();
    for (int i = (iNumPoints - 1); i >= 0; i--) {
      const ControlPoint *cp1 = mCNet->GetPoint(i);
      const ControlMeasure *cp1RefMeasure = cp1->GetReferenceMeasure();
      SurfacePoint surfacePt1;
      Camera *cam1;

      double dSample1 = Isis::Null, dLine1 = Isis::Null;

      if (sUnits == "meters") {
        surfacePt1 = cp1->GetSurfacePoint();

        if (!surfacePt1.Valid()) {
          string sn1 = cp1RefMeasure->GetCubeSerialNumber();
          string filename1 = mSerialNumList.Filename(sn1);
          Pvl pvl1(filename1);
          cam1 = CameraFactory::Create(pvl1);
          if (cam1->SetImage(cp1RefMeasure->GetSample(),
                             cp1RefMeasure->GetLine())) {
            surfacePt1.SetSpherical(
              Latitude(cam1->UniversalLatitude(), Angle::Degrees),
              Longitude(cam1->UniversalLongitude(), Angle::Degrees),
              Distance(cam1->LocalRadius())
            );
          }
        }
      }
      else
        // pixels
      {
        dSample1 = cp1RefMeasure->GetSample();
        dLine1 = cp1RefMeasure->GetLine();
      }

      for (int j = (mCNet->GetNumPoints() - 1); j >= 0; j--) {
        if (i == j) {
          continue;
        }
        const ControlPoint *cp2 = mCNet->GetPoint(j);
        const ControlMeasure *cp2RefMeasure = cp2->GetReferenceMeasure();

        SurfacePoint surfacePt2;
        Camera *cam2;
        double dDist = 0;

        double dSample2 = Isis::Null, dLine2 = Isis::Null;

        if (sUnits == "meters") {
          surfacePt2 = cp2->GetSurfacePoint();

          if (!surfacePt2.Valid()) {
            string sn2 = cp2RefMeasure->GetCubeSerialNumber();
            string filename2 = mSerialNumList.Filename(sn2);
            Pvl pvl2(filename2);
            cam2 = CameraFactory::Create(pvl2);

            if (cam2->SetImage(cp2RefMeasure->GetSample(),
                               cp2RefMeasure->GetLine())) {
              surfacePt2.SetSpherical(
                Latitude(cam2->UniversalLatitude(), Angle::Degrees),
                Longitude(cam2->UniversalLongitude(), Angle::Degrees),
                Distance(cam2->LocalRadius())
              );
            }
          }

          // Get the distance from the camera class
          dDist = Camera::Distance(
                    surfacePt1.GetLatitude().GetDegrees(),
                    surfacePt1.GetLongitude().GetDegrees(),
                    surfacePt2.GetLatitude().GetDegrees(),
                    surfacePt2.GetLongitude().GetDegrees(),
                    surfacePt1.GetLocalRadius()
                  );
        }
        else
          // pixels
        {
          dSample2 = cp2RefMeasure->GetSample();
          dLine2 = cp2RefMeasure->GetLine();

          double dDeltaSamp = dSample1 - dSample2;
          double dDeltaLine = dLine1 - dLine2;
          // use the distance formula for cartesian coordinates
          dDist = sqrt((dDeltaSamp * dDeltaSamp) + (dDeltaLine * dDeltaLine));
        }

        if (dDist <= dMaxDistance) {
          if (pbLastFilter) {
            if (!bMinDistance) {
              PointStats(*cp1);
            }
            mOstm << cp2->GetId() << "#" << dDist << ", ";
          }
          bMinDistance = true;
        }
        else
          continue;
      }
      if (!bMinDistance) {
        mCNet->DeletePoint(i);
      }
      if (pbLastFilter && bMinDistance) {
        mOstm << endl;
      }
      bMinDistance = false;
    }
  }

  /**
   * Filter the Points which have measures of specified Measure type and Ignored Flag.
   * Group by Points
   *
   * @author Sharmila Prasad (8/13/2010)
   *
   * @param pvlGrp - Pvl Group containing the filter info
   * @param pbLastFilter - Flag to indicate whether this is the last filter to print the stats
   */
  void ControlNetFilter::PointMeasurePropertiesFilter(const PvlGroup &pvlGrp, bool pbLastFilter) {
    int iIgnoredFlag = -1;
    string sType = "";
    iString isType;

    if (pvlGrp.HasKeyword("Ignore")) {
      iIgnoredFlag = 0;
      if (pvlGrp["Ignore"][0] == "true")
        iIgnoredFlag = 1;
    }

    if (pvlGrp.HasKeyword("MeasureType")) {
      sType = pvlGrp["MeasureType"][0];
      sType = isType.DownCase(sType);
    }

    if (pbLastFilter) {
      PointStatsHeader();
      mOstm << "FileName, SerialNum, MeasureIgnore, MeasureType" << endl;
    }

    int iNumPoints = mCNet->GetNumPoints();
    for (int i = (iNumPoints - 1); i >= 0; i--) {
      const ControlPoint *cPoint = mCNet->GetPoint(i);

      int iNumMeasures = cPoint->GetNumMeasures();
      int iNotMeasureType = 0;
      for (int j = 0; j < iNumMeasures; j++) {
        const ControlMeasure *cMeasure = cPoint->GetMeasure(j);
        bool bMeasureIgnored = cMeasure->IsIgnored();
        bool bMeasureFound = false;

        if (iIgnoredFlag == -1 || bMeasureIgnored == iIgnoredFlag) {
          if (sType == "all") {
            bMeasureFound = true;
          }
          else if (sType == "candidate" && cMeasure->GetType() == ControlMeasure::Candidate) {
            bMeasureFound = true;
          }
          else if (sType == "manual" && cMeasure->GetType() == ControlMeasure::Manual) {
            bMeasureFound = true;
          }
          else if (sType == "registeredpixel" && cMeasure->GetType() == ControlMeasure::RegisteredPixel) {
            bMeasureFound = true;
          }
          else if (sType == "registeredsubpixel" && cMeasure->GetType() == ControlMeasure::RegisteredSubPixel) {
            bMeasureFound = true;
          }
        }
        if (bMeasureFound) {
          if (pbLastFilter) {
            PointStats(*cPoint);
            string sn = cMeasure->GetCubeSerialNumber();
            mOstm << mSerialNumList.Filename(sn) << ", " << sn << ","
                  << sBoolean[(int) cMeasure->IsIgnored()] << ", "
                  << cMeasure->GetMeasureTypeString() << ", "
                  << sBoolean[cMeasure->GetType() == ControlMeasure::Reference]
                  << endl;
          }
        }
        else
          iNotMeasureType++;
      }
      if (iNotMeasureType == iNumMeasures) {
        mCNet->DeletePoint(i);
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
  /*void ControlNetFilter::PointGoodnessOfFitFilter(const PvlGroup & pvlGrp, bool pbLastFilter)
  {
    bool bLesserFlag=false, bGreaterFlag=false;
    double dLesserValue=0, dGreaterValue=0;

    if (pvlGrp.HasKeyword("LessThan")){
      dLesserValue = (int) pvlGrp["LessThan"][0];
      bLesserFlag = true;
    }

    if (pvlGrp.HasKeyword("GreaterThan")){
      dGreaterValue = (int) pvlGrp["GreaterThan"][0];
      bGreaterFlag = true;
    }

    if (!bLesserFlag && !bGreaterFlag) {
      return;
    }

    if (pbLastFilter) {
      mOstm << "PointID, Type, Ignore, Filename, SerialNum, GoodnessOfFit, MeasureIgnore, Reference";
      mOstm << endl << endl;
    }
    #ifdef _DEBUG_
    odb << "Lessthan=" << bLesserFlag << "  value=" << dLesserValue << endl;
    odb << "GreaterThan=" << bGreaterFlag << "  value=" << dGreaterValue << endl;
    #endif

    int iNumPoints = mCNet->GetNumPoints();
    for (int i=(iNumPoints-1); i>=0; i--) {
      ControlPoint cPoint = mCNet->GetPoint(i);

      int iNumMeasures = cPoint->GetNumMeasures();
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
        mCNet->DeletePoint(i);
      }
      else {
        if (pbLastFilter){
          for (int j=0; j<iNumMeasures; j++) {
            ControlMeasure cMeasure=cPoint[j];
            double dGFit = cMeasure.GoodnessOfFit();

            mOstm << cPoint.Id() << ", " << sPointType[cPoint.Type()] << ", ";
            mOstm << sBoolean[cPoint.Ignore()] << ", " ;
            PrintCubeFileSerialNum(cMeasure);
            mOstm << ", " << (dGFit==Isis::Null ? "Null" : iString(dGFit)) << ", ";
            mOstm << sBoolean[cMeasure.Ignore()] << ", ";
            mOstm << sBoolean[(int)cMeasure.Type() == ControlMeasure::Reference] << endl;
          }
        }
      }
    }
  }*/


  /**
   * Filter points based on the image serial # - Group by Point
   *
   * @author Sharmila Prasad (8/16/2010)
   *
   * @param pvlGrp - Pvl Group containing the filter info
   * @param pbLastFilter - Flag to indicate whether this is the last filter to print the stats
   */
  void ControlNetFilter::PointCubeNamesFilter(const PvlGroup &pvlGrp, bool pbLastFilter) {
    std::vector <string> sCubeNames;

    // Store the Cubenames from the PvlGroup
    for (int i = 0; i < pvlGrp.Keywords(); i++) {
      sCubeNames.push_back(pvlGrp[i][0]);
    }

    int size = sCubeNames.size();

    if (pbLastFilter) {
      PointStatsHeader();
      CubeStatsHeader();
      mOstm << endl;
    }

    int iNumPoints = mCNet->GetNumPoints();
    for (int i = (iNumPoints - 1); i >= 0; i--) {
      const ControlPoint *cPoint = mCNet->GetPoint(i);
      int iNumMeasures = cPoint->GetNumMeasures();
      int iNumNoMatch = 0;
      bool bMatch = false;
      for (int j = 0; j < iNumMeasures; j++) {
        const ControlMeasure *cMeasure = cPoint->GetMeasure(j);
        //odb << "Point" << i << ". Measure"  << j << ". Cube =" << cMeasure.CubeSerialNumber() << endl;
        for (int k = 0; k < size; k++) {
          if (cMeasure->GetCubeSerialNumber() == sCubeNames[k]) {
            bMatch = true;
            break;
          }
        }
        if (!bMatch) {
          iNumNoMatch++;
        }
      }
      if (iNumNoMatch == iNumMeasures) {
        mCNet->DeletePoint(i);
      }
    } //end point loop

    // If Last filter print to the output file in the required format
    if (pbLastFilter) {
      GenerateImageStats();
      iNumPoints = mCNet->GetNumPoints();
      for (int i = 0; i < iNumPoints; i++) {
        const ControlPoint *cPoint = mCNet->GetPoint(i);
        int iNumMeasures = cPoint->GetNumMeasures();
        for (int j = 0; j < iNumMeasures; j++) {
          const ControlMeasure *cMeasure = cPoint->GetMeasure(j);

          // Point Details
          mOstm << cPoint->GetId() << ", " << sPointType[cPoint->GetType()]
                << ", " << sBoolean[cPoint->IsIgnored()] << ", "
                << iNumMeasures << ", "
                << iNumMeasures - cPoint->GetNumValidMeasures() << ", "
                << sBoolean[cPoint->IsEditLocked()] << ", ";

          // Image Details
          string sn = cMeasure->GetCubeSerialNumber();
          int iPointDetails[IMAGE_POINT_SIZE], *iPntDetailsPtr = iPointDetails;
          GetImageStatsBySerialNum(sn, iPntDetailsPtr, IMAGE_POINT_SIZE);
          mOstm << mSerialNumList.Filename(sn) << ", " << sn << ", "
                << iPntDetailsPtr[total] << ", " << iPntDetailsPtr[ignore]
                << ", " << iPntDetailsPtr[ground] << endl;
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
  void ControlNetFilter::CubeNameExpressionFilter(const PvlGroup &pvlGrp, bool pbLastFilter) {
    iString sCubeExpr("");
    if (pvlGrp.HasKeyword("Expression")) {
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
    for (int i = (iNumCubes - 1); i >= 0;  i--) {
      string sCubeName = mSerialNumFilter.Filename(i);
      string sSerialNum = mSerialNumFilter.SerialNumber(i);
      int iPosition = 0;
      for (int j = (iTokenSize - 1); j >= 0; j--) {
        int iLen = strTokens[j].length();
        if (iLen > 0) {
          size_t found = sSerialNum.find(strTokens[j], iPosition);
          if (found != string::npos) {
            iPosition = found + iLen;
            // End of the expression - Found
            if (j == (iTokenSize - 1)) {
              break;
            }
          }
          else {
            mSerialNumFilter.Delete(sSerialNum);
            break;
          }
        }
      }
    }
    if (pbLastFilter) {
      GenerateImageStats();
      iNumCubes = mSerialNumFilter.Size();
      for (int i = 0; i < iNumCubes; i++) {
        string sn = mSerialNumFilter.SerialNumber(i);
        mOstm << mSerialNumFilter.Filename(i) << ", " << sn << ", ";
        int iPointDetails[IMAGE_POINT_SIZE], *iPntDetailsPtr = iPointDetails;
        GetImageStatsBySerialNum(sn, iPntDetailsPtr, IMAGE_POINT_SIZE);

        mOstm << iPntDetailsPtr[total]  << ", " << iPntDetailsPtr[ignore] << ", ";
        mOstm << iPntDetailsPtr[ground] << endl;
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
  void ControlNetFilter::CubeNumPointsFilter(const PvlGroup &pvlGrp, bool pbLastFilter) {
    bool bLessFlag = false, bGreaterFlag = false;
    int iLessPoints = 0, iGreaterPoints = 0;
    if (pvlGrp.HasKeyword("LessThan")) {
      iLessPoints = pvlGrp["LessThan"][0];
      bLessFlag = true;
    }
    if (pvlGrp.HasKeyword("GreaterThan")) {
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
    int iNumCubes = mSerialNumFilter.Size();
    int iNumPoints = mCNet->GetNumPoints();

    for (int sn = (iNumCubes - 1); sn >= 0; sn--) {
      int iPointsTotal = 0;
      int iPointsIgnored = 0;
      int iPointsGround = 0;
      int iPointsLocked = 0;
      string sSerialNum = mSerialNumFilter.SerialNumber(sn);

      for (int i = 0; i < iNumPoints; i++) {
        const ControlPoint *cPoint = mCNet->GetPoint(i);
        int iNumMeasures = cPoint->GetNumMeasures();
        for (int j = 0; j < iNumMeasures; j++) {
          const ControlMeasure *cMeasure = cPoint->GetMeasure(j);
          if (cMeasure->GetCubeSerialNumber() == sSerialNum) {
            iPointsTotal++;
            if (cPoint->IsIgnored()) {
              iPointsIgnored++;
            }
            if (cPoint->GetType() == ControlPoint::Ground) {
              iPointsGround++;
            }
            if (cPoint->IsEditLocked()) {
              iPointsLocked++;
            }
            break;
          }
        }
      }

      if ((bGreaterFlag && bLessFlag && !(iPointsTotal > iGreaterPoints && iPointsTotal < iLessPoints)) ||
          (bGreaterFlag && !(iPointsTotal > iGreaterPoints)) ||
          (bLessFlag && !(iPointsTotal < iLessPoints))) {
        mSerialNumFilter.Delete(sSerialNum);
      }
      else if (pbLastFilter) {
        mOstm << mSerialNumFilter.Filename(sSerialNum) << ", " << sSerialNum << ", ";
        mOstm << iPointsTotal << ", " << iPointsIgnored << ", " << iPointsGround << endl;
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
  void ControlNetFilter::CubeDistanceFilter(const PvlGroup &pvlGrp, bool pbLastFilter) {
    double dDistance = 0;
    string sUnits = "pixels";

    if (pvlGrp.HasKeyword("MaxDistance")) {
      dDistance = pvlGrp["MaxDistance"][0];
    }

    if (pvlGrp.HasKeyword("Units")) {
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

    int iNumCubes = mSerialNumFilter.Size();
    for (int sn = (iNumCubes - 1); sn >= 0; sn--) {
      string sSerialNum = mSerialNumFilter.SerialNumber(sn);
      Pvl pvl(mSerialNumList.Filename(sSerialNum));
      Camera *cam = CameraFactory::Create(pvl);
      double dDist = 0;
      bool bMatchDistance = false;

      std::vector <int> sPointIndex1;
      std::vector <int> sPointIndex2;
      std::vector <double> dPointDistance;

      // Point stats
      int iPointsTotal   = 0;
      int iPointsIgnored = 0;
      int iPointsGround  = 0;
      int iPointsLocked  = 0;

      // Reset the vectors
      sPointIndex1.clear();
      sPointIndex2.clear();
      dPointDistance.clear();

      int iNumPoints = mCNet->GetNumPoints();
      for (int i = 0; i < iNumPoints; i++) {
        const ControlPoint *cPoint1 = mCNet->GetPoint(i);
        const ControlMeasure *cMeasure1;
        bool bImageFound = false;
        int iNumMeasures1 = cPoint1->GetNumMeasures();
        for (int j = 0; j < iNumMeasures1; j++) {
          cMeasure1 = cPoint1->GetMeasure(j);
          if (cMeasure1->GetCubeSerialNumber() == sSerialNum) {
            iPointsTotal++;
            if (cPoint1->IsIgnored()) {
              iPointsIgnored++;
            }
            if (cPoint1->GetType() == ControlPoint::Ground) {
              iPointsGround++;
            }
            if (cPoint1->IsEditLocked()) {
              iPointsLocked++;
            }
            bImageFound = true;
            break;
          }
        }
        if (!bImageFound) {
          continue;
        }

        //if(cMeasure1.Sample()==0 && cMeasure1.Line()==0) continue;

        // if the user chooses distance in meters, create camera to find lat/lon for this measure
        double dRadius = 0, dLat1 = 0, dLon1 = 0;
        if (sUnits == "meters") {
          // try to set image using sample/line values
          if (cam->SetImage(cMeasure1->GetSample(), cMeasure1->GetLine())) {
            dRadius = cam->LocalRadius();
            dLat1 = cam->UniversalLatitude();
            dLon1 = cam->UniversalLongitude();
          }
          else
            continue;
        }

        for (int k = (i + 1); k < iNumPoints; k++) {
          const ControlPoint *cPoint2 = mCNet->GetPoint(k);
          int iNumMeasures2 = cPoint2->GetNumMeasures();
          const ControlMeasure *cMeasure2;
          bool bImageFound2 = false;

          for (int j = 0; j < iNumMeasures2; j++) {
            if (cPoint2->GetMeasure(j)->GetCubeSerialNumber() == sSerialNum) {
              cMeasure2 = cPoint2->GetMeasure(j);
              bImageFound2 = true;
              break;
            }
          }
          if (!bImageFound2 ||
              (cMeasure2->GetSample() == 0 && cMeasure2->GetLine() == 0))
            continue;

          if (sUnits == "pixels") {
            double dDeltaSamp = cMeasure1->GetSample() - cMeasure2->GetSample();
            double dDeltaLine = cMeasure1->GetLine() - cMeasure2->GetLine();
            // use the distance formula for cartesian coordinates
            dDist = sqrt((dDeltaSamp * dDeltaSamp) + (dDeltaLine * dDeltaLine));
          }
          else
            // calculate distance in meters
          {
            double dLat2 = 0, dLon2 = 0;
            if (cam->SetImage(cMeasure2->GetSample(), cMeasure2->GetLine())) {
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
        mOstm << iPointsTotal << ", " << iPointsIgnored << ", " << iPointsGround << ", ";
        mOstm << iPointsLocked << ", ";
        for (int j = 0; j < (int)sPointIndex1.size(); j++) {
          iString sPointIDDist(dPointDistance[j]);
          sPointIDDist += "#";
          sPointIDDist += (*mCNet)[sPointIndex1[j]]->GetId();
          sPointIDDist += "#";
          sPointIDDist += (*mCNet)[sPointIndex2[j]]->GetId();

          mOstm << (string)sPointIDDist << ",";
        }
        mOstm << endl;
      }
      delete(cam);
    } // end cube loop
  }

}


#include "ControlNet.h"
#include "PvlGroup.h"
#include "Camera.h"
#include "Portal.h"
#include "SpecialPixel.h"
#include "Pvl.h"
#include "SerialNumberList.h"

#include "CnetRefByResolution.h"

using namespace std;

namespace Isis {
  /**
   * CnetRefByResolution constructor with the def file
   *
   * @author Sharmila Prasad (5/25/2010)
   *
   * @param pPvlDef         - Pvl Definition File
   * @param psSerialNumfile - Serial Number file attached to the ControlNet
   * @param peType          - Resolution Type
   * @param pdResValue      - Resolution value 
   * @param pdMinRes        - Min Resolution value 
   * @param pdMaxRes        - Max Resolution value 
   */
  CnetRefByResolution::CnetRefByResolution(Pvl *pPvlDef, std::string psSerialNumfile, 
    ResolutionType peType, double pdResValue, double pdMinRes, double pdMaxRes)
    :ControlNetValidMeasure(pPvlDef) {
    mdResValue = pdResValue;
    mdMinRes   = pdMinRes;
    mdMaxRes   = pdMaxRes;
    meType     = peType;
    ReadSerialNumbers(psSerialNumfile);
  }

  /**
   * FindCnetRef traverses all the control points and measures in the network and checks for 
   * valid Measure which passes the Emission Incidence Angle, DN value tests and chooses the 
   * measure with the best Resolution criteria as the reference. Creates a new control network 
   * with these adjustments.
   *
   * @author Sharmila Prasad (5/25/2010)
   *
   * @param pNewNet   - Modified output Control Net 
   *  
   * @history 2010-10-14 Sharmila Prasad - Use single copy of Control Net in FindCnetRef() 
   */
  void CnetRefByResolution::FindCnetRef(ControlNet &pNewNet) {
    // Process each existing control point in the network
    int iTotalMeasures = 0;
    int iPointsModified = 0;
    int iMeasuresModified = 0;
    int iRefChanged = 0;

    //Status Report
    mStatus.SetText("Choosing Reference by Resolution...");
    mStatus.SetMaximumSteps(pNewNet.Size());
    mStatus.CheckStatus();

    //mPvlLog += GetStdOptions();
    for(int point = 0; point < pNewNet.Size(); ++point) {
      ControlPoint & newPnt = ((ControlNet &)pNewNet)[point]; 
      
      // Save a copy of the Original
      ControlPoint origPnt(newPnt);
      
      mdResVector.clear();

      // Stats and Accounting
      iTotalMeasures += origPnt.Size();

      // Logging
      PvlObject pvlPointObj("PointDetails");
      pvlPointObj += Isis::PvlKeyword("PointId", newPnt.Id());

      int iRefIndex = newPnt.ReferenceIndexNoException();
      iString istrTemp;

      std::vector <PvlGroup> pvlGrpVector;
      int iBestIndex = 0;

      // Only perform the interest operation on points of type "Tie" and
      // Points having atleast 1 measure and Point is not Ignored
      if(!newPnt.Ignore() && newPnt.Type() == ControlPoint::Tie && iRefIndex >= 0) {
        // Create a measurment for each image in this point using the reference
        // lat/lon.
        int iNumIgnore = 0;
        iString istrTemp;
        //double dBestResolution=135;

        for(int measure = 0; measure < origPnt.Size(); ++measure) {

          ControlMeasure & newMsr = newPnt[measure];
          newMsr.SetDateTime();
          newMsr.SetChooserName("Application cnetref(Resolution)");

          double dSample = newMsr.Sample();
          double dLine   = newMsr.Line();
          std::string sn = newMsr.CubeSerialNumber();

          // Log
          PvlGroup pvlMeasureGrp("MeasureDetails");
          pvlMeasureGrp += Isis::PvlKeyword("SerialNum", sn);
          pvlMeasureGrp += Isis::PvlKeyword("OriginalLocation", LocationString(dSample, dLine));

          if(!newMsr.Ignore()) {
            Cube *measureCube = mCubeMgr.OpenCube(mSerialNumbers.Filename(sn));

            newMsr.SetReference(false);
            newMsr.SetIgnore(false);

            if(!ValidStandardOptions(dSample, dLine, measureCube, &pvlMeasureGrp)) {
              pvlMeasureGrp += Isis::PvlKeyword("Ignored", "Failed Emission, Incidence, Resolution and/or Dn Value Test");
              newMsr.SetIgnore(true);
              iNumIgnore++;
            }
            mdResVector.push_back(mdResolution);
          } // Ignore == false
          else {
            pvlMeasureGrp += Isis::PvlKeyword("Ignored", "Originally Ignored");
            iNumIgnore++;
          }
          
          if(newMsr != origPnt[measure]) {
            iMeasuresModified++;
          }
          pvlGrpVector.push_back(pvlMeasureGrp);
        }// end Measure

        if((newPnt.Size() - iNumIgnore) < 2) {
          newPnt.SetIgnore(true);
          pvlPointObj += Isis::PvlKeyword("Ignored", "Good Measures less than 2");
        }
        // Set the Reference
        if(!newPnt.Ignore()) {
          iBestIndex = GetReferenceByResolution(newPnt);
          if(iBestIndex >= 0 && !newPnt[iBestIndex].Ignore()) {
            newPnt[iBestIndex].SetReference(true);
            pvlGrpVector[iBestIndex] += Isis::PvlKeyword("Reference", "true");
          }
          else {
            if(iBestIndex < 0 && meType == Range) {
              pvlPointObj += Isis::PvlKeyword("NOTE", "No Valid Measures within the Resolution Range. Reference defaulted to the first Measure");
            }
            iBestIndex = 0;
            newPnt[iBestIndex].SetReference(true);
          }
        }

        for(int i = 0; i < newPnt.Size(); i++) {
          pvlPointObj += pvlGrpVector[i];
        }
      } // end Tie
      else {
        if(iRefIndex < 0) {
          pvlPointObj += Isis::PvlKeyword("Comments", "No Measures in the Point");
        }
        else if(newPnt.Ignore()) {
          pvlPointObj += Isis::PvlKeyword("Comments", "Point was originally Ignored");
        }
        else {
          pvlPointObj += Isis::PvlKeyword("Comments", "Not Tie Point");
        }
        for(int measure = 0; measure < newPnt.Size(); measure++) {
          newPnt[measure].SetDateTime();
          newPnt[measure].SetChooserName("Application cnetref(Resolution)");
        }
      }

      if(newPnt != origPnt) {
        iPointsModified++;
      }

      if(!newPnt.Ignore() && iBestIndex != iRefIndex) {
        iRefChanged++;
        PvlGroup pvlRefChangeGrp("ReferenceChangeDetails");
        pvlRefChangeGrp += Isis::PvlKeyword("PrevSerialNumber", origPnt[iRefIndex].CubeSerialNumber());
        pvlRefChangeGrp += Isis::PvlKeyword("PrevResolution",   mdResVector[iRefIndex]);

        istrTemp = iString((int)origPnt[iRefIndex].Sample());
        istrTemp += ",";
        istrTemp += iString((int)origPnt[iRefIndex].Line());
        pvlRefChangeGrp += Isis::PvlKeyword("PrevLocation",     istrTemp);

        pvlRefChangeGrp += Isis::PvlKeyword("NewSerialNumber",  newPnt[iBestIndex].CubeSerialNumber());
        std::string sKeyName = "NewHighestResolution";
        if(meType == Low) {
          sKeyName = "NewLeastResolution";
        }
        else if(meType == Mean) {
          pvlRefChangeGrp += Isis::PvlKeyword("MeanResolution",  GetMeanResolution());
          sKeyName = "NewResolutionNeartoMean";
        }
        else if(meType == Nearest) {
          sKeyName = "NewResolutionNeartoValue";
        }
        else if(meType == Range) {
          sKeyName = "NewResolutionInRange";
        }
        pvlRefChangeGrp += Isis::PvlKeyword(sKeyName,  mdResVector[iBestIndex]);

        istrTemp = iString((int)newPnt[iBestIndex].Sample());
        istrTemp += ",";
        istrTemp += iString((int)newPnt[iBestIndex].Line());
        pvlRefChangeGrp += Isis::PvlKeyword("NewLocation",      istrTemp);

        pvlPointObj += pvlRefChangeGrp;
      }
      else {
        pvlPointObj += Isis::PvlKeyword("Reference", "No Change");
      }

      mPvlLog += pvlPointObj;
      mStatus.CheckStatus();
    }// end Point

    // Basic Statistics
    mStatisticsGrp += Isis::PvlKeyword("TotalPoints",      pNewNet.Size());
    mStatisticsGrp += Isis::PvlKeyword("PointsIgnored",    (pNewNet.Size() - pNewNet.NumValidPoints()));
    mStatisticsGrp += Isis::PvlKeyword("PointsModified",   iPointsModified);
    mStatisticsGrp += Isis::PvlKeyword("ReferenceChanged", iRefChanged);
    mStatisticsGrp += Isis::PvlKeyword("TotalMeasures",    iTotalMeasures);
    mStatisticsGrp += Isis::PvlKeyword("MeasuresModified", iMeasuresModified);

    mPvlLog += mStatisticsGrp;
  }

  /**
   * GetMeanResolution - Get Mean of all the resolutions of all the measures
   * in the Control Point
   *
   * @author Sharmila Prasad (6/1/2010)
   *
   * @return double
   */
  double CnetRefByResolution::GetMeanResolution(void) {
    double dTotal = 0;
    for(int i = 0; i < (int)mdResVector.size(); i++) {
      dTotal += mdResVector[i];
    }

    return (dTotal / mdResVector.size());
  }

  /**
   * GetReferenceByResolution - Get the Reference for each Control Point by user defined
   * Resolution criteria
   *
   * @author Sharmila Prasad (6/1/2010)
   *
   * @return int
   */
  int CnetRefByResolution::GetReferenceByResolution(ControlPoint &pNewPoint) {
    int iBestIndex = -1;
    double dBestResolution = -1;
    double dMean = 0;
    if(meType == Mean) {
      dMean = GetMeanResolution();
    }

    for(int i = 0; i < (int)mdResVector.size(); i++) {
      if(pNewPoint[i].Ignore()) {
        continue;
      }
      else {
        double dDiff = dBestResolution - mdResVector[i];
        if(meType == Low) {
          if(dBestResolution == -1 || dDiff < 0) {
            dBestResolution = mdResVector[i];
            iBestIndex = i;
          }
        }
        else if(meType == High) {
          double dDiff = dBestResolution - mdResVector[i];
          if(dBestResolution == -1 || dDiff > 0) {
            dBestResolution = mdResVector[i];
            iBestIndex = i;
          }
        }
        else if(meType == Mean) {
          double dDiff = fabs(dMean - mdResVector[i]);
          if(dBestResolution == -1 || dDiff <  dBestResolution) {
            dBestResolution = dDiff;
            iBestIndex = i;
          }
        }
        else if(meType == Nearest) {
          double dDiff = fabs(mdResValue - mdResVector[i]);
          if(dBestResolution == -1 || dDiff <  dBestResolution) {
            dBestResolution = dDiff;
            iBestIndex = i;
          }
        }
        else if(meType == Range) {
          if(mdResVector[i] >=  mdMinRes && mdResVector[i] <= mdMaxRes) {
            iBestIndex = i;
            return iBestIndex;
          }
        }
      }
    }
    return iBestIndex;
  }
};


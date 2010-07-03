#include "CnetRefByIncidence.h"
#include "ControlNet.h"
#include "PvlGroup.h"
#include "Camera.h"
#include "Portal.h"
#include "SpecialPixel.h"
#include "Pvl.h"

namespace Isis {
  /**
   * Construct CnetRefByIncidence with a PVL Def file
   *
   * @author Sharmila Prasad (5/24/2010)
   *
   * @param pPvlDef
   * @param pOrigNet
   * @param pNewNet
   * @param psSerialNumfile
   */
  CnetRefByIncidence::CnetRefByIncidence(Pvl *pPvlDef, std::string psSerialNumfile): CnetValidMeasure(pPvlDef) {
    ReadSerialNumbers(psSerialNumfile);
  }

  /**
   * This traverses all the control points and measures in the network and
   * checks for valid Measure which passes the Emission Incidence Angle, DN value tests
   * and  picks the Measure with the best Incidence Angle (closer) to zero as the Reference
   *
   * @author Sharmila Prasad (5/24/2010)
   *
   * @return New ControlNet
   */
  void CnetRefByIncidence::FindCnetRef(const ControlNet &pOrigNet, ControlNet &pNewNet) {
    // Process each existing control point in the network
    int iTotalMeasures = 0;
    int iPointsModified = 0;
    int iMeasuresModified = 0;
    int iRefChanged = 0;

    //Status Report
    mStatus.SetText("Choosing Reference by Incidence...");
    mStatus.SetMaximumSteps(pOrigNet.Size());
    mStatus.CheckStatus();

    //mPvlLog += GetStdOptions();
    for(int point = 0; point < pOrigNet.Size(); ++point) {
      ControlPoint &origPnt = ((ControlNet &)pOrigNet)[point];  // to remove the const

      // Stats and Accounting
      iTotalMeasures += origPnt.Size();

      // Logging
      PvlObject pvlPointObj("PointDetails");
      pvlPointObj += Isis::PvlKeyword("PointId", origPnt.Id());

      // Create a new control point
      ControlPoint newPnt;
      newPnt.SetId(origPnt.Id());
      newPnt.SetType(origPnt.Type());

      int iRefIndex = origPnt.ReferenceIndexNoException();
      iString istrTemp;

      std::vector <PvlGroup> pvlGrpVector;
      std::vector<double> bestIncidenceAngle;
      int iBestIndex = -1;

      // Only perform the interest operation on points of type "Tie" and
      // Points having atleast 1 measure and Point must not be Ignored
      if(!origPnt.Ignore() && origPnt.Type() == ControlPoint::Tie && iRefIndex >= 0) {
        // Create a measurment for each image in this point using the reference
        // lat/lon.
        int iNumIgnore = 0;
        iString istrTemp;
        double dBestIncidenceAngle = 135;

        for(int measure = 0; measure < origPnt.Size(); ++measure) {
          ControlMeasure newMsr(origPnt[measure]); //copy constructor
          newMsr.SetDateTime();
          newMsr.SetChooserName("Application cnetref(Incidence)");

          double dSample = origPnt[measure].Sample();
          double dLine   = origPnt[measure].Line();
          std::string sn = origPnt[measure].CubeSerialNumber();

          // Log
          PvlGroup pvlMeasureGrp("MeasureDetails");
          pvlMeasureGrp += Isis::PvlKeyword("SerialNum", sn);
          pvlMeasureGrp += Isis::PvlKeyword("OriginalLocation", LocationString(dSample, dLine));

          if(!newMsr.Ignore()) {
            Cube *measureCube = mCubeMgr.OpenCube(mSerialNumbers.Filename(sn));

            newMsr.SetReference(false);
            newMsr.SetIgnore(false);

            if(ValidStandardOptions(dSample, dLine, measureCube, &pvlMeasureGrp)) {
              if(mdIncidenceAngle < dBestIncidenceAngle) {
                dBestIncidenceAngle = mdIncidenceAngle;
                iBestIndex = measure;
              }
            }
            else {
              pvlMeasureGrp += Isis::PvlKeyword("Ignored", "Failed Emission, Incidence, Resolution and/or Dn Value Test");
              newMsr.SetIgnore(true);
              iNumIgnore++;
            }
            bestIncidenceAngle.push_back(mdIncidenceAngle);
          } // Ignore==False
          else {
            pvlMeasureGrp += Isis::PvlKeyword("Ignored", "Originally Ignored");
            iNumIgnore++;
          }
          newPnt.Add(newMsr);
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
        if(!newPnt.Ignore() && iBestIndex >= 0 && !newPnt[iBestIndex].Ignore()) {
          newPnt[iBestIndex].SetReference(true);
          pvlGrpVector[iBestIndex] += Isis::PvlKeyword("Reference", "true");
        }

        for(int i = 0; i < newPnt.Size(); i++) {
          pvlPointObj += pvlGrpVector[i];
        }
        pNewNet.Add(newPnt);
      } // end Tie
      else {
        newPnt = origPnt;
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
          newPnt[measure].SetChooserName("Application cnetref(Incidence)");
        }
        pNewNet.Add(newPnt);
      }

      if(newPnt != origPnt) {
        iPointsModified++;
      }

      if(!newPnt.Ignore() && iBestIndex != iRefIndex) {
        iRefChanged++;
        PvlGroup pvlRefChangeGrp("ReferenceChangeDetails");
        pvlRefChangeGrp += Isis::PvlKeyword("PrevSerialNumber", origPnt[iRefIndex].CubeSerialNumber());
        pvlRefChangeGrp += Isis::PvlKeyword("PrevIncAngle",  bestIncidenceAngle[iRefIndex]);

        istrTemp = iString((int)origPnt[iRefIndex].Sample());
        istrTemp += ",";
        istrTemp += iString((int)origPnt[iRefIndex].Line());
        pvlRefChangeGrp += Isis::PvlKeyword("PrevLocation",     istrTemp);

        pvlRefChangeGrp += Isis::PvlKeyword("NewSerialNumber",  newPnt[iBestIndex].CubeSerialNumber());
        pvlRefChangeGrp += Isis::PvlKeyword("NewLeastIncAngle",   bestIncidenceAngle[iBestIndex]);

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
    mStatisticsGrp += Isis::PvlKeyword("TotalPoints",      pOrigNet.Size());
    mStatisticsGrp += Isis::PvlKeyword("PointsIgnored", (pNewNet.Size() - pNewNet.NumValidPoints()));
    mStatisticsGrp += Isis::PvlKeyword("PointsModified",   iPointsModified);
    mStatisticsGrp += Isis::PvlKeyword("ReferenceChanged", iRefChanged);
    mStatisticsGrp += Isis::PvlKeyword("TotalMeasures",    iTotalMeasures);
    mStatisticsGrp += Isis::PvlKeyword("MeasuresModified", iMeasuresModified);

    mPvlLog += mStatisticsGrp;
  }
};

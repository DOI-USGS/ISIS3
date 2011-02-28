#include "Application.h"
#include "CnetRefByIncidence.h"
#include "ControlNet.h"
#include "ControlPoint.h"
#include "ControlMeasure.h"
#include "PvlGroup.h"
#include "Camera.h"
#include "MeasureValidationResults.h"
#include "Portal.h"
#include "SpecialPixel.h"
#include "Pvl.h"

namespace Isis {
  /**
   * Construct CnetRefByIncidence with a PVL Def file
   *
   * @author Sharmila Prasad (5/24/2010)
   *
   * @param pPvlDef         - Pvl Definition File
   * @param psSerialNumfile - Serial Number file attached to the ControlNet
   */
  CnetRefByIncidence::CnetRefByIncidence(Pvl *pPvlDef, std::string psSerialNumfile)
    : ControlNetValidMeasure(pPvlDef) {
    ReadSerialNumbers(psSerialNumfile);
  }

  /**
   * This traverses all the control points and measures in the network and
   * checks for valid Measure which passes the Emission Incidence Angle, DN value tests
   * and  picks the Measure with the best Incidence Angle (closer) to zero as the Reference
   *
   * @author Sharmila Prasad (5/24/2010)
   * @history 2010-10-04 Sharmila Prasad - Modified for Binary CNet
   * @history 2010-10-15 Sharmila Prasad - Use single copy of Control Net in FindCnetRef()
   *
   * @param pNewNet   - Modified output Control Net
   */
  void CnetRefByIncidence::FindCnetRef(ControlNet &pNewNet) {
    // Process each existing control point in the network
    int iTotalMeasures = 0;
    int iPointsModified = 0;
    int iMeasuresModified = 0;
    int iRefChanged = 0;

    //Status Report
    mStatus.SetText("Choosing Reference by Incidence...");
    mStatus.SetMaximumSteps(pNewNet.GetNumPoints());
    mStatus.CheckStatus();

    for (int point = 0; point < pNewNet.GetNumPoints(); ++point) {
      ControlPoint *newPnt = pNewNet.GetPoint(point);
      const ControlPoint *origPnt = newPnt;

      // Stats and Accounting
      iTotalMeasures += newPnt->GetNumMeasures();

      // Logging
      PvlObject pvlPointObj("PointDetails");
      pvlPointObj += Isis::PvlKeyword("PointId", newPnt->GetId());

      // Edit Lock Option
      bool bPntEditLock = newPnt->IsEditLocked();
      if (!bPntEditLock) {
        newPnt->SetDateTime(Application::DateTime());
      }

      int iNumMeasuresLocked = newPnt->GetNumLockedMeasures();
      bool bRefLocked = newPnt->GetRefMeasure()->IsEditLocked();

      int iRefIndex = -1;
      if (newPnt->ReferenceHasBeenExplicitlySet())
        iRefIndex = newPnt->IndexOfRefMeasure();
      iString istrTemp;

      std::vector <PvlGroup> pvlGrpVector;
      std::vector <double>   bestIncidenceAngle;
      int iBestIndex = 0;

      // Only perform the interest operation on points of type "Tie" and
      // Points having atleast 1 measure and Points not Ignored
      // Check for EditLock in the Measures and also verfify that
      // only a Reference Measure can be Locked else error
      if (!newPnt->IsIgnored() && newPnt->GetType() == ControlPoint::Tie && iRefIndex >= 0 &&
          (iNumMeasuresLocked == 0 || (iNumMeasuresLocked > 0 && bRefLocked))) {
        int iNumIgnore = 0;
        iString istrTemp;
        double dBestIncidenceAngle = 135;

        for (int measure = 0; measure < newPnt->GetNumMeasures(); ++measure) {

          ControlMeasure *newMsr = newPnt->GetMeasure(measure);
          bool bMeasureLocked = newMsr->IsEditLocked();

          if (!bPntEditLock && !bMeasureLocked) {
            newMsr->SetDateTime(Application::DateTime());
            newMsr->SetChooserName("Application cnetref(Incidence)");
          }

          std::string sn = newMsr->GetCubeSerialNumber();
          double dSample = newMsr->GetSample();
          double dLine   = newMsr->GetLine();

          // Log
          PvlGroup pvlMeasureGrp("MeasureDetails");
          pvlMeasureGrp += Isis::PvlKeyword("SerialNum", sn);
          pvlMeasureGrp += Isis::PvlKeyword("OriginalLocation", LocationString(dSample, dLine));

          if (!newMsr->IsIgnored()) {
            Cube *measureCube = mCubeMgr.OpenCube(mSerialNumbers.Filename(sn));

            MeasureValidationResults results =
              ValidStandardOptions(dSample, dLine, measureCube, &pvlMeasureGrp);
            if (results.isValid()) {
              if (!bPntEditLock && !bRefLocked) {
                newMsr->SetType(ControlMeasure::Candidate);
                if (mdIncidenceAngle < dBestIncidenceAngle) {
                  dBestIncidenceAngle = mdIncidenceAngle;
                  iBestIndex = measure;
                }
              }
            }
            else {
              if (bPntEditLock) {
                pvlMeasureGrp += Isis::PvlKeyword("UnIgnored", "Failed Validation Test but not Ignored as Point EditLock is True");
              }
              else if (bMeasureLocked) {
                pvlMeasureGrp += Isis::PvlKeyword("UnIgnored", "Failed Validation Test but not Ignored as Measure EditLock is True");
              }
              else {
                pvlMeasureGrp += Isis::PvlKeyword("Ignored", "Failed Validation Test");
                newMsr->SetIgnored(true);
                iNumIgnore++;
              }
            }
            bestIncidenceAngle.push_back(mdIncidenceAngle);
          } // Ignore == false
          else {
            pvlMeasureGrp += Isis::PvlKeyword("Ignored", "Originally Ignored");
            iNumIgnore++;
          }
          //newPnt.Add(newMsr);
          if (newMsr != origPnt->GetMeasure(measure)) {
            iMeasuresModified++;
          }

          //newPnt.UpdateMeasure(newMsr); // Redesign fixed this
          pvlGrpVector.push_back(pvlMeasureGrp);
        }// end Measure

        if ((newPnt->GetNumMeasures() - iNumIgnore) < 2) {
          if (bPntEditLock) {
            pvlPointObj += Isis::PvlKeyword("UnIgnored", "Good Measures less than 2 but not Ignored as Point EditLock is True");
          }
          else {
            newPnt->SetIgnored(true);
            pvlPointObj += Isis::PvlKeyword("Ignored", "Good Measures less than 2");
          }
        }

        // Set the Reference if the Point is unlocked and Reference measure is unlocked
        if (!newPnt->IsIgnored() && iBestIndex >= 0 &&
            !newPnt->GetMeasure(iBestIndex)->IsIgnored() && !bPntEditLock && !bRefLocked) {
          newPnt->SetRefMeasure(iBestIndex);
          pvlGrpVector[iBestIndex] += Isis::PvlKeyword("Reference", "true");
          //newPnt.UpdateMeasure(cm); // Redesign fixed this

          // Log info, if Point not locked, apriori source == Reference and a new reference
          if (iRefIndex != iBestIndex &&
              newPnt->GetAprioriSurfacePointSource() == ControlPoint::SurfacePointSource::Reference) {
            pvlGrpVector[iBestIndex] += Isis::PvlKeyword("AprioriSource", "Reference is the source and has changed");
          }
        }

        for (int i = 0; i < newPnt->GetNumMeasures(); i++) {
          pvlPointObj += pvlGrpVector[i];
        }
      } // end Tie
      else {
        int iComment = 1;
        if (iRefIndex < 0) {
          std::string sComment = "Comment" + iComment++;
          pvlPointObj += Isis::PvlKeyword(sComment, "No Measures in the Point");
        }

        if (newPnt->IsIgnored()) {
          std::string sComment = "Comment" + iComment++;
          pvlPointObj += Isis::PvlKeyword(sComment, "Point was originally Ignored");
        }

        if (origPnt->GetType() == ControlPoint::Ground) {
          std::string sComment = "Comment" + iComment++;
          pvlPointObj += Isis::PvlKeyword(sComment, "Not a Tie Point");
        }

        if (iNumMeasuresLocked > 0 && !bRefLocked) {
          pvlPointObj += Isis::PvlKeyword("Error", "Point has Measure(s) with EditLock set to true but not the Reference");
        }

        for (int measure = 0; measure < newPnt->GetNumMeasures(); measure++) {
          ControlMeasure *cm = newPnt->GetMeasure(measure);
          cm->SetDateTime(Application::DateTime());
          cm->SetChooserName("Application cnetref(Incidence)");
          //newPnt.UpdateMeasure(cm); // Redesign fixed this
        }
      }

      if (newPnt != origPnt) {
        iPointsModified++;
      }

      if (!newPnt->IsIgnored() && newPnt->ReferenceHasBeenExplicitlySet() && iBestIndex != iRefIndex 
          && !bPntEditLock && !bRefLocked) {
        iRefChanged++;
        PvlGroup pvlRefChangeGrp("ReferenceChangeDetails");
        pvlRefChangeGrp += Isis::PvlKeyword("PrevSerialNumber",
            origPnt->GetMeasure(iRefIndex)->GetCubeSerialNumber());
        pvlRefChangeGrp += Isis::PvlKeyword("PrevIncAngle",     bestIncidenceAngle[iRefIndex]);

        istrTemp = iString((int)origPnt->GetMeasure(iRefIndex)->GetSample());
        istrTemp += ",";
        istrTemp += iString((int)origPnt->GetMeasure(iRefIndex)->GetLine());
        pvlRefChangeGrp += Isis::PvlKeyword("PrevLocation",     istrTemp);

        pvlRefChangeGrp += Isis::PvlKeyword("NewSerialNumber",
            newPnt->GetMeasure(iBestIndex)->GetCubeSerialNumber());
        pvlRefChangeGrp += Isis::PvlKeyword("NewLeastIncAngle", bestIncidenceAngle[iBestIndex]);

        istrTemp = iString((int)newPnt->GetMeasure(iBestIndex)->GetSample());
        istrTemp += ",";
        istrTemp += iString((int)newPnt->GetMeasure(iBestIndex)->GetLine());
        pvlRefChangeGrp += Isis::PvlKeyword("NewLocation",      istrTemp);

        pvlPointObj += pvlRefChangeGrp;
      }
      else {
        pvlPointObj += Isis::PvlKeyword("Reference", "No Change");
      }

      //pNewNet.UpdatePoint(newPnt); // Redesign fixed this
      mPvlLog += pvlPointObj;
      mStatus.CheckStatus();
    }// end Point

    // Basic Statistics
    mStatisticsGrp += Isis::PvlKeyword("TotalPoints",      pNewNet.GetNumPoints());
    mStatisticsGrp += Isis::PvlKeyword("PointsIgnored", (pNewNet.GetNumPoints() - pNewNet.GetNumValidPoints()));
    mStatisticsGrp += Isis::PvlKeyword("PointsModified",   iPointsModified);
    mStatisticsGrp += Isis::PvlKeyword("ReferenceChanged", iRefChanged);
    mStatisticsGrp += Isis::PvlKeyword("TotalMeasures",    iTotalMeasures);
    mStatisticsGrp += Isis::PvlKeyword("MeasuresModified", iMeasuresModified);

    mPvlLog += mStatisticsGrp;
  }
};

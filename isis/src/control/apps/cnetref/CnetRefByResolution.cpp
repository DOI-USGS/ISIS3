#include "Application.h"
#include "ControlNet.h"
#include "ControlPoint.h"
#include "ControlMeasure.h"
#include "PvlGroup.h"
#include "Camera.h"
#include "MeasureValidationResults.h"
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
    : ControlNetValidMeasure(pPvlDef) {
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
   * @history 2010-10-04 Sharmila Prasad - Modified for Binary CNet (Edit Lock)
   *
   * @param pNewNet   - Modified output Control Net
   *
   */
  void CnetRefByResolution::FindCnetRef(ControlNet &pNewNet) {
    // Process each existing control point in the network
    int iTotalMeasures = 0;
    int iPointsModified = 0;
    int iMeasuresModified = 0;
    int iRefChanged = 0;

    //Status Report
    mStatus.SetText("Choosing Reference by Resolution...");
    mStatus.SetMaximumSteps(pNewNet.GetNumPoints());
    mStatus.CheckStatus();

    //mPvlLog += GetStdOptions();
    for (int point = 0; point < pNewNet.GetNumPoints(); ++point) {
      ControlPoint *newPnt = pNewNet.GetPoint(point);
      const ControlPoint *origPnt = newPnt;

      mdResVector.clear();

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
      if (newPnt->IsReferenceExplicit())
        iRefIndex = newPnt->IndexOfRefMeasure();
      iString istrTemp;

      std::vector <PvlGroup> pvlGrpVector;
      int iBestIndex = 0;

      // Only perform the interest operation on points of type "Tie" and
      // Points having atleast 1 measure and Point is not Ignored
      // Check for EditLock in the Measures and also verfify that
      // only a Reference Measure can be Locked else error
      if (!newPnt->IsIgnored() && newPnt->GetType() == ControlPoint::Tie && iRefIndex >= 0 &&
          (iNumMeasuresLocked == 0 || (iNumMeasuresLocked > 0 && bRefLocked))) {
        int iNumIgnore = 0;
        iString istrTemp;

        for (int measure = 0; measure < newPnt->GetNumMeasures(); ++measure) {

          ControlMeasure *newMsr = newPnt->GetMeasure(measure);

          bool bMeasureLocked = newMsr->IsEditLocked();
          double dSample      = newMsr->GetSample();
          double dLine        = newMsr->GetLine();
          std::string sn      = newMsr->GetCubeSerialNumber();

          if (!bPntEditLock && !bMeasureLocked) {
            newMsr->SetDateTime(Application::DateTime());
            newMsr->SetChooserName("Application cnetref(Resolution)");
          }

          // Log
          PvlGroup pvlMeasureGrp("MeasureDetails");
          pvlMeasureGrp += Isis::PvlKeyword("SerialNum", sn);
          pvlMeasureGrp += Isis::PvlKeyword("OriginalLocation", LocationString(dSample, dLine));

          if (!newMsr->IsIgnored()) {
            Cube *measureCube = mCubeMgr.OpenCube(mSerialNumbers.Filename(sn));

            MeasureValidationResults results =
              ValidStandardOptions(dSample, dLine, measureCube, &pvlMeasureGrp);
            if (!results.isValid()) {
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
            } // valid measure
            else {
              if (!bPntEditLock && !bRefLocked) {
                newMsr->SetType(ControlMeasure::Candidate);
                newMsr->SetIgnored(false);
                mdResVector.push_back(mdResolution);
              }
            }
          } // Ignore == false
          else {
            pvlMeasureGrp += Isis::PvlKeyword("Ignored", "Originally Ignored");
            iNumIgnore++;
          }
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
        if (!newPnt->IsIgnored() && !bPntEditLock && !bRefLocked) {
          iBestIndex = GetReferenceByResolution(newPnt);
          if (iBestIndex >= 0 && !newPnt->GetMeasure(iBestIndex)->IsIgnored()) {
            newPnt->SetRefMeasure(iBestIndex);
            //newPnt.UpdateMeasure(cm); // Redesign fixed this
            pvlGrpVector[iBestIndex] += Isis::PvlKeyword("Reference", "true");
          }
          else {
            if (iBestIndex < 0 && meType == Range) {
              pvlPointObj += Isis::PvlKeyword("NOTE", "No Valid Measures within the Resolution Range. Reference defaulted to the first Measure");
            }
            iBestIndex = 0;
            newPnt->SetRefMeasure(iBestIndex);
            //newPnt.UpdateMeasure(cm); // Redesign fixed this

            // Log info, if Point not locked, apriori source == Reference and a new reference
            if (iRefIndex != iBestIndex &&
                newPnt->GetAprioriSurfacePointSource() == ControlPoint::SurfacePointSource::Reference) {
              pvlGrpVector[iBestIndex] += Isis::PvlKeyword("AprioriSource", "Reference is the source and has changed");
            }
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

        if (newPnt->GetType() == ControlPoint::Ground) {
          std::string sComment = "Comment" + iComment++;
          pvlPointObj += Isis::PvlKeyword(sComment, "Not a Tie Point");
        }

        if (iNumMeasuresLocked > 0 && !bRefLocked) {
          pvlPointObj += Isis::PvlKeyword("Error", "Point has Measure(s) with EditLock set to true but not the Reference");
        }

        for (int measure = 0; measure < newPnt->GetNumMeasures(); measure++) {
          ControlMeasure *cm = newPnt->GetMeasure(iBestIndex);
          cm->SetDateTime(Application::DateTime());
          cm->SetChooserName("Application cnetref(Resolution)");
          //newPnt.UpdateMeasure(cm); // Redesign fixed this
        }
      }

      if (newPnt != origPnt) {
        iPointsModified++;
      }

      if (!newPnt->IsIgnored() && newPnt->IsReferenceExplicit() && iBestIndex != iRefIndex 
          && !bPntEditLock && !bRefLocked) {
        iRefChanged++;
        PvlGroup pvlRefChangeGrp("ReferenceChangeDetails");
        pvlRefChangeGrp += Isis::PvlKeyword("PrevSerialNumber",
            origPnt->GetMeasure(iRefIndex)->GetCubeSerialNumber());
        pvlRefChangeGrp += Isis::PvlKeyword("PrevResolution",   mdResVector[iRefIndex]);

        istrTemp = iString((int)origPnt->GetMeasure(iRefIndex)->GetSample());
        istrTemp += ",";
        istrTemp += iString((int)origPnt->GetMeasure(iRefIndex)->GetLine());
        pvlRefChangeGrp += Isis::PvlKeyword("PrevLocation",     istrTemp);

        pvlRefChangeGrp += Isis::PvlKeyword("NewSerialNumber",
            newPnt->GetMeasure(iBestIndex)->GetCubeSerialNumber());
        std::string sKeyName = "NewHighestResolution";
        if (meType == Low) {
          sKeyName = "NewLeastResolution";
        }
        else if (meType == Mean) {
          pvlRefChangeGrp += Isis::PvlKeyword("MeanResolution",  GetMeanResolution());
          sKeyName = "NewResolutionNeartoMean";
        }
        else if (meType == Nearest) {
          sKeyName = "NewResolutionNeartoValue";
        }
        else if (meType == Range) {
          sKeyName = "NewResolutionInRange";
        }
        pvlRefChangeGrp += Isis::PvlKeyword(sKeyName,  mdResVector[iBestIndex]);

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
    for (int i = 0; i < (int)mdResVector.size(); i++) {
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
  int CnetRefByResolution::GetReferenceByResolution(ControlPoint *pNewPoint) {
    int iBestIndex = -1;
    double dBestResolution = -1;
    double dMean = 0;
    if (meType == Mean) {
      dMean = GetMeanResolution();
    }

    for (int i = 0; i < (int)mdResVector.size(); i++) {
      if (pNewPoint->GetMeasure(i)->IsIgnored()) {
        continue;
      }
      else {
        double dDiff = dBestResolution - mdResVector[i];
        if (meType == Low) {
          if (dBestResolution == -1 || dDiff < 0) {
            dBestResolution = mdResVector[i];
            iBestIndex = i;
          }
        }
        else if (meType == High) {
          double dDiff = dBestResolution - mdResVector[i];
          if (dBestResolution == -1 || dDiff > 0) {
            dBestResolution = mdResVector[i];
            iBestIndex = i;
          }
        }
        else if (meType == Mean) {
          double dDiff = fabs(dMean - mdResVector[i]);
          if (dBestResolution == -1 || dDiff <  dBestResolution) {
            dBestResolution = dDiff;
            iBestIndex = i;
          }
        }
        else if (meType == Nearest) {
          double dDiff = fabs(mdResValue - mdResVector[i]);
          if (dBestResolution == -1 || dDiff <  dBestResolution) {
            dBestResolution = dDiff;
            iBestIndex = i;
          }
        }
        else if (meType == Range) {
          if (mdResVector[i] >=  mdMinRes && mdResVector[i] <= mdMaxRes) {
            iBestIndex = i;
            return iBestIndex;
          }
        }
      }
    }
    return iBestIndex;
  }
};


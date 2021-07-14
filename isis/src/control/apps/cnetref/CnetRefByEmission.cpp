/** This is free and unencumbered software released into the public domain.

The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */

#include "Application.h"
#include "CnetRefByEmission.h"
#include "ControlNet.h"
#include "ControlPoint.h"
#include "ControlMeasure.h"
#include "PvlGroup.h"
#include "Camera.h"
#include "MeasureValidationResults.h"
#include "Portal.h"
#include "SpecialPixel.h"
#include "Pvl.h"


using namespace std;

namespace Isis {
  /**
   * Construct CnetRefByEmission with a PVL Def file
   *
   * @author Sharmila Prasad (5/14/2010)
   *
   * @param pPvlDef         - Pvl Definition File
   * @param psSerialNumfile - Serial Number file attached to the ControlNet
   */
  CnetRefByEmission::CnetRefByEmission(Pvl *pPvlDef, QString psSerialNumfile)
    : ControlNetValidMeasure(pPvlDef) {
    ReadSerialNumbers(psSerialNumfile);
    SetCameraRequiredFlag(true);
  }

  /**
   * This traverses all the control points and measures in the network and
   * checks for valid Measure which passes the Emission Incidence Angle, DN value tests
   * and  picks the Measure with the best Emission Angle (closer) to zero as the Reference
   *
   * @author Sharmila Prasad (5/14/2010)
   * @history 2010-10-04 Sharmila Prasad - Modified for Binary CNet
   * @history 2010-10-15 Sharmila Prasad - Use single copy of Control Net in FindCnetRef()
   *
   * @param pNewNet   - Modified output Control Net
   *
   */
  void CnetRefByEmission::FindCnetRef(ControlNet &pNewNet) {
    // Process each existing control point in the network
    int iPointsModified = 0;
    int iMeasuresModified = 0;
    int iRefChanged = 0;

    //Status Report
    mStatus.SetText("Choosing Reference by Emission...");
    mStatus.SetMaximumSteps(pNewNet.GetNumPoints());
    mStatus.CheckStatus();

    for (int point = 0; point < pNewNet.GetNumPoints(); ++point) {
      ControlPoint *newPnt = pNewNet.GetPoint(point);

      //cerr << "Point " <<  point << "  ID=" << newPnt->GetId() ;
      // Create a copy of original control point
      const ControlPoint origPnt(*newPnt);
      bool bError = false;

      // Logging
      PvlObject pvlPointObj("PointDetails");
      pvlPointObj += Isis::PvlKeyword("PointId", newPnt->GetId());

      // Edit Lock Option
      bool bPntEditLock = newPnt->IsEditLocked();
      if (!bPntEditLock) {
        newPnt->SetDateTime(Application::DateTime());
      }
      else {
        pvlPointObj += Isis::PvlKeyword("Reference", "No Change, PointEditLock");
      }

      int iNumMeasuresLocked = newPnt->GetNumLockedMeasures();
      bool bRefLocked = newPnt->GetRefMeasure()->IsEditLocked();
      int numMeasures = newPnt->GetNumMeasures();

      int iRefIndex = -1;
      if (newPnt->IsReferenceExplicit())
        iRefIndex = newPnt->IndexOfRefMeasure();
      QString istrTemp;

      std::vector <PvlGroup> pvlGrpVector;
      std::vector <double>   bestEmissionAngle;
      int iBestIndex = 0;

      // Only perform the interest operation on points of type "Free" and
      // Points having atleast 1 measure and Points not Ignored
      // Check for EditLock in the Measures and also verfify that
      // only a Reference Measure can be Locked else error
      if (!newPnt->IsIgnored() && newPnt->GetType() == ControlPoint::Free && numMeasures > 0 &&
          (iNumMeasuresLocked == 0 || (iNumMeasuresLocked > 0 && bRefLocked))) {
        int iNumIgnore = 0;
        QString istrTemp;
        double dBestEmissionAngle = 135;

        for (int measure = 0; measure < numMeasures; ++measure) {
          ControlMeasure *newMsr = newPnt->GetMeasure(measure);
          bool bMeasureLocked = newMsr->IsEditLocked();
          if (!bPntEditLock && !bMeasureLocked) {
            newMsr->SetDateTime(Application::DateTime());
            newMsr->SetChooserName("Application cnetref(Emission)");
          }

          QString sn = newMsr->GetCubeSerialNumber();
          double dSample = newMsr->GetSample();
          double dLine   = newMsr->GetLine();

          // Log
          PvlGroup pvlMeasureGrp("MeasureDetails");
          pvlMeasureGrp += Isis::PvlKeyword("SerialNum", sn);
          pvlMeasureGrp += Isis::PvlKeyword("OriginalLocation", LocationString(dSample, dLine));

          if (bMeasureLocked)
            pvlMeasureGrp += Isis::PvlKeyword("EditLock", "True");

          if (!newMsr->IsIgnored()) {
            Cube *measureCube = mCubeMgr.OpenCube(mSerialNumbers.fileName(sn));

            MeasureValidationResults results =
              ValidStandardOptions(newMsr, measureCube, &pvlMeasureGrp);
            if (results.isValid()) {
              if (!bPntEditLock && !bRefLocked) {
                newMsr->SetType(ControlMeasure::Candidate);
                if (mdEmissionAngle < dBestEmissionAngle) {
                  dBestEmissionAngle = mdEmissionAngle;
                  iBestIndex = measure;
                }
              }
            }
            else {
              if (bPntEditLock) {
                pvlMeasureGrp += Isis::PvlKeyword("UnIgnored", "Failed Validation Test but "
                    "not Ignored as Point EditLock is True");
              }
              else if (bMeasureLocked) {
                pvlMeasureGrp += Isis::PvlKeyword("UnIgnored", "Failed Validation Test but "
                    "not Ignored as Measure EditLock is True");
              }
              else {
                pvlMeasureGrp += Isis::PvlKeyword("Ignored",   "Failed Validation Test");
                newMsr->SetIgnored(true);
                iNumIgnore++;
              }
            }
            bestEmissionAngle.push_back(mdEmissionAngle);
          } // Ignore == false
          else {
            pvlMeasureGrp += Isis::PvlKeyword("Ignored", "Originally Ignored");
            iNumIgnore++;
          }
          if (newMsr != origPnt.GetMeasure(measure)) {
            iMeasuresModified++;
          }
          pvlGrpVector.push_back(pvlMeasureGrp);
        }// end Measure

        if ((newPnt->GetNumMeasures() - iNumIgnore) < 2) {
          if (bPntEditLock) {
            pvlPointObj += Isis::PvlKeyword("UnIgnored", "Good Measures less than 2 but "
                "not Ignored as Point EditLock is True");
          }
          else {
            newPnt->SetIgnored(true);
            pvlPointObj += Isis::PvlKeyword("Ignored", "Good Measures less than 2");
          }
        }

        // Set the Reference if the Point is unlocked and Reference measure is unlocked
        if (!newPnt->IsIgnored() && iBestIndex >= 0 &&
            !newPnt->GetMeasure(iBestIndex)->IsIgnored() &&
            !bPntEditLock && !bRefLocked) {
          newPnt->SetRefMeasure(iBestIndex);
          pvlGrpVector[iBestIndex] += Isis::PvlKeyword("Reference", "true");

          // Log info, if Point not locked, apriori source == Reference and a new reference
          if (iRefIndex != iBestIndex &&
              newPnt->GetAprioriSurfacePointSource() == ControlPoint::SurfacePointSource::Reference) {
            pvlGrpVector[iBestIndex] += Isis::PvlKeyword("AprioriSource", "Reference is the source and has changed");
          }
        }

        for (int i = 0; i < newPnt->GetNumMeasures(); i++) {
          pvlPointObj += pvlGrpVector[i];
        }
      } // end Free
      else {
        int iComment = 0;
        if (numMeasures == 0) {
          QString sComment = "Comment";
          sComment += toString(++iComment);
          pvlPointObj += Isis::PvlKeyword(sComment, "No Measures in the Point");
        }

        if (newPnt->IsIgnored()) {
          QString sComment = "Comment";
          sComment += toString(++iComment);
          pvlPointObj += Isis::PvlKeyword(sComment, "Point was originally Ignored");
        }

        if (origPnt.GetType() == ControlPoint::Fixed) {
          QString sComment = "Comment";
          sComment += toString(++iComment);
          pvlPointObj += Isis::PvlKeyword(sComment, "Fixed Point");
        }
        else if (newPnt->GetType() == ControlPoint::Constrained) {
          QString sComment = "Comment";
          sComment += toString(++iComment);
          pvlPointObj += Isis::PvlKeyword(sComment, "Constrained Point");
        }

        if (iNumMeasuresLocked > 0 && !bRefLocked) {
          pvlPointObj += Isis::PvlKeyword("Error", "Point has a Measure with EditLock set to true "
                                          "but the Reference is not Locked");
          //QString message = "Invalid Nearest Resolution Value";
          //throw Isis::iException::Message(Isis::iException::User, message, _FILEINFO_);
          bError = true;
        }
        else {
          for (int measure = 0; measure < newPnt->GetNumMeasures(); measure++) {
            ControlMeasure *cm = newPnt->GetMeasure(measure);
            cm->SetDateTime(Application::DateTime());
            cm->SetChooserName("Application cnetref(Emission)");
          }
        }
      }

      if (*newPnt != origPnt) {
        iPointsModified++;
      }

      if (!bError && !newPnt->IsIgnored() && newPnt->IsReferenceExplicit() &&
          iBestIndex != iRefIndex && !bPntEditLock && !bRefLocked) {
        iRefChanged++;
        PvlGroup pvlRefChangeGrp("ReferenceChangeDetails");
        if (iRefIndex >= 0) {
          pvlRefChangeGrp += Isis::PvlKeyword("PrevSerialNumber",
              origPnt.GetReferenceSN());
          pvlRefChangeGrp += Isis::PvlKeyword("PrevEmAngle",
              toString(bestEmissionAngle[iRefIndex]));

          istrTemp = toString((int)origPnt.GetMeasure(iRefIndex)->GetSample());
          istrTemp += ",";
          istrTemp += toString((int)origPnt.GetMeasure(iRefIndex)->GetLine());
          pvlRefChangeGrp += Isis::PvlKeyword("PrevLocation",     istrTemp);
        }
        else {
          pvlRefChangeGrp += Isis::PvlKeyword("PrevReference", "Not Set");
        }

        pvlRefChangeGrp += Isis::PvlKeyword("NewSerialNumber",
            newPnt->GetMeasure(iBestIndex)->GetCubeSerialNumber());
        pvlRefChangeGrp += Isis::PvlKeyword("NewLeastEmAngle",
            toString(bestEmissionAngle[iBestIndex]));

        istrTemp = toString((int)newPnt->GetMeasure(iBestIndex)->GetSample());
        istrTemp += ",";
        istrTemp += toString((int)newPnt->GetMeasure(iBestIndex)->GetLine());
        pvlRefChangeGrp += Isis::PvlKeyword("NewLocation", istrTemp);

        pvlPointObj += pvlRefChangeGrp;
      }
      else {
        pvlPointObj += Isis::PvlKeyword("Reference", "No Change");
      }

      mPvlLog += pvlPointObj;
      mStatus.CheckStatus();
    }// end Point

    // CnetRef Change Statistics
    mStatisticsGrp += Isis::PvlKeyword("PointsModified",   toString(iPointsModified));
    mStatisticsGrp += Isis::PvlKeyword("ReferenceChanged", toString(iRefChanged));
    mStatisticsGrp += Isis::PvlKeyword("MeasuresModified", toString(iMeasuresModified));

    mPvlLog += mStatisticsGrp;
  }
};

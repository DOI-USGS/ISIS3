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

namespace Isis {
  /**
   * Construct CnetRefByEmission with a PVL Def file
   *
   * @author Sharmila Prasad (5/14/2010)
   *
   * @param pPvlDef         - Pvl Definition File
   * @param psSerialNumfile - Serial Number file attached to the ControlNet
   */
  CnetRefByEmission::CnetRefByEmission(Pvl *pPvlDef, std::string psSerialNumfile)
                    :ControlNetValidMeasure(pPvlDef) {
    ReadSerialNumbers(psSerialNumfile);
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
    int iTotalMeasures = 0;
    int iPointsModified = 0;
    int iMeasuresModified = 0;
    int iRefChanged = 0;
    
    //Status Report
    mStatus.SetText("Choosing Reference by Emission...");
    mStatus.SetMaximumSteps(pNewNet.Size());
    mStatus.CheckStatus();

    for(int point = 0; point < pNewNet.Size(); ++point) {
      ControlPoint & newPnt = pNewNet[point];
      const ControlPoint origPnt(newPnt);

      // Stats and Accounting
      iTotalMeasures += newPnt.Size();

      // Logging
      PvlObject pvlPointObj("PointDetails");
      pvlPointObj += Isis::PvlKeyword("PointId", newPnt.Id());

      // Edit Lock Option
      bool bPntEditLock = newPnt.EditLock();
      if(!bPntEditLock) {
        newPnt.SetDateTime(Application::DateTime());
      }

      int iNumMeasuresLocked = newPnt.NumLockedMeasures();
      bool bRefLocked = newPnt.ReferenceLocked();
      
      int iRefIndex = newPnt.ReferenceIndexNoException();
      iString istrTemp;
      
      std::vector <PvlGroup> pvlGrpVector;
      std::vector <double>   bestEmissionAngle;
      int iBestIndex = 0;
      
      // Only perform the interest operation on points of type "Tie" and
      // Points having atleast 1 measure and Points not Ignored
      // Check for EditLock in the Measures and also verfify that
      // only a Reference Measure can be Locked else error
      if(!newPnt.Ignore() && newPnt.Type() == ControlPoint::Tie && iRefIndex >= 0 && 
         (iNumMeasuresLocked == 0 || (iNumMeasuresLocked > 0 && bRefLocked)) ) {
        int iNumIgnore = 0;
        iString istrTemp;
        double dBestEmissionAngle = 135;

        for(int measure = 0; measure < newPnt.Size(); ++measure) {

          ControlMeasure & newMsr = newPnt[measure];
          bool bMeasureLocked = newMsr.EditLock();
          if(!bPntEditLock && !bMeasureLocked) {
            newMsr.SetDateTime(Application::DateTime());
            newMsr.SetChooserName("Application cnetref(Emission)");
          }

          std::string sn = newMsr.CubeSerialNumber();
          double dSample = newMsr.Sample();
          double dLine   = newMsr.Line();

          // Log
          PvlGroup pvlMeasureGrp("MeasureDetails");
          pvlMeasureGrp += Isis::PvlKeyword("SerialNum", sn);
          pvlMeasureGrp += Isis::PvlKeyword("OriginalLocation", LocationString(dSample, dLine));

          if(!newMsr.Ignore()) {
            Cube *measureCube = mCubeMgr.OpenCube(mSerialNumbers.Filename(sn));

            MeasureValidationResults results =
              ValidStandardOptions(dSample, dLine, measureCube, &pvlMeasureGrp);
            if(results.isValid()) {
              if(!bPntEditLock && !bRefLocked) {
                newMsr.SetType(ControlMeasure::Candidate);
                if (mdEmissionAngle < dBestEmissionAngle) {
                  dBestEmissionAngle = mdEmissionAngle;
                  iBestIndex = measure;
                }
              }
            } 
            else {
              if(bPntEditLock) {
                pvlMeasureGrp += Isis::PvlKeyword("UnIgnored", "Failed Validation Test but not Ignored as Point EditLock is True");
              }
              else if(bMeasureLocked) {
                pvlMeasureGrp += Isis::PvlKeyword("UnIgnored", "Failed Validation Test but not Ignored as Measure EditLock is True");
              }
              else {
                pvlMeasureGrp += Isis::PvlKeyword("Ignored",   "Failed Validation Test");
                newMsr.SetIgnore(true);
                iNumIgnore++;
              }
            }
            bestEmissionAngle.push_back(mdEmissionAngle);
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
          if(bPntEditLock) {
            pvlPointObj += Isis::PvlKeyword("UnIgnored", "Good Measures less than 2 but not Ignored as Point EditLock is True");
          }
          else {
            newPnt.SetIgnore(true);
            pvlPointObj += Isis::PvlKeyword("Ignored", "Good Measures less than 2");
          }
        }

        // Set the Reference if the Point is unlocked and Reference measure is unlocked
        if(!newPnt.Ignore() && iBestIndex >= 0 && !newPnt[iBestIndex].Ignore() && !bPntEditLock && !bRefLocked) {
          newPnt[iBestIndex].SetType(ControlMeasure::Reference);
          pvlGrpVector[iBestIndex] += Isis::PvlKeyword("Reference", "true");

          // Log info, if Point not locked, apriori source == Reference and a new reference
          if(iRefIndex != iBestIndex && 
             newPnt.AprioriSurfacePointSource() == ControlPoint::SurfacePointSource::Reference) {
            pvlGrpVector[iBestIndex] += Isis::PvlKeyword("AprioriSource", "Reference is the source and has changed");
          }
        }

        for(int i = 0; i < newPnt.Size(); i++) {
          pvlPointObj += pvlGrpVector[i];
        }
      } // end Tie
      else {
        int iComment = 1;
        if(iRefIndex < 0) {
          std::string sComment = "Comment" + iComment++;
          pvlPointObj += Isis::PvlKeyword(sComment, "No Measures in the Point");
        }
        
        if(newPnt.Ignore()) {
          std::string sComment = "Comment" + iComment++;
          pvlPointObj += Isis::PvlKeyword(sComment, "Point was originally Ignored");
        }
        
        if (newPnt.Type() == ControlPoint::Tie) {
          std::string sComment = "Comment" + iComment++;
          pvlPointObj += Isis::PvlKeyword(sComment, "Not a Tie Point");
        }
        
        if (iNumMeasuresLocked > 0 && !bRefLocked){
          pvlPointObj += Isis::PvlKeyword("Error", "Point has Measure(s) with EditLock set to true but not the Reference");
        }
        
        for(int measure = 0; measure < newPnt.Size(); measure++) {
          newPnt[measure].SetDateTime(Application::DateTime());
          newPnt[measure].SetChooserName("Application cnetref(Emission)");
        }
      }

      if(newPnt != origPnt) {
        iPointsModified++;
      }

      if(!newPnt.Ignore() && iBestIndex != iRefIndex && !bPntEditLock && !bRefLocked) {
        iRefChanged++;
        PvlGroup pvlRefChangeGrp("ReferenceChangeDetails");
        pvlRefChangeGrp += Isis::PvlKeyword("PrevSerialNumber", origPnt[iRefIndex].CubeSerialNumber());
        pvlRefChangeGrp += Isis::PvlKeyword("PrevEmAngle",      bestEmissionAngle[iRefIndex]);

        istrTemp = iString((int)origPnt[iRefIndex].Sample());
        istrTemp += ",";
        istrTemp += iString((int)origPnt[iRefIndex].Line());
        pvlRefChangeGrp += Isis::PvlKeyword("PrevLocation",     istrTemp);

        pvlRefChangeGrp += Isis::PvlKeyword("NewSerialNumber",  newPnt[iBestIndex].CubeSerialNumber());
        pvlRefChangeGrp += Isis::PvlKeyword("NewLeastEmAngle",  bestEmissionAngle[iBestIndex]);

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
};

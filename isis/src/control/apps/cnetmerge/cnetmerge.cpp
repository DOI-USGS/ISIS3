#include "Isis.h"

#include <sstream>
#include <cfloat>

#include "ControlMeasure.h"
#include "ControlNet.h"
#include "ControlPoint.h"
#include "FileList.h"
#include "iException.h"
#include "iTime.h"
#include "Progress.h"

using namespace std;
using namespace Isis;

ControlPoint * MergePoints(ControlPoint *addPoint, ControlPoint *basePoint,
    bool allowMeasureOverride, bool allowReferenceOverride,
    bool &mergeHasConflicts);

// Main program
void IsisMain() {
  // Get user parameters
  UserInterface &ui = Application::GetUserInterface();
  FileList filelist;
  if (ui.GetString("INPUTTYPE") == "LIST") {
    filelist = ui.GetFilename("CLIST");
  }
  else if (ui.GetString("INPUTTYPE") == "CNETS") {
    filelist.push_back(ui.GetFilename("CNET"));
    filelist.push_back(ui.GetFilename("CNET2"));
  }
  Filename outfile(ui.GetFilename("ONET"));

  bool allowPointOverride = false;
  bool allowMeasureOverride = false;
  bool allowReferenceOverride = false;
  if (ui.GetString("DUPLICATEPOINTS") == "MERGE") {
    allowPointOverride = ui.GetBoolean("OVERWRITEPOINTS");
    allowMeasureOverride = ui.GetBoolean("OVERWRITEMEASURES");
    allowReferenceOverride  = ui.GetBoolean("OVERWRITEREFERENCE");
  }

  // Creates a Progress
  Progress progress;
  progress.SetMaximumSteps(filelist.size());
  progress.CheckStatus();

  // Set up the output ControlNet with the first Control Net in the list
  ControlNet baseNet(Filename(filelist[0]).Expanded());
  baseNet.SetNetworkId(ui.GetString("NETWORKID"));
  baseNet.SetUserName(Isis::Application::UserName());
  baseNet.SetCreatedDate(Isis::Application::DateTime());
  baseNet.SetModifiedDate(Isis::iTime::CurrentLocalTime());
  baseNet.SetDescription(ui.GetString("DESCRIPTION"));

  progress.CheckStatus();

  ofstream ss;
  bool report = false;
  if (ui.WasEntered("LOG")) {
    report = true;
    string report = ui.GetFilename("LOG");
    ss.open(report.c_str(), ios::out);
  }

  bool mergePoints = (ui.GetString("DUPLICATEPOINTS") == "MERGE");

  for (int cnetIndex = 1; cnetIndex < (int)filelist.size(); cnetIndex ++) {
    ControlNet addNet(Filename(filelist[cnetIndex]).Expanded());

    // Checks to make sure the ControlNets are valid to merge
    if (baseNet.GetTarget().DownCase() != addNet.GetTarget().DownCase()) {
      string msg = "Input [" + addNet.GetNetworkId() + "] does not target the "
          "same target as other Control Network(s)";
      throw iException::Message(iException::User, msg, _FILEINFO_);
    }

    // Adds currentnet to the ControlNet if it does not exist in cnet
    for (int cp = 0; cp < addNet.GetNumPoints(); cp++) {
      ControlPoint *sourcePoint = addNet.GetPoint(cp);

      // Duplicate point in the input
      ControlPoint *dupPoint = NULL;

      //TODO: This functionality needs to be reworked for the redesign
      try {
        // Find if there is a duplicate point. This will throw an exception if
        //   the control network doesn't have a duplicate point.
        dupPoint = baseNet.GetPoint(QString(sourcePoint->GetId()));
      }
      catch (iException &e) {
        e.Clear();

        // There was no duplicate point so transfer the point directly
        ControlPoint *cp = new ControlPoint(*sourcePoint);
        baseNet.AddPoint(cp);
        // dupPoint should be null if this happened
      }

      if (dupPoint) {
        // We can't merge points, throw an exception!
        if (!mergePoints) {
          string msg = "Inputs contain the same ControlPoint. [Id=";
          msg += sourcePoint->GetId() + "] Set DUPLICATEPOINTS=MERGE to";
          msg += " merge conflicting Control Points.";
          throw iException::Message(iException::User, msg, _FILEINFO_);
        }

        // Default to the duplicate with the merged point; handle more
        //   difficult cases below
        ControlPoint *mergedPoint = dupPoint;

        bool doMergePoint = true;

        if (report) {
          ss << "Control Point " << sourcePoint->GetId() << " was merged from "
             << addNet.GetNetworkId() << endl;
        }

        // Merge the Control Points correctly
        if (dupPoint->GetType() == ControlPoint::Ground &&
            sourcePoint->GetType() == ControlPoint::Ground) {
          // See if there are conflicts in merging the two points
          bool pointHasConflict = false;
          SurfacePoint surfPt(dupPoint->GetAprioriSurfacePoint());
          if (surfPt.Valid() &&
              surfPt == sourcePoint->GetAprioriSurfacePoint()) {
            pointHasConflict = true;
          }

          // Merge the Control Points as best we can
          if (pointHasConflict && !allowPointOverride) {
            doMergePoint = false;

            if (report) {
              ss << "    The merge of Control Point " << sourcePoint->GetId()
                 << " was canceled due to conflicts." << endl;
            }
          }
        }

        if (doMergePoint) {
          bool mergeHasConflicts = false;

          mergedPoint = MergePoints(sourcePoint,
              dupPoint,
              allowMeasureOverride,
              allowReferenceOverride,
              mergeHasConflicts);

          if (report && mergeHasConflicts) {
            ss << "    Control Measures from " << sourcePoint->GetId()
               << " were not merged due to conflicts." << endl;
          }
        }

        dupPoint = NULL;

        baseNet.DeletePoint(sourcePoint->GetId());
        baseNet.AddPoint(mergedPoint);
      }
    }

    progress.CheckStatus();
  }

  // Writes out the final Control Net
  baseNet.Write(outfile.Expanded());
}


ControlPoint *MergePoints(ControlPoint *addPoint, ControlPoint *basePoint,
    bool allowMeasureOverride, bool allowReferenceOverride,
    bool &mergeHasConflicts) {

  // Start with a copy of the base point, which we will attempt to add to in
  // order to create the new merged point
  ControlPoint *mergedPoint = new ControlPoint(*basePoint);

  // Loop through every measure in the add point, attempting to add it to the
  // resulting merged point.  If there are conflicts, attempt to resolve them
  // using rules defined by the program user.
  mergeHasConflicts = false;
  for (int addIndex = 0; addIndex < mergedPoint->GetNumMeasures(); addIndex++) {
    bool merged = false;
    ControlMeasure *addMeasure = addPoint->GetMeasure(addIndex);

    // Try to get the mergee equivalent and merge this control measure in
    try {
      ControlMeasure *mergedMeasure =
        mergedPoint->GetMeasure(addMeasure->GetCubeSerialNumber());

      // If we have found the equivalent control measures in the merger and
      //   mergee
      if (addMeasure->GetCubeSerialNumber() ==
          mergedMeasure->GetCubeSerialNumber()) {
        // If we have a ground truth in our merger then try to propagate it to
        //   the mergee.
        if (addPoint->GetType() == ControlPoint::Ground) {
          if (!allowMeasureOverride) {
            // Allow reference override refers to the merger's reference, not
            //   the mergee's reference. If we can't change the reference then
            //   keep the merger's reference status.
            if (mergedPoint->IsReferenceExplicit() &&
                mergedMeasure != mergedPoint->GetRefMeasure() &&
                addMeasure != addPoint->GetRefMeasure()) {
              ControlMeasure *origReference = mergedPoint->GetRefMeasure();
              origReference->SetType(ControlMeasure::Candidate);
              //mergee.UpdateMeasure(origReference); // Redesign fixed this
            }

            // Copy the rest of merger's information to mergee, mergee will be
            //   a reference since merger is a reference.
            mergedMeasure = addMeasure;
            mergeHasConflicts = true; // lost some information
          }
        }

        // If SNs match, but not ground, then keep mergee's version
        merged = true;
      }

      //mergee.UpdateMeasure(mergeeMeasure); // Redesign fixed this
    }
    catch (iException &e) {
      // No matching serial number was found, we need to pull over this measure
      e.Clear();

      ControlMeasure *newMeasure = addMeasure;

      // We have a new reference
      if (mergedPoint->IsReferenceExplicit() &&
          addPoint->GetRefMeasure() != addMeasure) {
        if (allowReferenceOverride) {
          // Use the new reference
          ControlMeasure *origReference = mergedPoint->GetRefMeasure();
          origReference->SetType(ControlMeasure::Candidate);
          //mergee.UpdateMeasure(origReference); // Redesign fixed this
          // new measure is already a reference since merger is a reference
        }
        else {
          // Don't allow the new point to be a reference
          newMeasure->SetType(ControlMeasure::Candidate);
          mergeHasConflicts = true;
        }
      }

      mergedPoint->Add(newMeasure);
    }
  }

  return mergedPoint;
}

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

ControlPoint *MergePoints(ControlPoint *, ControlPoint *, bool, bool &);

// Main program
void IsisMain() {
  // Get user parameters
  UserInterface &ui = Application::GetUserInterface();
  FileList filelist;
  if(ui.GetString("INPUTTYPE") == "LIST") {
    filelist = ui.GetFilename("FROMLIST");
  }
  else if(ui.GetString("INPUTTYPE") == "CNETS") {
    filelist.push_back(ui.GetFilename("FROM1"));
    filelist.push_back(ui.GetFilename("FROM2"));
  }
  Filename outfile(ui.GetFilename("TO"));

  bool allowPointOverride = false;
  bool allowMeasureOverride = false;
  if(ui.GetString("DUPLICATEPOINTS") == "MERGE") {
    allowPointOverride = ui.GetBoolean("OVERWRITEPOINTS");
    allowMeasureOverride = ui.GetBoolean("OVERWRITEMEASURES");
  }

  // Creates a Progress
  Progress progress;
  progress.SetMaximumSteps(filelist.size());
  progress.CheckStatus();

  // Set up the output ControlNet with the first Control Net in the list
  ControlNet destinationNet(Filename(filelist[0]).Expanded());
  destinationNet.SetNetworkId(ui.GetString("ID"));
  destinationNet.SetUserName(Isis::Application::UserName());
  destinationNet.SetCreatedDate(Isis::Application::DateTime());
  destinationNet.SetModifiedDate(Isis::iTime::CurrentLocalTime());
  destinationNet.SetDescription(ui.GetString("DESCRIPTION"));

  progress.CheckStatus();

  ofstream ss;
  bool report = false;
  if(ui.WasEntered("REPORT")) {
    report = true;
    string report = ui.GetFilename("REPORT");
    ss.open(report.c_str(), ios::out);
  }

  bool mergePoints = (ui.GetString("DUPLICATEPOINTS") == "MERGE");

  for(int cnetIndex = 1; cnetIndex < (int)filelist.size(); cnetIndex ++) {
    ControlNet sourceNet(Filename(filelist[cnetIndex]).Expanded());

    // Checks to make sure the ControlNets are valid to merge
    if(destinationNet.GetTarget() != sourceNet.GetTarget()) {
      string msg = "Input [" + sourceNet.GetNetworkId() + "] does not target the "
                   "same target as other Control Network(s)";
      throw iException::Message(iException::User, msg, _FILEINFO_);
    }

    // Adds currentnet to the ControlNet if it does not exist in cnet
    for(int cp = 0; cp < sourceNet.GetNumPoints(); cp++) {
      ControlPoint *sourcePoint = sourceNet.GetPoint(cp);

      // Duplicate point in the input
      ControlPoint *dupPoint = NULL;

      //TODO: This functionality needs to be reworked for the redesign
      try {
        // Find if there is a duplicate point. This will throw an exception if
        //   the control network doesn't have a duplicate point.
        dupPoint = destinationNet.GetPoint(QString(sourcePoint->GetId()));
      }
      catch(iException &e) {
        e.Clear();

        // There was no duplicate point so transfer the point directly
        destinationNet.AddPoint(sourcePoint);
        // dupPoint should be null if this happened
      }

      if(dupPoint) {
        // We can't merge points, throw an exception!
        if(!mergePoints) {
          string msg = "Inputs contain the same ControlPoint. [Id=";
          msg += sourcePoint->GetId() + "] Set DUPLICATEPOINTS=MERGE to";
          msg += " merge duplicate Control Points.";
          throw iException::Message(iException::User, msg, _FILEINFO_);
        }

        // Default to the duplicate with the merged point; handle more
        //   difficult cases below
        ControlPoint *mergedPoint = dupPoint;

        bool doMergePoint = true;

        if(report) {
          ss << "Control Point " << sourcePoint->GetId() << " was merged from "
             << sourceNet.GetNetworkId() << endl;
        }

        // Merge the Control Points correctly
        if(dupPoint->GetType() == ControlPoint::Ground &&
            sourcePoint->GetType() == ControlPoint::Ground) {
          // See if there are conflicts in merging the two points
          bool pointHasConflict = false;
          SurfacePoint surfPt(dupPoint->GetSurfacePoint());
          if(surfPt.Valid() &&
              dupPoint->GetSurfacePoint() == sourcePoint->GetSurfacePoint()) {
            pointHasConflict = true;
          }

          // Merge the Control Points as best we can
          if(pointHasConflict && !allowPointOverride) {
            doMergePoint = false;

            if(report) {
              ss << "    The merge of Control Point " << sourcePoint->GetId()
                 << " was canceled due to conflicts." << endl;
            }
          }
        }

        if(doMergePoint) {
          bool mergeHasConflicts = false;

          mergedPoint = MergePoints(sourcePoint,
                                    dupPoint,
                                    allowMeasureOverride,
                                    mergeHasConflicts);

          if(report && mergeHasConflicts) {
            ss << "    Control Measures from " << sourcePoint->GetId()
               << " were not merged due to conflicts." << endl;
          }
        }

        dupPoint = NULL;

        destinationNet.DeletePoint(sourcePoint->GetId());
        destinationNet.AddPoint(mergedPoint);
      }
    }

    progress.CheckStatus();
  }

  // Writes out the final Control Net
  destinationNet.Write(outfile.Expanded());
}


ControlPoint *MergePoints(ControlPoint *point1, ControlPoint *point2,
                          bool allowReferenceOverride, bool &mergeHasConflicts) {
  ControlPoint *merger = point1; // Merging from this one
  ControlPoint *mergee = point2;        //   to this one
  mergeHasConflicts = false;

  for(int mergerIndex = 0; mergerIndex < mergee->GetNumMeasures(); mergerIndex ++) {
    bool merged = false;
    ControlMeasure *mergerMeasure = merger->GetMeasure(mergerIndex);

    // Try to get the mergee equivalent and merge this control measure in
    try {
      ControlMeasure *mergeeMeasure =
        mergee->GetMeasure(mergerMeasure->GetCubeSerialNumber());

      // If we have found the equivalent control measures in the merger and
      //   mergee
      if(mergerMeasure->GetCubeSerialNumber() ==
          mergeeMeasure->GetCubeSerialNumber()) {
        // If we have a ground truth in our merger then try to propagate it to
        //   the mergee.
        if(merger->GetType() == ControlPoint::Ground) {
          if(!allowReferenceOverride) {
            // Allow reference override refers to the merger's reference, not
            //   the mergee's reference. If we can't change the reference then
            //   keep the merger's reference status.
            if(mergeeMeasure->GetType() != ControlMeasure::Reference &&
                mergerMeasure->GetType() == ControlMeasure::Reference &&
                mergee->HasReference()) {
              ControlMeasure *origReference =
                mergee->GetMeasure(mergee->GetReferenceIndex());
              origReference->SetType(ControlMeasure::Candidate);
              //mergee.UpdateMeasure(origReference); // Redesign fixed this
            }

            // Copy the rest of merger's information to mergee, mergee will be
            //   a reference since merger is a reference.
            mergeeMeasure = mergerMeasure;
            mergeHasConflicts = true; // lost some information
          }
        }

        // If SNs match, but not ground, then keep mergee's version
        merged = true;
      }

      //mergee.UpdateMeasure(mergeeMeasure); // Redesign fixed this
    }
    catch(iException &e) {
      // No matching serial number was found, we need to pull over this measure
      e.Clear();

      ControlMeasure *newMeasure = mergerMeasure;

      // We have a new reference
      if(mergee->HasReference() &&
          mergerMeasure->GetType() == ControlMeasure::Reference) {
        if(allowReferenceOverride) {
          // Use the new reference
          ControlMeasure *origReference =
            mergee->GetMeasure(mergee->GetReferenceIndex());
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

      mergee->Add(newMeasure);
    }
  }

  return mergee;
}

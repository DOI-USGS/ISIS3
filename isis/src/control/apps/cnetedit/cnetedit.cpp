#define GUIHELPERS

#include "Isis.h"

#include "ControlMeasure.h"
#include "ControlPoint.h"
#include "ControlNet.h"
#include "ControlNetValidMeasure.h"
#include "ControlPointList.h"
#include "Cube.h"
#include "iException.h"
#include "MeasureValidationResults.h"
#include "Progress.h"
#include "Pvl.h"
#include "PvlObject.h"
#include "PvlGroup.h"
#include "PvlKeyword.h"
#include "SerialNumberList.h"

using namespace std;
using namespace Isis;

void DeletePoint(ControlNet & cnet, int cp);
void DeleteMeasure(ControlNet & cnet, int cp, int cm);
void PopulateLog(ControlNet & cnet);
void ProcessControlPoints(std::string psFileName, ControlNet &pcCnet);
void ProcessControlMeasures(std::string psFileName, ControlNet &pcCnet);
void CheckAllMeasureValidity(ControlNet & cnet, std::string cubeList);
MeasureValidationResults InvalidMeasure(ControlMeasure & curMeasure, std::string filename);

void PrintTemp();

map <string, void *> GuiHelpers() {
  map <string, void *> helper;
  helper ["PrintTemp"] = (void *) PrintTemp;
  return helper;
}

int numPointsDeleted;
int numMeasuresDeleted;

bool deleteIgnored;
bool preservePoints;

bool keepLog;
Pvl * editLog;

ControlNetValidMeasure * validator;


// Main program
void IsisMain() {

  // Reset the counts of points and measures deleted
  numPointsDeleted = 0;
  numMeasuresDeleted = 0;

  // Interface for getting user parameters
  UserInterface &ui = Application::GetUserInterface();

  // Get global user parameters
  deleteIgnored = ui.GetBoolean("DELETE");
  preservePoints = ui.GetBoolean("PRESERVE");

  keepLog = ui.WasEntered("LOG");
  editLog = NULL;

  ControlNet cnet(ui.GetFilename("CNET"));
  if(keepLog)
    PopulateLog(cnet);

  /*
   * As a first pass, just try and delete anything that's already ignored
   * in the Control Network, if the user wants to delete ignored points and
   * measures.  Originally, this check was performed last, only if the user
   * didn't specify any other deletion methods.  However, performing this
   * check first will actually improve the running time in cases where there
   * are already ignored points and measures in the input network.  The added
   * cost of doing this check here actually doesn't add to the running time at
   * all, because these same checks would need to have been done later
   * regardless.
   */
  if(deleteIgnored) {
    for(int cp = cnet.Size() - 1; cp >= 0; cp--) {
      if(cnet[cp].Ignore()) {
        DeletePoint(cnet, cp);
      }
      else {
        for(int cm = cnet[cp].Size() - 1; cm >= 0; cm--) {
          if(cnet[cp][cm].Ignore()) {
            if(cm == cnet[cp].ReferenceIndex()) {
              // If the reference is ignored, the point must ignored too
              cnet[cp].SetIgnore(true);
            }
            else {
              // Can't delete the reference without deleting the whole point
              DeleteMeasure(cnet, cp, cm);
            }
          }
        }

        // Check if the number of measures in the point is zero or there are too
        // few measures in the point and we don't want to preserve them.
        if(((cnet[cp].Size() < 2 && !preservePoints) && cnet[cp].Type() != ControlPoint::Ground)
            || cnet[cp].Size() == 0 || cnet[cp].Ignore()) {
          DeletePoint(cnet, cp);
        }
      }
    }
  }

  //List has Points Ids
  if(ui.WasEntered("POINTLIST")) {
    std::string pointlistFilename = ui.GetFilename("POINTLIST");
    ProcessControlPoints(pointlistFilename, cnet);
  }

  //List has Cube file names
  if(ui.WasEntered("CUBELIST")) {
    std::string ignorelistFilename = ui.GetFilename("CUBELIST");
    ProcessControlMeasures(ignorelistFilename, cnet);
  }

  // Perform validity check
  if(ui.GetBoolean("CHECKVALID")) {
    validator = NULL;

    Pvl defFile(ui.GetFilename("DEFFILE"));
    validator = new ControlNetValidMeasure(&defFile);

    std::string cubeList = ui.GetFilename("FROMLIST");
    CheckAllMeasureValidity(cnet, cubeList);

    if(validator != NULL) {
      delete validator;
      validator = NULL;
    }

    // Log the DEFFILE to the print file
    Application::Log(defFile.FindGroup("ValidMeasure", Pvl::Traverse));
  }

  // Log statistics
  if(keepLog) {
    editLog->AddKeyword(PvlKeyword("PointsDeleted", numPointsDeleted));
    editLog->AddKeyword(PvlKeyword("MeasuresDeleted", numMeasuresDeleted));

    std::string logFilename = ui.GetFilename("LOG");
    editLog->Write(logFilename);

    if(editLog != NULL) {
      delete editLog;
      editLog = NULL;
    }
  }

  cnet.Write(ui.GetFilename("ONET"));
}


void DeletePoint(ControlNet & cnet, int cp) {
  if (keepLog) {
    string id = cnet[cp].Id();
    editLog->FindObject(id).AddKeyword(
        PvlKeyword("Deleted", "true"));

    for(int cm = cnet[cp].Size() - 1; cm >= 0; cm--) {
      if (!editLog->Object(cp).Group(cm).HasKeyword("Deleted")) {
        editLog->Object(cp).Group(cm).AddKeyword(
            PvlKeyword("Deleted", "true"));
      }
    }
  }

  numMeasuresDeleted += cnet[cp].Size();
  cnet.Delete(cp);
  numPointsDeleted++;
}


void DeleteMeasure(ControlNet & cnet, int cp, int cm) {
  if (keepLog) {
    string id = cnet[cp].Id();
    string serial = cnet[cp][cm].CubeSerialNumber();
    editLog->FindObject(id).FindGroup(serial).AddKeyword(
        PvlKeyword("Deleted", "true"));
  }

  cnet[cp].Delete(cm);
  numMeasuresDeleted++;
}


void PopulateLog(ControlNet & cnet) {
  editLog = new Pvl;

  for(int cp = 0; cp < cnet.Size(); cp++) {
    PvlObject pointInfo(cnet[cp].Id());
    pointInfo.AddKeyword(PvlKeyword("PointId", cnet[cp].Id()));

    if(cnet[cp].Ignore()) {
      pointInfo.AddKeyword(PvlKeyword("Ignored", "From input"));
    }

    for(int cm = 0; cm < cnet[cp].Size(); cm++) {
      PvlGroup measureInfo(cnet[cp][cm].CubeSerialNumber());
      measureInfo.AddKeyword(
          PvlKeyword("SerialNumber", cnet[cp][cm].CubeSerialNumber()));

      if(cm == cnet[cp].ReferenceIndex()) {
        measureInfo.AddKeyword(
            PvlKeyword("Reference", "true"));
      }

      if(cnet[cp][cm].Ignore()) {
        if(cm == cnet[cp].ReferenceIndex()) {
          // If the reference is ignored, the point must ignored too
          if(!cnet[cp].Ignore()) {
            cnet[cp].SetIgnore(true);
            pointInfo.AddKeyword(
                PvlKeyword("Ignored", "Reference measure ignored"));
          }
        }

        measureInfo.AddKeyword(
            PvlKeyword("Ignored", "From input"));
      }

      pointInfo.AddGroup(measureInfo);
    }

    editLog->AddObject(pointInfo);
  }
}


/**
 * Reads the Control Points list and matches with the control
 * network. If match was successful, ignore the point. If
 * Delete option was chosen, delete the point
 *
 * @param psFileName - Filename with Control Points
 * @param pcCnet     - holds the input Control Network
 * @param pcPvlLog   - Pvl for which control points stats have
 *                     to be added
 *
 * @return none
 */
void ProcessControlPoints(std::string psFileName, ControlNet &pcCnet) {
  ControlPointList cpList(psFileName);

  for(int cp = pcCnet.Size() - 1; cp >= 0; cp --) {

    // Compare each Point Id listed with the Point in the
    // Control Network for according exclusion
    if(!pcCnet[cp].Ignore() && cpList.HasControlPoint(pcCnet[cp].Id())) {
      pcCnet[cp].SetIgnore(true);

      if(keepLog) {
        editLog->Object(cp).AddKeyword(
          PvlKeyword("Ignored", "Point ID in POINTLIST"));
      }
    }

    if(deleteIgnored) {
      //look for previously ignored control points
      if(pcCnet[cp].Ignore()) {
        DeletePoint(pcCnet, cp);
      }
      else {
        //look for previously ignored control measures
        for(int cm = pcCnet[cp].Size() - 1; cm >= 0; cm--) {
          if(pcCnet[cp][cm].Ignore() && deleteIgnored) {
            DeleteMeasure(pcCnet, cp, cp);
          }
        }
        // Check if there are too few measures in the point or the point was previously ignored
        if(((pcCnet[cp].Size() < 2 && !preservePoints) && pcCnet[cp].Type() != ControlPoint::Ground)
            || pcCnet[cp].Size() == 0 || (pcCnet[cp].Ignore() && deleteIgnored)) {
          DeletePoint(pcCnet, cp);
        }
      }
    }
  }
}

/**
 * Reads the Cube file list and creates the serial number of the
 * Cubes. If Control Measure serial# matches with the control
 * network,ignore the point. If Delete option was chosen, delete
 * the Measure
 *
 * @param psFileName - Filename with Cube File names
 * @param pcCnet     - holds the input Control Network
 *
 * @return none
 */
void ProcessControlMeasures(std::string psFileName, ControlNet &pcCnet) {
  SerialNumberList snl = psFileName;

  for(int cp = pcCnet.Size() - 1; cp >= 0; cp --) {

    // Compare each Serial Number listed with the serial number in the
    // Control Measure for according exclusion
    for(int cm = pcCnet[cp].Size() - 1; cm >= 0 ; cm--) {
      std::string serialNumber = pcCnet[cp][cm].CubeSerialNumber();
      if(!pcCnet[cp][cm].Ignore() && snl.HasSerialNumber(serialNumber)) {
        pcCnet[cp][cm].SetIgnore(true);

        if(keepLog) {
          editLog->Object(cp).Group(cm).AddKeyword(
            PvlKeyword("Ignored", "Serial Number in CUBELIST"));
        }

        if(cm == pcCnet[cp].ReferenceIndex() && !pcCnet[cp].Ignore()) {
          pcCnet[cp].SetIgnore(true);

          if(keepLog) {
            editLog->Object(cp).AddKeyword(
                PvlKeyword("Ignored", "Reference measure ignored"));
          }
        }
      }

      //also look for previously ignored control measures
      if(deleteIgnored && pcCnet[cp][cm].Ignore() && cm != pcCnet[cp].ReferenceIndex()) {
        DeleteMeasure(pcCnet, cp, cm);
      }
    }
    // Check if there are too few measures in the point or the point was previously ignored
    if(((pcCnet[cp].Size() < 2 && !preservePoints) && pcCnet[cp].Type() != ControlPoint::Ground)
        || pcCnet[cp].Size() == 0 || (pcCnet[cp].Ignore() && deleteIgnored)) {
      DeletePoint(pcCnet, cp);
    }
  }
}


void CheckAllMeasureValidity(ControlNet & cnet, std::string cubeList) {
  SerialNumberList serialNumbers(cubeList);

  Progress progress;
  progress.SetText("Checking Measure Validity");
  progress.SetMaximumSteps(cnet.Size());
  progress.CheckStatus();

  for(int cp = cnet.Size() - 1; cp >= 0; cp--) {

    // Compare each Serial Number listed with the serial number in the
    // Control Measure for according exclusion
    for(int cm = cnet[cp].Size() - 1; cm >= 0; cm--) {

      std::string serialNumber = cnet[cp][cm].CubeSerialNumber();
      if(!serialNumbers.HasSerialNumber(serialNumber)) {
        std::string msg = "Serial Number [" + serialNumber + "] contains no ";
        msg += "matching cube in FROMLIST [" + cubeList + "]";
        throw iException::Message(iException::User, msg, _FILEINFO_);
      }

      if(!cnet[cp][cm].Ignore()) {
        MeasureValidationResults results = 
          InvalidMeasure(cnet[cp][cm], serialNumbers.Filename(serialNumber));
        if(!results.isValid()) {
          cnet[cp][cm].SetIgnore(true);

          if(keepLog) {
            editLog->Object(cp).Group(cm).AddKeyword(
                PvlKeyword("Ignored", "Validity Check " + results.toString()));
          }

          if(cm == cnet[cp].ReferenceIndex()) {
            cnet[cp].SetIgnore(true);

            if(keepLog && !cnet[cp].Ignore()) {
              editLog->Object(cp).AddKeyword(
                  PvlKeyword("Ignored", "Reference measure ignored"));
            }
          }
        }
      }

      //also look for previously ignored control measures
      if(deleteIgnored && cnet[cp][cm].Ignore() && cm != cnet[cp].ReferenceIndex()) {
        DeleteMeasure(cnet, cp, cm);
      }
    }

    // Check if there are too few measures in the point or the point was previously ignored
    if(((cnet[cp].Size() < 2 && !preservePoints) && cnet[cp].Type() != ControlPoint::Ground)
        || cnet[cp].Size() == 0 || (cnet[cp].Ignore() && deleteIgnored)) {
      DeletePoint(cnet, cp);
    }

    progress.CheckStatus();
  }
}


MeasureValidationResults InvalidMeasure(ControlMeasure & curMeasure, std::string cubeName) {
  Cube curCube;
  curCube.Open(cubeName);

  MeasureValidationResults results = validator->ValidStandardOptions(
      curMeasure.Sample(), curMeasure.Line(), &curCube);

  // TODO we want to output this to the log, but the validator is not yet able
  // to do this easily

  return results;
}


// Helper function to print out template to session log
void PrintTemp() {
  UserInterface &ui = Application::GetUserInterface();

  // Get template pvl
  Pvl userTemp;
  userTemp.Read(ui.GetFilename("DEFFILE"));

  //Write template file out to the log
  Isis::Application::GuiLog(userTemp);
}

#define GUIHELPERS

#include "Isis.h"

#include "ControlNet.h"
#include "SerialNumberList.h"
#include "ControlPointList.h"
#include "ControlNetValidMeasure.h"
#include "iException.h"

using namespace std;
using namespace Isis;

void ProcessControlPoints(std::string psFileName, ControlNet &pcCnet, Pvl &pcPvlLog);
void ProcessControlMeasures(std::string psFileName, ControlNet &pcCnet);
void CheckAllMeasureValidity(ControlNet & cnet, std::string cubeList);
bool InvalidMeasure(ControlMeasure & curMeasure, std::string filename);

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

  // Only default options were chosen, no special ignore lists or checks
  bool useDefaultOptions = true;

  //List has Points Ids
  ControlNet cnet(ui.GetFilename("CNET"));
  Pvl pvlLog;
  if(ui.WasEntered("POINTLIST")) {
    useDefaultOptions = false;
    std::string pointlistFilename = ui.GetFilename("POINTLIST");
    ProcessControlPoints(pointlistFilename, cnet, pvlLog);
  }

  //List has Cube file names
  if(ui.WasEntered("CUBELIST")) {
    useDefaultOptions = false;
    std::string ignorelistFilename = ui.GetFilename("CUBELIST");
    ProcessControlMeasures(ignorelistFilename, cnet);
  }

  if(ui.GetBoolean("CHECKVALID")) {
    useDefaultOptions = false;

    validator = NULL;

    Pvl defFile(ui.GetFilename("DEFFILE"));
    validator = new ControlNetValidMeasure(&defFile);

    std::string cubeList = ui.GetFilename("FROMLIST");
    CheckAllMeasureValidity(cnet, cubeList);

    if(validator != NULL) {
      delete validator;
      validator = NULL;
    }
  }

  //No List files - only Delete option was chosen
  if(useDefaultOptions && deleteIgnored) {
    for(int cp = cnet.Size() - 1; cp >= 0; cp --) {
      if(cnet[cp].Ignore()) {
        numMeasuresDeleted += cnet[cp].Size();
        cnet.Delete(cp);
        numPointsDeleted++;
      }
      else {
        for(int cm = cnet[cp].Size() - 1; cm >= 0; cm --) {
          if(cnet[cp][cm].Ignore()) {
            cnet[cp].Delete(cm);
            numMeasuresDeleted++;
          }
        }

        // Check if the number of measures in the point is zero or
        // there are few measures in the point and preserve flag is false.
        if(((cnet[cp].Size() < 2 && !preservePoints) && cnet[cp].Type() != ControlPoint::Ground)
            || cnet[cp].Size() == 0) {
          numMeasuresDeleted += cnet[cp].Size();
          cnet.Delete(cp);
          numPointsDeleted++;
        }
      }
    }
  }

  //log statistics
  if(keepLog) {
    pvlLog += Isis::PvlKeyword("PointsDeleted", numPointsDeleted);
    pvlLog += Isis::PvlKeyword("MeasuresDeleted", numMeasuresDeleted);

    std::string logFilename = ui.GetFilename("LOG");
    pvlLog.Write(logFilename);
  }

  cnet.Write(ui.GetFilename("ONET"));
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
void ProcessControlPoints(std::string psFileName, ControlNet &pcCnet, Pvl &pcPvlLog) {
  ControlPointList cpList(psFileName);

  for(int cp = pcCnet.Size() - 1; cp >= 0; cp --) {

    // Compare each Point Id listed with the Point in the
    // Control Network for according exclusion
    if(cpList.HasControlPoint(pcCnet[cp].Id())) {
      pcCnet[cp].SetIgnore(true);
    }

    if(deleteIgnored) {
      //look for previously ignored control points
      if(pcCnet[cp].Ignore()) {
        numMeasuresDeleted += pcCnet[cp].Size();
        pcCnet.Delete(cp);
        numPointsDeleted++;
      }
      else {
        //look for previously ignored control measures
        for(int cm = pcCnet[cp].Size() - 1; cm >= 0; cm--) {
          if(pcCnet[cp][cm].Ignore() && deleteIgnored) {
            pcCnet[cp].Delete(cm);
            numMeasuresDeleted++;
          }
        }
        // Check if there are too few measures in the point or the point was previously ignored
        if(((pcCnet[cp].Size() < 2 && !preservePoints) && pcCnet[cp].Type() != ControlPoint::Ground)
            || pcCnet[cp].Size() == 0 || (pcCnet[cp].Ignore() && deleteIgnored)) {
          numMeasuresDeleted += pcCnet[cp].Size();
          pcCnet.Delete(cp);
          numPointsDeleted++;
        }
      }
    }
  }
  if(keepLog) {
    cpList.RegisterStatistics(pcPvlLog);
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
      if(snl.HasSerialNumber(serialNumber)) {
        pcCnet[cp][cm].SetIgnore(true);
      }

      //also look for previously ignored control measures
      if(deleteIgnored && pcCnet[cp][cm].Ignore()) {
        pcCnet[cp].Delete(cm);
        numMeasuresDeleted++;
      }
    }
    // Check if there are too few measures in the point or the point was previously ignored
    if(((pcCnet[cp].Size() < 2 && !preservePoints) && pcCnet[cp].Type() != ControlPoint::Ground)
        || pcCnet[cp].Size() == 0 || (pcCnet[cp].Ignore() && deleteIgnored)) {
      numMeasuresDeleted += pcCnet[cp].Size();
      pcCnet.Delete(cp);
      numPointsDeleted++;
    }
  }
}


void CheckAllMeasureValidity(ControlNet & cnet, std::string cubeList) {
  SerialNumberList serialNumbers(cubeList);

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

      if(InvalidMeasure(cnet[cp][cm], serialNumbers.Filename(serialNumber))) {
        cnet[cp][cm].SetIgnore(true);
      }

      //also look for previously ignored control measures
      if(deleteIgnored && cnet[cp][cm].Ignore()) {
        cnet[cp].Delete(cm);
        numMeasuresDeleted++;
      }
    }

    // Check if there are too few measures in the point or the point was previously ignored
    if(((cnet[cp].Size() < 2 && !preservePoints) && cnet[cp].Type() != ControlPoint::Ground)
        || cnet[cp].Size() == 0 || (cnet[cp].Ignore() && deleteIgnored)) {
      numMeasuresDeleted += cnet[cp].Size();
      cnet.Delete(cp);
      numPointsDeleted++;
    }
  }
}


bool InvalidMeasure(ControlMeasure & curMeasure, std::string cubeName) {
  Cube curCube;
  curCube.Open(cubeName);

  // TODO we want to output this to the log, but the validator is not yet able
  // to do this easily

  return !validator->ValidStandardOptions(
      curMeasure.Sample(), curMeasure.Line(), &curCube);
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

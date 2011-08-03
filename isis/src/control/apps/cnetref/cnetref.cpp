#define GUIHELPERS // should always be the first command

#include "Isis.h"

#include "CnetRefByEmission.h"
#include "CnetRefByIncidence.h"
#include "CnetRefByResolution.h"
#include "ControlNet.h"
#include "ControlNetStatistics.h"
#include "GuiEditFile.h"
#include "InterestOperatorFactory.h"
#include "InterestOperator.h"
#include "iException.h"
#include "Progress.h"
#include "Pvl.h"

#include <string.h>

using namespace std;
using namespace Isis;

void ViewDefFile();
void EditDefFile();

map <string, void *> GuiHelpers() {
  map <string, void *> helper;
  helper ["View"] = (void *)ViewDefFile;
  helper ["Edit"] = (void *)EditDefFile;
  return helper;
}

ResolutionType GetResolutionType(std::string psType);

void IsisMain() {

  try {
    UserInterface &ui = Application::GetUserInterface();
    std::string sSerialNumFile = ui.GetFilename("FROMLIST");

    // get the Criteria option
    std::string sCriteria = ui.GetString("CRITERIA");

    // Check format of Pvl DefFile
    bool bDefFile = false;
    Pvl *pvlDefFile = 0;
    Pvl pvlTemplate, pvlResults;
    if (ui.WasEntered("DEFFILE")) {
      bDefFile = true;
      pvlDefFile = new Pvl(ui.GetFilename("DEFFILE"));

      // Log the DefFile
      Application::Log(pvlDefFile->Group(0));

      if (pvlDefFile->Group(0).HasKeyword("PixelsFromEdge") && pvlDefFile->Group(0).HasKeyword("MetersFromEdge")) {
        string message = "DefFile Error : Cannot have both \"PixelsFromEdge\" && \"MetersFromEdge\"" ;
        throw Isis::iException::Message(Isis::iException::User, message, _FILEINFO_);
      }

      Pvl pvlTemplate, pvlResults;
      if (sCriteria == "INTEREST") {
        pvlTemplate = Pvl("$ISIS3DATA/base/templates/cnetref/cnetref_operator.def");
      }
      else {
        pvlTemplate = Pvl("$ISIS3DATA/base/templates/cnetref/cnetref_nooperator.def");
      }
      pvlTemplate.ValidatePvl(*pvlDefFile, pvlResults);
      if (pvlResults.Groups() > 0 || pvlResults.Keywords() > 0) {
        Application::Log(pvlResults.Group(0));
        string sErrMsg = "Invalid Deffile\n";
        throw Isis::iException::Message(Isis::iException::User, sErrMsg, _FILEINFO_);
      }
    }

    // Get the original control net internalized
    Progress progress;
    ControlNet cNet(ui.GetFilename("CNET"), &progress);

    if (ui.WasEntered("NETWORKID")) {
      cNet.SetNetworkId(ui.GetString("NETWORKID"));
    }

    cNet.SetUserName(Isis::Application::UserName());

    if (ui.WasEntered("DESCRIPTION")) {
      cNet.SetDescription(ui.GetString("DESCRIPTION"));
    }

    // Get the output Log file
    bool bLogFile = false;
    string sLogFile;
    if (ui.WasEntered("LOG")) {
      sLogFile = ui.GetFilename("LOG");
      bLogFile = true;
    }

    // get the Criteria option
    ControlNetValidMeasure *cnetValidMeas = NULL;

    // Process Reference by Emission Angle
    if (sCriteria == "EMISSION") {
      cnetValidMeas = new CnetRefByEmission(pvlDefFile, sSerialNumFile);
      cnetValidMeas->FindCnetRef(cNet);
    }

    // Process Reference by Incidence Angle
    else if (sCriteria == "INCIDENCE") {
      cnetValidMeas = new CnetRefByIncidence(pvlDefFile, sSerialNumFile);
      cnetValidMeas->FindCnetRef(cNet);
    }

    // Process Reference by Resolution
    else if (sCriteria == "RESOLUTION") {
      std::string sType = ui.GetString("TYPE");
      double dResValue = 0;
      double dMinRes = 0, dMaxRes = 0;
      if (sType == "NEAREST") {
        dResValue = ui.GetDouble("RESVALUE");
        if (dResValue < 0) {
          std::string message = "Invalid Nearest Resolution Value";
          throw Isis::iException::Message(Isis::iException::User, message, _FILEINFO_);
        }
      }
      else if (sType == "RANGE") {
        dMinRes = ui.GetDouble("MINRES");
        dMaxRes = ui.GetDouble("MAXRES");
        if (dMinRes < 0 || dMaxRes < 0 || dMinRes > dMaxRes) {
          std::string message = "Invalid Resolution Range";
          throw Isis::iException::Message(Isis::iException::User, message, _FILEINFO_);
        }
      }
      cnetValidMeas = new CnetRefByResolution(pvlDefFile, sSerialNumFile, GetResolutionType(sType), dResValue, dMinRes, dMaxRes);
      cnetValidMeas->FindCnetRef(cNet);
    }

    // Process Reference by Interest
    else if (sCriteria == "INTEREST") {
      if (!bDefFile) {
        std::string msg = "Interest Option must have a DefFile";
        throw Isis::iException::Message(Isis::iException::User, msg, _FILEINFO_);
      }
      std::string sOverlapListFile = "";
      if (ui.WasEntered("LIMIT")) {
        if (ui.GetBoolean("LIMIT")) {
          sOverlapListFile = Filename(ui.GetFilename("OVERLAPLIST")).Expanded();
        }
      }

      // Get the InterestOperator set up
      InterestOperator *interestOp = InterestOperatorFactory::Create(*pvlDefFile);
      interestOp->Operate(cNet, sSerialNumFile, sOverlapListFile);

      // Write to print.prt and screen interest details
      // add operator to print.prt
      PvlGroup opGroup = interestOp->Operator();
      Application::Log(opGroup);
      if (bLogFile) {
        Pvl pvlLog = interestOp->GetLogPvl();
        pvlLog += opGroup;
        pvlLog.Write(sLogFile);
      }
      Application::Log(interestOp->GetStdOptions());
      Application::Log(interestOp->GetStatistics());
    }
    
    // Write the new control network out
    cNet.Write(ui.GetFilename("ONET"));

    // Get Control Net Stats Summary
    PvlGroup statsGrp;
    ControlNetStatistics cnetStats(&cNet);
    cnetStats.GenerateControlNetStats(statsGrp);
    Application::Log(statsGrp);
    
    if (cnetValidMeas) {
      Pvl pvlLog = cnetValidMeas->GetLogPvl();
      if (bLogFile) {
        pvlLog.Write(sLogFile);
      }
      Application::Log(cnetValidMeas->GetStdOptions());
      Application::Log(cnetValidMeas->GetStatistics());

      // clean up
      delete cnetValidMeas;
    }

  } // REFORMAT THESE ERRORS INTO ISIS TYPES AND RETHROW
  catch (Isis::iException &e) {
    throw;
  }
  catch (geos::util::GEOSException *exc) {
    string message = "GEOS Exception: " + (iString)exc->what();
    delete exc;
    throw Isis::iException::Message(Isis::iException::User, message, _FILEINFO_);
  }
  catch (std::exception const &se) {
    string message = "std::exception: " + (iString)se.what();
    throw Isis::iException::Message(Isis::iException::User, message, _FILEINFO_);
  }
  catch (...) {
    string message = "Other Error";
    throw Isis::iException::Message(Isis::iException::User, message, _FILEINFO_);
  }
}

/**
 * Return the enumerated ResolutionType for a given string
 *
 * @author Sharmila Prasad (6/4/2010)
 *
 * @param psType
 *
 * @return ResolutionType
 */
ResolutionType GetResolutionType(std::string psType) {
  if (psType == "LOW")
    return Low;
  else if (psType == "HIGH")
    return High;
  else if (psType == "MEAN")
    return Mean;
  else if (psType == "NEAREST")
    return Nearest;
  else if (psType == "RANGE")
    return Range;

  return High;
}

/**
 * Helper function to print out template to log.
 * 
 * @author Sharmila Prasad (5/24/2011)
 */
void ViewDefFile() {
  UserInterface &ui = Application::GetUserInterface();

  // Get template PVL
  Pvl defFile;
  defFile.Read(ui.GetFilename("DEFFILE"));

  // Write deffile file out to the log
  Isis::Application::GuiLog(defFile);
}

/**
 * Helper function to be able to edit the Deffile. 
 * Opens an editor to edit the file. 
 * 
 * @author Sharmila Prasad (5/23/2011)
 */
void EditDefFile(void) {
  UserInterface &ui = Application::GetUserInterface();
  string sDefFile = ui.GetAsString("DEFFILE");
  
  GuiEditFile::EditFile(ui, sDefFile);
}


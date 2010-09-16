#include "Isis.h"

#include "Pvl.h"
#include "ControlNet.h"
#include "Progress.h"
#include "InterestOperatorFactory.h"
#include "InterestOperator.h"
#include "iException.h"

#include "CnetRefByEmission.h"
#include "CnetRefByIncidence.h"
#include "CnetRefByResolution.h"

#include <string.h>

using namespace std;
using namespace Isis;

ResolutionType GetResolutionType(std::string psType);

void IsisMain() {

  try {
    UserInterface &ui = Application::GetUserInterface();
    std::string sSerialNumFile = ui.GetFilename("FROMLIST");

    // Get the original control net internalized
    Progress progress;
    ControlNet origNet(ui.GetFilename("NETWORK"), &progress);

    // Create the new control net to store the points in.
    ControlNet newNet;
    newNet.SetType(origNet.Type());
    newNet.SetTarget(origNet.Target());
    if(ui.WasEntered("NETWORKID")) {
      newNet.SetNetworkId(ui.GetString("NETWORKID"));
    }
    else {
      newNet.SetNetworkId(origNet.NetworkId());
    }

    newNet.SetUserName(Isis::Application::UserName());

    if(ui.WasEntered("DESCRIPTION")) {
      newNet.SetDescription(ui.GetString("DESCRIPTION"));
    }
    else {
      newNet.SetDescription(origNet.Description());
    }

    bool bDefFile = false;
    Pvl *pvlDefFile = 0;
    if(ui.WasEntered("DEFFILE")) {
      bDefFile = true;
      pvlDefFile = new Pvl(ui.GetFilename("DEFFILE"));
    }

    // Get the output Log file
    bool bLogFile = false;
    string sLogFile;
    if(ui.WasEntered("LOG")) {
      sLogFile = ui.GetFilename("LOG");
      bLogFile = true;
    }

    // get the Criteria option
    ControlNetValidMeasure *cnetValidMeas = NULL;
    std::string sCriteria = ui.GetString("CRITERIA");

    // Process Reference by Emission Angle
    if(sCriteria == "EMISSION") {
      cnetValidMeas = new CnetRefByEmission(pvlDefFile, sSerialNumFile);
      cnetValidMeas->FindCnetRef(origNet, newNet);
    }

    // Process Reference by Incidence Angle
    else if(sCriteria == "INCIDENCE") {
      cnetValidMeas = new CnetRefByIncidence(pvlDefFile, sSerialNumFile);
      cnetValidMeas->FindCnetRef(origNet, newNet);
    }

    // Process Reference by Resolution
    else if(sCriteria == "RESOLUTION") {
      std::string sType = ui.GetString("TYPE");
      double dResValue = 0;
      double dMinRes = 0, dMaxRes = 0;
      if(sType == "NEAREST") {
        dResValue = ui.GetDouble("RESVALUE");
        if(dResValue < 0) {
          std::string message = "Invalid Nearest Resolution Value";
          throw Isis::iException::Message(Isis::iException::User, message, _FILEINFO_);
        }
      }
      else if(sType == "RANGE") {
        dMinRes = ui.GetDouble("MINRES");
        dMaxRes = ui.GetDouble("MAXRES");
        if(dMinRes < 0 || dMaxRes < 0 || dMinRes > dMaxRes) {
          std::string message = "Invalid Resolution Range";
          throw Isis::iException::Message(Isis::iException::User, message, _FILEINFO_);
        }
      }
      cnetValidMeas = new CnetRefByResolution(pvlDefFile, sSerialNumFile, GetResolutionType(sType), dResValue, dMinRes, dMaxRes);
      cnetValidMeas->FindCnetRef(origNet, newNet);
    }

    // Process Reference by Interest
    else if(sCriteria == "INTEREST") {
      if(!bDefFile) {
        std::string msg = "Interest Option must have a DefFile";
        throw Isis::iException::Message(Isis::iException::User, msg, _FILEINFO_);
      }
      std::string sOverlapListFile = "";
      if(ui.WasEntered("LIMIT")) {
        if(ui.GetBoolean("LIMIT")) {
          sOverlapListFile = Filename(ui.GetFilename("OVERLAPLIST")).Expanded();
        }
      }

      // Get the InterestOperator set up
      InterestOperator *interestOp = InterestOperatorFactory::Create(*pvlDefFile);
      interestOp->Operate(origNet, newNet, sSerialNumFile, sOverlapListFile);

      // Write to print.prt and screen interest details
      // add operator to print.prt
      PvlGroup opGroup = interestOp->Operator();
      Application::Log(opGroup);
      if(bLogFile) {
        Pvl pvlLog = interestOp->GetLogPvl();
        pvlLog += opGroup;
        pvlLog.Write(sLogFile);
      }
      Application::Log(interestOp->GetStdOptions());
      Application::Log(interestOp->GetStatistics());
    }

    // Write the new control network out
    newNet.Write(ui.GetFilename("TO"));

    if(cnetValidMeas) {
      Pvl pvlLog = cnetValidMeas->GetLogPvl();
      if(bLogFile) {
        pvlLog.Write(sLogFile);
      }
      Application::Log(cnetValidMeas->GetStdOptions());
      Application::Log(cnetValidMeas->GetStatistics());

      // clean up
      delete cnetValidMeas;
    }

  } // REFORMAT THESE ERRORS INTO ISIS TYPES AND RETHROW
  catch(Isis::iException &e) {
    throw;
  }
  catch(geos::util::GEOSException *exc) {
    string message = "GEOS Exception: " + (iString)exc->what();
    delete exc;
    throw Isis::iException::Message(Isis::iException::User, message, _FILEINFO_);
  }
  catch(std::exception const &se) {
    string message = "std::exception: " + (iString)se.what();
    throw Isis::iException::Message(Isis::iException::User, message, _FILEINFO_);
  }
  catch(...) {
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
  if(psType == "LOW")
    return Low;
  else if(psType == "HIGH")
    return High;
  else if(psType == "MEAN")
    return Mean;
  else if(psType == "NEAREST")
    return Nearest;
  else if(psType == "RANGE")
    return Range;

  return High;
}

#include "Isis.h"

#include <iomanip>

#include "Brick.h"
#include "ControlNet.h"
#include "ControlPoint.h"
#include "Cube.h"
#include "Filename.h"
#include "iException.h"
#include "UniversalGroundMap.h"

using namespace std;
using namespace Isis;

void IsisMain() {
  UserInterface &ui = Application::GetUserInterface();
  
  ControlNet cnet_in (ui.GetFilename("CNET"));
  ControlNet cnet_out;

  // Get input cube and get camera model for it  
  string from = ui.GetFilename("DEM");
  Cube cube;
  cube.Open(from);
// TODO: need empty constructor to declare ugm outside try/catch
//  try {
    UniversalGroundMap ugm(cube);
//  } catch (iException e){
//    std::string msg = "Cannot initalize UniversalGroundMap for cube [" + from + "]";
//    throw iException::Message(iException::User,msg,_FILEINFO_);
//  }

  
  int numSuccesses = 0;
  int numFailures = 0;
  string failedIDs = "";

  for (int i=0; i < cnet_in.Size() ; i++) {
    ControlPoint temp_cp = cnet_in[i];
    if(temp_cp.Type() == ControlPoint::Ground) {
      // Create Brick on samp, line to get the dn value of the pixel
      Brick b(1,1,1,cube.PixelType());
      bool ugSuccess = ugm.SetUniversalGround(temp_cp.UniversalLatitude(), temp_cp.UniversalLongitude());
      b.SetBasePosition((int)ugm.Sample(),(int)ugm.Line(),1);
      cube.Read(b);
      double newRadius = b[0];

      // if we are unable to calculate a valid Radius value for a point, 
      // we will ignore this point and keep track of it
      if (IsSpecial(newRadius) || ugSuccess == false){
        numFailures++;
        if (numFailures > 1) {
          failedIDs = failedIDs + ", ";
        }
        failedIDs = failedIDs + temp_cp.Id();
        temp_cp.SetIgnore(true);
      }
      // otherwise, we will replace the computed radius value to the output control net
      else{
        numSuccesses++;
        temp_cp.SetUniversalGround(temp_cp.UniversalLatitude(), temp_cp.UniversalLongitude(), newRadius);
      }
    }
    cnet_out.Add(temp_cp);
  }

  if (numSuccesses == 0) {
    string msg = "No valid radii can be calculated. Verify that the DEM [" + ui.GetAsString("DEM") + "] is valid.";
    throw Isis::iException::Message(Isis::iException::User, msg, _FILEINFO_);
  }

  // Propogate and Create Labels
  cnet_out.SetType(cnet_in.Type());
  cnet_out.SetTarget (cnet_in.Target());
  cnet_out.SetNetworkId(ui.GetString("NETWORKID"));
  cnet_out.SetUserName(Isis::Application::UserName());
  cnet_out.SetDescription(cnet_in.Description());
  cnet_out.SetCreatedDate(Isis::Application::DateTime());

  cnet_out.Write(ui.GetFilename("TO"));

  // Write results to Logs 
  // Summary group is created with the counts of successes and failures
  PvlGroup summaryGroup = PvlGroup("Summary");
  summaryGroup.AddKeyword(PvlKeyword("Successes", numSuccesses));
  summaryGroup.AddKeyword(PvlKeyword("Failures", numFailures));

  bool errorlog;
  Filename errorlogFile;
  // if a filename was entered, use it to create the log
  if (ui.WasEntered("ERRORS")) {
    errorlog = true;
    errorlogFile = ui.GetFilename("ERRORS");
  }
  // if no filename was entered, but there were some failures,
  // create an error log named "failures" in the current directory
  else if (numFailures > 0) {
    errorlog = true;
    errorlogFile = "failures.log";
  }
  // if all radii are successfully calculated and no error
  // file is named, only output summary to application log
  else {
    errorlog = false;
  }

  if (errorlog) {
    // Write details in error log
    Pvl results;
    results.SetName("Results");
    results.AddGroup(summaryGroup);
    if (numFailures > 0) {
      // if there are any failures, add comment to the summary log to alert user
      summaryGroup.AddComment("Unable to calculate radius for all points. Point IDs for failures contained in ["+errorlogFile.Name()+"].");
      PvlGroup failGroup = PvlGroup("Failures");
      failGroup.AddComment("A point fails if we are unable to set universal ground or if the radius calculated is a special pixel value.");
      failGroup.AddKeyword(PvlKeyword("PointIDs", failedIDs));
      results.AddGroup(failGroup);
    }
    results.Write(errorlogFile.Expanded());
  }
  // Write summary to application log
  Application::Log(summaryGroup);
}

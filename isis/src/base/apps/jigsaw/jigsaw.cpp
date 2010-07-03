#include "Isis.h"
#include "Process.h"
#include "BundleAdjust.h"
#include "Table.h"
#include "iException.h"
#include "CubeAttribute.h"
#include "iTime.h"

using namespace std;
using namespace Isis;

void IsisMain() {
  
  // Get the control network and image list
  UserInterface &ui = Application::GetUserInterface();
  std::string cnetFile = ui.GetFilename("CNET");
  std::string cubeList = ui.GetFilename("FROMLIST");

  // Get the held list if entered and prep for bundle adjustment
  BundleAdjust *b = NULL;

  if (ui.WasEntered("HELDLIST")) {
    std::string heldList = ui.GetFilename("HELDLIST");
    b = new BundleAdjust(cnetFile,cubeList,heldList);
  }
  else {
    b = new BundleAdjust(cnetFile,cubeList);
  }

  b->SetObservationMode(ui.GetBoolean("OBSERVATIONS"));
  b->SetSolutionMethod(ui.GetString("METHOD"));
  b->SetSolveRadii(ui.GetBoolean("RADIUS"));

  b->SetCkDegree(ui.GetInteger("CKDEGREE"));
  b->SetSolveCamDegree(ui.GetInteger("SOLVEDEGREE"));
  std::string camsolve = ui.GetString("CAMSOLVE");
  if (camsolve == "NONE") {
    b->SetSolveCmatrix ( BundleAdjust::None );
  }
  else if (camsolve == "ANGLES") {
    b->SetSolveCmatrix ( BundleAdjust::AnglesOnly );
  }
  else if (camsolve == "VELOCITIES") {
    b->SetSolveCmatrix ( BundleAdjust::AnglesVelocity );
  }
  else if (camsolve == "ACCELERATIONS") {
    b->SetSolveCmatrix ( BundleAdjust::AnglesVelocityAcceleration );
  }
  else {
    b->SetSolveCmatrix ( BundleAdjust::All );
  }

  b->SetSolveTwist( ui.GetBoolean("TWIST"));
  
  std::string spsolve = ui.GetString("SPSOLVE");
  if (spsolve == "NONE") {
    b->SetSolveSpacecraftPosition( BundleAdjust::Nothing );
  }
  else if (spsolve == "POSITION") {
    b->SetSolveSpacecraftPosition ( BundleAdjust::PositionOnly );
  }
  else if (spsolve == "VELOCITIES") {
    b->SetSolveSpacecraftPosition ( BundleAdjust::PositionVelocity );
  }
  else {
    b->SetSolveSpacecraftPosition ( BundleAdjust::PositionVelocityAcceleration );
  }

  // Check to make sure user entered something to adjust... Or can just points be in solution?
  if (camsolve == "NONE"  &&  spsolve == "NONE") {
    string msg = "Must either solve for camera pointing or spacecraft position";
    throw iException::Message(Isis::iException::User,msg,_FILEINFO_);
  }

  double tol = ui.GetDouble("TOL");
  int maxIterations = ui.GetInteger("MAXITS");

  // Bundle adjust the network
  try {
    b->Solve(tol,maxIterations);
    b->ControlNet()->Write(ui.GetFilename("ONET"));
    PvlGroup gp( "JigsawResults" );

    // Update the cube pointing if requested
    if (ui.GetBoolean("UPDATE")) {

      for (int i=0; i<b->Images(); i++) {
        Process p;
        CubeAttributeInput inAtt;
        Cube *c = p.SetInputCube(b->Filename(i), inAtt, ReadWrite);
        //check for existing polygon, if exists delete it
        if (c->Label()->HasObject("Polygon")) {
          c->Label()->DeleteObject("Polygon");
        }

        //check for CameraStatistics Table, if exists, delete
        for (int iobj=0; iobj<c->Label()->Objects(); iobj++) {
          PvlObject obj = c->Label()->Object(iobj);
          if (obj.Name() != "Table") continue;
          if (obj["Name"][0] != iString("CameraStatistics")) continue;
          c->Label()->DeleteObject(iobj);
          break;
        }

        //  Get Kernel group and add or replace LastModifiedInstrumentPointing
        //  keyword.
        Table cmatrix = b->Cmatrix(i);
        std::string jigComment = "Jigged = "+ Isis::iTime::CurrentLocalTime();
        cmatrix.Label().AddComment(jigComment);
        Table spvector = b->SpVector(i);
        spvector.Label().AddComment(jigComment);
        c->Write(cmatrix);
        c->Write(spvector);
        p.WriteHistory(*c);
      }
      gp += PvlKeyword("Status", "Camera pointing updated");
    }
    else {
      gp += PvlKeyword("Status", "Camera pointing NOT updated");
    }
    Application::Log( gp );
  }

  catch (iException &e) {

    b->ControlNet()->Write(ui.GetFilename("ONET"));
    string msg = "Unable to bundle adjust network [" + cnetFile + "]";
    throw iException::Message(Isis::iException::User,msg,_FILEINFO_);
  }

  delete b;

}

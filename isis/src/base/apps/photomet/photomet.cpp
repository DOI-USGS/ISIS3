#include "Isis.h"
#include <string>
#include "Camera.h"
#include "ProcessByLine.h"
#include "SpecialPixel.h"
#include "Photometry.h"
#include "Pvl.h"
#include "Cube.h"

#include "PvlGroup.h"
#include "iException.h"

#define MIN(x,y) (((x) < (y)) ? (x) : (y))
#define MAX(x,y) (((x) > (y)) ? (x) : (y))

using namespace std; 
using namespace Isis;

// Global variables
Camera *cam;
Cube *icube;
Photometry *pho;
double maxema; 
double maxinc; 

void photomet (Buffer &in, Buffer &out);

void IsisMain() {
  // We will be processing by line 
  ProcessByLine p;

  // Set up the input cube and get camera information
  icube = p.SetInputCube("FROM");
  cam = icube->Camera();

  // Create the output cube
  p.SetOutputCube("TO");

  // Set up the user interface
  UserInterface &ui = Application::GetUserInterface();
  // Get the name of the parameter file
  Pvl par(ui.GetFilename("PHOPAR"));
  // Set value for maximum emission/incidence angles chosen by user
  maxema = ui.GetDouble("MAXEMISSION");
  maxinc = ui.GetDouble("MAXINCIDENCE");

  // Get the BandBin Center from the image
  PvlGroup pvlg = icube->GetGroup("BandBin");
  double wl;
  if (pvlg.HasKeyword("Center")) {
    PvlKeyword &wavelength = pvlg.FindKeyword("Center");
    wl = wavelength[0];
  } else {
    wl = 1.0;
  }

  // Create the photometry object and set the wavelength
  PvlGroup &algo = par.FindObject("NormalizationModel").FindGroup("Algorithm",Pvl::Traverse);
  if (!algo.HasKeyword("Wl")) {
    algo.AddKeyword(Isis::PvlKeyword("Wl",wl));
  }
  pho = new Photometry(par);
  pho->SetPhotomWl(wl);

  // Start the processing
  p.StartProcess(photomet);
  p.EndProcess();
}

/**
 * Perform photometric correction 
 * 
 * @param in Buffer containing input DN values
 * @param out Buffer containing output DN values 
 * @author Janet Barrett 
 * @internal 
 *   @history 2009-01-08 Jeannie Walldren - Modified to set off
 *            target pixels to null.  Added check for new maxinc
 *            and maxema parameters.
 */
void photomet (Buffer &in, Buffer &out) {

  double pha,inc,ema,mult,base;
  for (int i=0; i<in.size(); i++) {
    // if special pixel, copy to output
    if (!IsValidPixel(in[i])) {
      out[i] = in[i];
    }
    // if off the target, set to null
    else if (!cam->SetImage(in.Sample(i),in.Line(i))) {
      out[i] = NULL8;
    }
    // otherwise, compute angle values
    else{
      pha = cam->PhaseAngle();
      inc = cam->IncidenceAngle();
      ema = cam->EmissionAngle();
      
      // if invalid angles, set to null
      if (inc >= 90.0 || ema >= 90.0) {
        out[i] = NULL8;
      } 
      // if angles greater than max allowed by user, set to null
      else if (inc > maxinc || ema > maxema) {
        out[i] = NULL8;
      }
      // otherwise, do photometric correction
      else {
        pho->Compute(pha,inc,ema,in[i],out[i],mult,base);
      }
    } 
  }
}

#include "Isis.h"
#include "ProcessByLine.h"
#include "SpecialPixel.h"
#include "MultivariateStatistics.h"
#include "OverlapStatistics.h"
#include "UserInterface.h"
#include "iException.h"

using namespace std;
using namespace Isis;
void getStats (vector<Buffer *> &in, vector<Buffer *> &out);
void toneMatch (Buffer &in, Buffer &out);

MultivariateStatistics stats;
double base,mult;

void IsisMain() {
  // We will be processing by line
  ProcessByLine p;

  // Get the user interface
  UserInterface &ui = Application::GetUserInterface();

  // If the user selected pOverlap, get the projected overlap statistics
  if (ui.GetBoolean("POVERLAP")) {
    // Set up the overlap statistics object
    Cube from,match;
    from.Open(ui.GetFilename("FROM"));
    match.Open(ui.GetFilename("MATCH"));
    OverlapStatistics oStats(from,match);

    //Make sure the projections overlap
    if (!oStats.HasOverlap()) {
      string msg = "Input Cubes do not appear to overlap";
      throw Isis::iException::Message(Isis::iException::User,msg,_FILEINFO_);
    }
    // Get mvstat data for the overlapping area
    stats = oStats.GetMStats(1);
  }

  // If the user didnt select pOverlap, get the stats of the entire cubes
  else {
  // Setup the input cubes to match
  p.SetInputCube("FROM",Isis::OneBand);
  p.SetInputCube("MATCH",Isis::OneBand);

  // Get the statistics from the cubes
  p.StartProcess(getStats);
  }

  // compute the linear regression fit of the mvstats data
  stats.LinearRegression(base,mult);

  // Apply the correction
  p.ClearInputCubes();
  p.SetInputCube("FROM");
  p.SetOutputCube ("TO");
  p.StartProcess(toneMatch);
  p.EndProcess();
}

void getStats (vector<Buffer *> &in, vector<Buffer *> &out) {
  stats.AddData(in[0]->DoubleBuffer(),in[1]->DoubleBuffer(),in[0]->size());
}

void toneMatch (Buffer &in, Buffer &out) {
  for (int i=0; i<in.size(); i++) {
    if (Isis::IsSpecial(in[i])) {
      out[i] = in[i];
    }
    else {
      out[i] = base + mult * in[i];
    }
  }
}

#include "Isis.h"
#include "ProcessByLine.h"

using namespace std;
using namespace Isis;

void cubeatt (Buffer &in, Buffer &out);

void IsisMain() {
  // We will be processing by line
  ProcessByLine p;

  // Should we propagate tables
  if (!Application::GetUserInterface().GetBoolean("PROPTABLES")) {
    p.PropagateTables(false);
  }

  // Setup the input and output cubes
  p.SetInputCube("FROM");
  p.SetOutputCube ("TO" );

  p.StartProcess(cubeatt);
  p.EndProcess();
}

// Line processing routine
void cubeatt (Buffer &in, Buffer &out)
{
  // Loop and copy pixels in the line.
  for (int i=0; i<in.size(); i++) {
    out[i] = in[i];
  }
}

#include "cubeatt.h"

#include "ProcessByLine.h"

using namespace std;
using namespace Isis;

namespace Isis {
  void cubeatt_process(Buffer &in, Buffer &out);

  void cubeatt(UserInterface &ui) {
    Cube icube;
    CubeAttributeInput inAtt = ui.GetInputAttribute("FROM");
    if (inAtt.bands().size() != 0) {
      icube.setVirtualBands(inAtt.bands());
    }
    icube.open(ui.GetFileName("FROM"));
    cubeatt(&icube, ui);
  }

  void cubeatt(Cube *icube, UserInterface &ui) {
    // We will be processing by line
    ProcessByLine p;

    // Should we propagate tables
    if(ui.GetBoolean("PROPTABLES")) {
      p.PropagateTables(false);
    }

    // Setup the input and output cubes
    p.SetInputCube(icube);
    p.SetOutputCube(ui.GetFileName("TO"), ui.GetOutputAttribute("TO"), 
                    icube->sampleCount(), icube->lineCount(), icube->bandCount());

    p.StartProcess(cubeatt_process);
    p.EndProcess();
  }

  // Line processing routine
  void cubeatt_process(Buffer &in, Buffer &out) {
    // Loop and copy pixels in the line.
    for(int i = 0; i < in.size(); i++) {
      out[i] = in[i];
    }
  }
}

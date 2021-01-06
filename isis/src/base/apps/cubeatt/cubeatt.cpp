#include "cubeatt.h"

#include "ProcessByLine.h"

using namespace std;
using namespace Isis;

namespace Isis {
  void cubeattProcess(Buffer &in, Buffer &out);

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
    bool propTables = ui.GetBoolean("PROPTABLES");
    QString outputFileName = ui.GetFileName("TO");
    CubeAttributeOutput outputAttributes= ui.GetOutputAttribute("TO");
    cubeatt(icube, outputFileName, outputAttributes, propTables);
  }


  void cubeatt(Cube *icube, QString outputCubePath, CubeAttributeOutput outAttributes, bool propTables) {
    // We will be processing by line
    ProcessByLine p;

    // Should we propagate tables
    p.PropagateTables(propTables);

    // Setup the input and output cubes
    p.SetInputCube(icube);
    p.SetOutputCube(outputCubePath, outAttributes, 
                    icube->sampleCount(), icube->lineCount(), icube->bandCount());

    p.StartProcess(cubeattProcess);
    p.EndProcess();
  }


  // Line processing routine
  void cubeattProcess(Buffer &in, Buffer &out) {
    // Loop and copy pixels in the line.
    for(int i = 0; i < in.size(); i++) {
      out[i] = in[i];
    }
  }
}

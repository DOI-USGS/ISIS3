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
    icube.open(ui.GetCubeName("FROM"));
    cubeatt(&icube, ui);
  }


  // Doesn't allow specification of input attributes
  void cubeatt(Cube *icube, UserInterface &ui) {
    bool propTables = ui.GetBoolean("PROPTABLES");
    QString outputFileName = ui.GetCubeName("TO");
    CubeAttributeOutput outputAttributes= ui.GetOutputAttribute("TO");
    cubeatt(icube, outputFileName, outputAttributes, propTables);
  }

  // Doesn't allow specification of input attributes
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

  // Allows specification of both input and output attributes
  void cubeatt(QString inputCubePath, CubeAttributeInput inAtt, QString outputCubePath, CubeAttributeOutput outputAttributes, bool propTables) {
    Cube icube;
    if (inAtt.bands().size() != 0) {
      icube.setVirtualBands(inAtt.bands());
    }
    icube.open(inputCubePath);
    cubeatt(&icube, outputCubePath, outputAttributes, propTables);
  }

  // Line processing routine
  void cubeattProcess(Buffer &in, Buffer &out) {
    // Loop and copy pixels in the line.
    for(int i = 0; i < in.size(); i++) {
      out[i] = in[i];
    }
  }
}

#include "Isis.h"

#include "Cube.h"
#include "IException.h"
#include "IString.h"
#include "OriginalLabel.h"
#include "ProcessImportFits.h"
#include "Pvl.h"
#include "PvlGroup.h"
#include "PvlKeyword.h"
#include "UserInterface.h"

#include <fstream>
#include <iostream>
#include <QString>

using namespace std;
using namespace Isis;

void IsisMain() {

  UserInterface &ui = Application::GetUserInterface();

  ProcessImportFits pfits;

  pfits.setFitsFile(FileName(ui.GetFileName("FROM")));
  pfits.setProcessFileStructure(0);

  Cube *output = pfits.SetOutputCube("TO");

  // Add instrument group if any keywords were put into it.
  PvlGroup instGrp = pfits.standardInstrumentGroup(pfits.fitsLabel(0));
  if (instGrp.keywords() > 0) {
    
    output->label()->findObject("IsisCube") += instGrp;
  }

  // Save the input FITS label in the Cube original labels
  Pvl pvl;
  pvl += pfits.fitsLabel(0);
  OriginalLabel originals(pvl);
  output->write(originals);

  // Convert the image data
  pfits.StartProcess();
  pfits.EndProcess();
}

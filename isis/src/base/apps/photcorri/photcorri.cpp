/* This program has been modified by Driss Takir (USGS) to 
     * incorporate the four photometric models (Lommel-Seeliger, 
     * Rolo, Minnaert, and McEwen) adopted by the OSIRIS-REx 
     * project. 
     * Build 3.0- 8/15/2016 
*/ 


#include "Isis.h"

#include <string>
#include "ProcessByLine.h"

#include "SpecialPixel.h"
#include "Rolo.h"
#include "PhotometricFunction.h"
#include "Minnaert.h"
#include "Lommelseeliger.h"
#include "Mcewen.h"
#include "Pvl.h"
#include "Cube.h"

#include "PvlGroup.h"
#include "IException.h"

using namespace std;
using namespace Isis;

void IsisMain () {
  // We will be processing by line
  ProcessByLine p;

  // Set up the input cube and get camera information
  Cube *icube = p.SetInputCube("FROM");

  // Create the output cube
  Cube *ocube = p.SetOutputCube("TO");

  // Set up the user interface
  UserInterface &ui = Application::GetUserInterface();

  bool useBackplane = false;

  // Get the name of the parameter file
  Pvl par(ui.GetFileName("PHOTMODEL"));

  IString algoName = PhotometricFunction::AlgorithmName(par);
  algoName.UpCase();
    
  PhotometricFunction *pho;

  if (algoName == "ROLO") {
    pho = new Rolo(par, *icube, !useBackplane);
  }
  else if (algoName == "MINNAERT") {
    pho = new Minnaert(par, *icube, !useBackplane);
  }
  else if (algoName == "LOMMELSEELIGER") {
    pho = new Lommelseeliger(par, *icube, !useBackplane);
  }
  else if (algoName == "MCEWEN") {
    pho = new Mcewen(par, *icube, !useBackplane);
  }
  else {
    string msg = " Algorithm Name [" + algoName + "] not recognized in *.pvl file ";
    msg += "Compatible Algorithms are:\n    Rolo\n    Minnaert\n    Lommelseeliger\n   Mcewen";

    throw IException(IException::User, msg, _FILEINFO_);
  }

  pho->SetMaximumPhaseAngle(ui.GetDouble("MAXPHASE"));
  pho->SetMaximumEmissionAngle(ui.GetDouble("MAXEMISSION"));
  pho->SetMaximumIncidenceAngle(ui.GetDouble("MAXINCIDENCE"));

  pho->setIncidence(ui.GetDouble("INCIDENCE"));
  pho->setEmission(ui.GetDouble("EMISSION"));
  pho->setPhase(ui.GetDouble("PHASE"));
    
  // determine how photometric angles should be calculated
  bool useDem = ui.GetBoolean("USEDEM");
  pho->setUseDem(useDem);


  // Start the processing

  p.ProcessCube(*pho);
  PvlGroup photo("Photometry");
  pho->Report(photo);
  ocube->putGroup(photo);
  Application::Log(photo);


    
  p.Finalize();
    
  delete pho;
  pho = NULL;
}



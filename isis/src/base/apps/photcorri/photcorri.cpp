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
#include "McEwen.h"
#include "Pvl.h"
#include "Cube.h"

#include "PvlGroup.h"
#include "IException.h"

using namespace std;
using namespace Isis;

void IsisMain() {
  // We will be processing by line
  ProcessByLine processByLine;

  // Set up the input cube and get camera information
  Cube *icube = processByLine.SetInputCube("FROM");

  // Create the output cube
  Cube *ocube = processByLine.SetOutputCube("TO");

  // Set up the user interface
  UserInterface &ui = Application::GetUserInterface();

  bool useBackplane = false;

  // Get the name of the parameter file
  Pvl par(ui.GetFileName("PHOTMODEL"));

  QString algoName = PhotometricFunction::algorithmName(par).toUpper();
    
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
    pho = new McEwen(par, *icube, !useBackplane);
  }
  else {
    QString msg = " Algorithm Name [" 
                  + algoName 
                  + "] given in PHOTMODEL file ["
                  + ui.GetFileName("PHOTMODEL")
                  + "] not recognized."
                  + "Supported Algorithms include: "
                  + "[Rolo, Minnaert, Lommelseeliger, McEwen].";

    throw IException(IException::User, msg, _FILEINFO_);
  }

  pho->setMaximumPhaseAngle(ui.GetDouble("MAXPHASE"));
  pho->setMaximumEmissionAngle(ui.GetDouble("MAXEMISSION"));
  pho->setMaximumIncidenceAngle(ui.GetDouble("MAXINCIDENCE"));

  pho->setIncidence(ui.GetDouble("INCIDENCE"));
  pho->setEmission(ui.GetDouble("EMISSION"));
  pho->setPhase(ui.GetDouble("PHASE"));
    
  // determine how photometric angles should be calculated
  bool useDem = ui.GetBoolean("USEDEM");
  pho->useDem(useDem);


  // Start the processing

  processByLine.ProcessCube(*pho);
  PvlGroup photo("Photometry");
  pho->report(photo);
  ocube->putGroup(photo);
  Application::Log(photo);


    
  processByLine.Finalize();
    
  delete pho;
  pho = NULL;
}



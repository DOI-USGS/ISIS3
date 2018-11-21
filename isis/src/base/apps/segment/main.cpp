#include "Isis.h"

#include "Application.h"
#include "Cube.h"
#include "ProgramLauncher.h"

using namespace std;
using namespace Isis;

void IsisMain() {

  //Get user parameters
  UserInterface &ui = Application::GetUserInterface();
  FileName inFile = ui.GetFileName("FROM");
  int numberOfLines = ui.GetInteger("NL");
  int lineOverlap   = ui.GetInteger("OVERLAP");

  //Throws exception if user is dumb
  if(lineOverlap >= numberOfLines) {
    throw IException(IException::User, "The Line Overlap (OVERLAP) must be less than the Number of Lines (LN).", _FILEINFO_);
  }

  //Opens the cube
  Cube cube;
  cube.open(inFile.expanded());

  //Loops through, cropping as desired
  int cropNum = 1;
  int startLine = 1;
  bool hasReachedEndOfCube = false;
  while(startLine <= cube.lineCount()  &&  not hasReachedEndOfCube) {
    //! Sets up the proper paramaters for running the crop program
    QString parameters = "FROM=" + inFile.expanded() +
                         " TO=" + inFile.path() + "/" + inFile.baseName() + ".segment" + toString(cropNum) + ".cub"
                         + " LINE=" + toString(startLine) + " NLINES=";

    if(startLine + numberOfLines > cube.lineCount()) {
      parameters += toString(cube.lineCount() - (startLine - 1));
      hasReachedEndOfCube = true;
    }
    else {
      parameters += toString(numberOfLines);
    }
    ProgramLauncher::RunIsisProgram("crop", parameters);
    //The starting line for next crop
    startLine = 1 + cropNum * (numberOfLines - lineOverlap);
    cropNum++;
  }
}

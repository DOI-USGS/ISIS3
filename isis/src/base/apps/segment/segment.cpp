#include "Isis.h"
#include "Application.h"
#include "Cube.h"

using namespace std; 
using namespace Isis;

void IsisMain() {

  //Get user parameters
  UserInterface &ui = Application::GetUserInterface();
  Filename inFile = ui.GetFilename("FROM");
  int numberOfLines = ui.GetInteger("NL");
  int lineOverlap   = ui.GetInteger("OVERLAP");

  //Throws exception if user is dumb
  if ( lineOverlap >= numberOfLines ) {
    throw iException::Message( iException::User, "The Line Overlap (OVERLAP) must be less than the Number of Lines (LN).", _FILEINFO_ );
  }

  //Opens the cube
  Cube cube;
  cube.Open( inFile.Expanded() );

  //Loops through, cropping as desired
  int cropNum = 1;
  int startLine = 1;
  bool hasReachedEndOfCube = false;
  while ( startLine <= cube.Lines()  &&  not hasReachedEndOfCube ) {
    //! Sets up the proper paramaters for running the crop program
    string parameters = "FROM=" + inFile.Expanded() +
        " TO=" + inFile.Path() + "/" + inFile.Basename() + ".segment" + iString(cropNum) + ".cub"
        + " LINE=" + iString(startLine) + " NLINES=";

    if ( startLine + numberOfLines > cube.Lines() ) {
      parameters += iString( cube.Lines() - ( startLine - 1 ) );
      hasReachedEndOfCube = true;
    }
    else {
      parameters += iString(numberOfLines);
    }
    Isis::iApp ->Exec("crop",parameters);
    //The starting line for next crop
    startLine = 1 + cropNum * ( numberOfLines - lineOverlap );
    cropNum++;
  }
}

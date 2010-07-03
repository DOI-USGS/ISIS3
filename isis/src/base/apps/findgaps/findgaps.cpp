#include "Isis.h"
#include "ProcessByLine.h"
#include "SpecialPixel.h"
#include "iString.h"

#include "Statistics.h"
#include "MultivariateStatistics.h"

using namespace std; 
using namespace Isis;

void FindGaps ( Buffer &in );

// The Correlation Tollerance variable
double corTol;
// Vector to hold the previous line
std::vector<double> previousLine;
// The PvlGroup for storing results
PvlGroup pvl("Gap");
Pvl toDisplay;
bool inGap = false;
int lineNum;  // Used when gaps go to the end of the band


void IsisMain() {
  // Processing by line
  ProcessByLine p;

  // Setup the input cube and lastLine array
  Cube *icube = p.SetInputCube( "FROM" );
  previousLine.resize(icube->Samples());
  lineNum = icube->Lines();

  // Gets the Correlation Tollerance
  UserInterface &ui = Application::GetUserInterface();
  corTol = ui.GetDouble("CORTOL");

  // Starts the find gaps process
  p.StartProcess( FindGaps );
  //In case the last gap runs to the end of the cube
  if ( inGap ) {
    pvl.AddKeyword( PvlKeyword( "ToEndOfBand", lineNum ) );
  }
  toDisplay.Write( ui.GetFilename("TO") );
  toDisplay.Clear();
  inGap = false;
  p.EndProcess();
}

/**
 * Processes the current line with the previous, and acts
 * accordingly, posting bad results in Log.
 * @param in
 */
void FindGaps ( Buffer &in ) {
  // Copys line 1 into previousLine since it is the top of the Band
  if ( in.Line() == 1 ) {
    for (int i=0; i<in.size(); i++) previousLine[i] = in[i];
    return;
  }

  // Uses MultivariateStatistics to compare the last line with the current
  MultivariateStatistics mSt;
  mSt.AddData( &previousLine[0], in.DoubleBuffer(), in.size() );
  double correlation = mSt.Correlation();
  if ( std::fabs( correlation ) < corTol  ||  correlation == Isis::Null ) {
    // Then current line is a Gap, and acts accordingly
    if ( !inGap ) {
      inGap = true;
      pvl.AddKeyword( PvlKeyword( "NewGapInBand", in.Band() ) );
      pvl.AddKeyword( PvlKeyword( "StartLine", in.Line() ) );
      if( correlation == Isis::Null ) {
        correlation = 0.0;
      }
      pvl.AddKeyword( PvlKeyword( "Correlation", correlation ) ); 
    }
  }
  else if ( inGap ) {
    // Then it was the last line of the gap 2 lines ago, since this line and its pervious line correlate
    inGap = false;
    if( in.Line()-2 == 0 ) {
      pvl.AddKeyword( PvlKeyword( "ToEndOfBand", lineNum ) );
    }
    else {
      pvl.AddKeyword( PvlKeyword( "LastGapLine", in.Line()-2 ) );
    }
    toDisplay.AddGroup( pvl );
    pvl = PvlGroup("Gap");
  }
  // Sets upt previousLine for next pass
  for (int i=0; i<in.size(); i++) previousLine[i] = in[i];
}

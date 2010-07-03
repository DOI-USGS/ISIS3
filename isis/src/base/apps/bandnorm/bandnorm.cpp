#include "Isis.h"

// system include files go first
#include <string>
#include <iostream>
#include <iomanip>
#include <vector>
#include <algorithm>

// Isis specific include files go next
#include "ProcessByLine.h"
#include "SpecialPixel.h"
#include "iException.h"
#include "Pvl.h"
#include "TextFile.h"
#include "Statistics.h"

using namespace std;
using namespace Isis;

static vector<int> band;
static vector<double> average;
static vector<double> normalizer;

// function prototypes
void getStats(Buffer &in);
void normalize(Buffer &in, Buffer &out);
void Tokenize( const string& str,
               vector<string> & tokens,
               const string& delimiters = " " );

void IsisMain() {
  // We will be processing by line
  ProcessByLine p;

  // Setup the input cube
  UserInterface &ui = Application::GetUserInterface();
  Cube *icube = p.SetInputCube("FROM");

  // Now get the statistics for each band or the entire cube
  string avg = ui.GetString("AVERAGE");
  p.StartProcess(getStats);
  if (avg == "BAND") {
    int b = 0;
    Statistics stats;    
    for (int i=0; i<(int)average.size(); i++) {
      if (b == band[i]) {
        stats.AddData(&average[i],(unsigned int)1);
      }
      else {
        normalizer.push_back(stats.Average());
        b++;
        stats.Reset();
      }               
    }
    normalizer.push_back(stats.Average());
  }
  else if( avg == "PENCIL" ) {
    TextFile pencil;
    pencil.Open( ui.GetFilename("SPECTRUM") );
    if( pencil.LineCount()-1 < icube->Bands()) {
        string msg = "The spectral pencil file [" + ui.GetAsString("SPECTRUM") +
                 "] does not contain enough data for all bands.";
        throw iException::Message(iException::User,msg,_FILEINFO_);
    }
    string st;
    int column=-1;
    vector<string> tokens;
    pencil.GetLine( st );   //Takes care of title line
    Tokenize( st, tokens, ", \"-+" );
    if( ui.GetAsString("METHOD") == "number" ) {
      column = ui.GetInteger("NUMBER");
    }
    else {
      for( unsigned i=0; i<tokens.size(); i++ ) {
        if( tokens[i] == ui.GetString("NAME") ) {
          column = i;
          break;
        }
      }
    }
    if( column < 0  ||  (unsigned)column > tokens.size() ) {
      string msg = "The column specified in file ["+ ui.GetFilename("SPECTRUM")
                   + "] was not found.";
      throw Isis::iException::Message(Isis::iException::User,msg,_FILEINFO_);
    }
    // Add the correct column of data to normalizer
    for( int i=0; i<icube->Bands(); i++ ) {
      tokens.clear();
      pencil.GetLine( st );
      Tokenize( st, tokens, ", \"" );
      normalizer.push_back( Isis::iString(tokens[column]).ToDouble() );
    }
  }
  else {  // avg == "CUBE"
    Statistics stats;
    for (int i=0; i<(int)average.size(); i++) {
      stats.AddData(&average[i],(unsigned int)1);
    }
    for (int b=0; b<icube->Bands(); b++) {
      normalizer.push_back(stats.Average());
    }
  } 

  // Setup the output file and apply the correction
  p.SetOutputCube("TO");
  p.StartProcess(normalize);

  // Cleanup
  p.EndProcess();
  normalizer.clear();
  band.clear();
}

//**********************************************************
// Get statistics on a band or entire cube
//**********************************************************
void getStats(Buffer &in) {
  Statistics stats;
  stats.AddData(in.DoubleBuffer(),in.size());
  average.push_back(stats.Average());
  band.push_back(in.Band()-1);
}

// Apply coefficients
void normalize(Buffer &in, Buffer &out) {
  int index = in.Band()-1;
  double coeff = normalizer[index];

  // Now loop and apply the coefficents
  for (int i=0; i<in.size(); i++) {
    if (IsSpecial(in[i])) {
      out[i] = in[i];
    }
    else {
      out[i] = Null;
      if (coeff != 0.0 && IsValidPixel(coeff)) {
        out[i] = in[i]/coeff;
      }
    }
  }
}

// Tokenizer
void Tokenize( const string& str,
               vector<string> & tokens,
               const string& delimiters )
{
  //Skip delimiters at the beginning
  string::size_type lastPos = str.find_first_not_of(delimiters, 0);
  // Find first "non-delimiter".
  string::size_type pos     = str.find_first_of(delimiters, lastPos);

  while( string::npos != pos  ||  string::npos != lastPos ) {
    // Found a token, add it to the vector
    tokens.push_back( str.substr( lastPos, pos - lastPos ) );
    // Skip delimiters
    lastPos = str.find_first_not_of( delimiters, pos);
    // Find next "non-delimiter"
    pos = str.find_first_of(delimiters, lastPos);
  }
}

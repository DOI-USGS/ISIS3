//************************************************************************
// See Full documentation in isis2raw.xml
//************************************************************************
#include "Isis.h"

#include <iostream>
#include <sstream>


#include "ProcessExport.h"
#include "UserInterface.h"
#include "SpecialPixel.h"

using namespace std; 
using namespace Isis;

enum Pixtype { NONE, NEG, BOTH };

void checkRange( UserInterface &ui, double &min, double &max );
void setRangeAndPixels( UserInterface &ui, ProcessExport &p,
                        double &min, double &max, Pixtype ptype );

// Main program
void IsisMain() {
  // Create an object for exporting Isis data
  ProcessExport p;

  // Open the input cube
  p.SetInputCube("FROM");

  UserInterface &ui = Application::GetUserInterface();

  // Applies the input to output stretch options
  if (ui.GetString ("STRETCH") == "LINEAR") {
    if (ui.GetString ("BITTYPE") != "32BIT")
      p.SetInputRange();
  }
  if (ui.GetString ("STRETCH") == "MANUAL")
    p.SetInputRange(ui.GetDouble("MINIMUM"), ui.GetDouble("MAXIMUM"));

  //  Determine bit size, output range, and calculate number of bytes to write
  //  for each line.
  double min = -DBL_MAX;
  double max = DBL_MAX;
  if (ui.GetString ("BITTYPE") == "8BIT") {
    p.SetOutputType(Isis::UnsignedByte);
    min = 0.0;
    max = 255.0;
    checkRange( ui, min, max );
    setRangeAndPixels( ui, p, min, max, BOTH );
  }
  else if (ui.GetString ("BITTYPE") == "S16BIT") {
    p.SetOutputType(Isis::SignedWord);
    min = -32768.0;
    max = 32767.0;
    checkRange( ui, min, max );
    setRangeAndPixels( ui, p, min, max, NEG );
  }
  else if (ui.GetString ("BITTYPE") == "U16BIT") {
    p.SetOutputType(Isis::UnsignedWord);
    min = 0.0;
    max = 65535.0;
    checkRange( ui, min, max );
    setRangeAndPixels( ui, p, min, max, BOTH );
  }
  else if (ui.GetString ("BITTYPE") == "32BIT") {
    p.SetOutputType(Isis::Real);
    checkRange( ui, min, max );
    setRangeAndPixels( ui, p, min, max, NONE );
  }

  // Set the output endianness
  if (ui.GetString("ENDIAN") == "MSB")
    p.SetOutputEndian(Isis::Msb);
  else if (ui.GetString("ENDIAN") == "LSB")
    p.SetOutputEndian(Isis::Lsb);
  // Open the cube for writing
  string to = ui.GetFilename("TO","raw");
  ofstream fout;  
  fout.open (to.c_str (),ios::out|ios::binary);
  if (!fout.is_open ()) {
    string msg = "Cannot open raw output file";
    throw iException::Message(iException::Programmer,msg,_FILEINFO_);
  }

  // Set the Output Storage Format
  if ( Isis::iString( ui.GetString("STORAGEORDER") ).DownCase()  == "bil" ) {
    p.SetFormat( ProcessExport::BIL );
  }
  else if ( Isis::iString( ui.GetString("STORAGEORDER") ).DownCase()  == "bip" ) {
    p.SetFormat( ProcessExport::BIP );
  }

  // Write the raw cube data
  p.StartProcess (fout);

  fout.close();  
  p.EndProcess();

  // Records what output values were used, then sends it to the print.prt file
  // as well as the terminal.
  PvlGroup results( "DNs Used" );
  results += PvlKeyword( "Null", p.OutputNull() );
  results += PvlKeyword( "LRS", p.OutputLrs() );
  results += PvlKeyword( "LIS", p.OutputLis() );
  results += PvlKeyword( "HIS", p.OutputHis() );
  results += PvlKeyword( "HRS", p.OutputHrs() );
  results += PvlKeyword( "ValidMin", min );
  results += PvlKeyword( "ValidMax", max );
  Application::Log( results );

  return;
}

// Validates provided range
void checkRange( UserInterface &ui, double &min, double &max ) {
  if ( ui.WasEntered("OMIN") ) {
    if ( ui.GetDouble("OMIN") < min ) {
      string message = "OMIN [" + iString(min) + "] is too small for the provided BITTYPE [";
      message += ui.GetString("BITTYPE") + "]";
      throw Isis::iException::Message(Isis::iException::User,message,_FILEINFO_);
    }
    else {
      min = ui.GetDouble("OMIN");
    }
  }
  if ( ui.WasEntered("OMAX") ) {
    if ( ui.GetDouble("OMAX") > max ) {
      string message = "OMAX [" + iString(max) + "] is too large for the provided BITTYPE [";
      message += ui.GetString("BITTYPE") + "]";
      throw Isis::iException::Message(Isis::iException::User,message,_FILEINFO_);
    }
    else {
      max = ui.GetDouble("OMAX");
    }
  }
  if( min >= max ) {
    string message = "OMIN [" + iString(min) + "] cannot be greater than or equal to OMAX [";
    message += iString(max) + "]";
    throw Isis::iException::Message(Isis::iException::User,message,_FILEINFO_);
  }
}

// Sets up special pixels and valid pixel ranges
void setRangeAndPixels( UserInterface &ui, ProcessExport &p, double &min, double &max, Pixtype ptype ) {
  if( ptype == NEG ) {
    if ( ui.GetBoolean("NULL") ) {
      p.SetOutputNull(min++);
    }
    if ( ui.GetBoolean("LRS") ) {
      p.SetOutputLrs(min++);
    }
    if ( ui.GetBoolean("LIS") ) {
      p.SetOutputLis(min++);
    }
     if ( ui.GetBoolean("HIS") ) {
      p.SetOutputHis(min++);
    }
    if ( ui.GetBoolean("HRS") ) {
      p.SetOutputHrs(min++);
    }
  }
  else if( ptype == BOTH ) {
    if ( ui.GetBoolean("NULL") ) {
      p.SetOutputNull(min++);
    }
    if ( ui.GetBoolean("LRS") ) {
      p.SetOutputLrs(min++);
    }
    if ( ui.GetBoolean("LIS") ) {
      p.SetOutputLis(min++);
    }
    if ( ui.GetBoolean("HRS") ) {
      p.SetOutputHrs(max--);
    }
    if ( ui.GetBoolean("HIS") ) {
      p.SetOutputHis(max--);
    }
  }
  p.SetOutputRange(min,max);
}

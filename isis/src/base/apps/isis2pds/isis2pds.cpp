#include "Isis.h"
#include "ProcessExportPds.h"

using namespace std; 
using namespace Isis; 

enum Pixtype { NONE, NEG, BOTH };

void setRangeAndPixels( UserInterface &ui, ProcessExportPds &p,
                        double &min, double &max, Pixtype ptype );
void IsisMain() {
  // Set the processing object
  ProcessExportPds p;
  
  // Setup the input cube
  p.SetInputCube("FROM");

  UserInterface &ui = Application::GetUserInterface();

  if (ui.GetString ("STRETCH") == "LINEAR") {
    if (ui.GetString ("BITTYPE") != "32BIT")
      p.SetInputRange();
  }
  if (ui.GetString ("STRETCH") == "MANUAL")
    p.SetInputRange(ui.GetDouble("MINIMUM"), ui.GetDouble("MAXIMUM"));

  double min = -DBL_MAX;
  double max = DBL_MAX;

  if (ui.GetString("BITTYPE") == "8BIT") {
    p.SetOutputType(Isis::UnsignedByte);
    min = 0.0;
    max = 255.0;
    setRangeAndPixels( ui, p, min, max, BOTH );
  } 
  else if (ui.GetString("BITTYPE") == "S16BIT") {
    p.SetOutputType(Isis::SignedWord);
    min = -32768.0;
    max = 32767.0;
    setRangeAndPixels( ui, p, min, max, NEG );
  } 
  else if (ui.GetString("BITTYPE") == "U16BIT") {
    p.SetOutputType(Isis::UnsignedWord);
    min = 0.0;
    max = 65535.0;
    setRangeAndPixels( ui, p, min, max, BOTH );
  } 
  else {
    p.SetOutputType(Isis::Real);
    p.SetOutputNull(Isis::NULL4);
    p.SetOutputLrs(Isis::LOW_REPR_SAT4);
    p.SetOutputLis(Isis::LOW_INSTR_SAT4);
    p.SetOutputHrs(Isis::HIGH_REPR_SAT4);
    p.SetOutputHis(Isis::HIGH_INSTR_SAT4);
    setRangeAndPixels( ui, p, min, max, NONE );
  }

  if (ui.GetString("ENDIAN") == "MSB")
    p.SetOutputEndian(Isis::Msb);
  else if (ui.GetString("ENDIAN") == "LSB")
    p.SetOutputEndian(Isis::Lsb);

  if (ui.GetString("LABTYPE") == "FIXED")
    p.SetExportType ( ProcessExportPds::Fixed );

  //Set the resolution to  Kilometers  
  p.SetPdsResolution( ProcessExportPds::Kilometer );

  p.StandardPdsLabel( ProcessExportPds::Image);  
 
  Filename outFile(ui.GetFilename("TO", "img"));
  string outFilename(outFile.Expanded());
  ofstream oCube(outFilename.c_str());
  p.OutputLabel(oCube);  
  p.StartProcess(oCube);  
  oCube.close();
  p.EndProcess();

  //Records what it did to the print.prt file
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

//Sets up special pixels and valid pixel ranges
void setRangeAndPixels( UserInterface &ui, ProcessExportPds &p, double &min, double &max, Pixtype ptype ) {
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

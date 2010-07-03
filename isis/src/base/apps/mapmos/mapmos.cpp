#include "Isis.h"
#include "ProcessMapMosaic.h"
#include "PvlGroup.h"

using namespace std; 
using namespace Isis;

void IsisMain() {
  // Get the user interface
  UserInterface &ui = Application::GetUserInterface();
  
  ProcessMapMosaic m;

  // Get the MatchBandBin Flag
  m.SetBandBinMatch(ui.GetBoolean("MATCHBANDBIN"));  

  // Get the track flag
  bool bTrack = ui.GetBoolean("TRACK");
  m.SetTrackFlag(bTrack); 

  // Get the output projection set up properly
  if (ui.GetBoolean("CREATE")) {	
    Cube inCube;
    inCube.Open(ui.GetFilename("FROM"));

	// Set the create flag
	m.SetCreateFlag(true);

    // Use the input projection as a starting point for the mosaic
    PvlGroup mapGroup = inCube.Label()->FindGroup("Mapping", Pvl::Traverse);
    inCube.Close();

    mapGroup.AddKeyword(PvlKeyword("MinimumLatitude",  ui.GetDouble("MINLAT")), Pvl::Replace);
    mapGroup.AddKeyword(PvlKeyword("MaximumLatitude",  ui.GetDouble("MAXLAT")), Pvl::Replace);
    mapGroup.AddKeyword(PvlKeyword("MinimumLongitude", ui.GetDouble("MINLON")), Pvl::Replace);
    mapGroup.AddKeyword(PvlKeyword("MaximumLongitude", ui.GetDouble("MAXLON")), Pvl::Replace);

    CubeAttributeOutput oAtt = ui.GetOutputAttribute("MOSAIC");	 

    m.SetOutputCube(ui.GetFilename("FROM"), mapGroup, oAtt, ui.GetFilename("MOSAIC"));
  }
  else {
    m.SetOutputCube(ui.GetFilename("MOSAIC"));
  }

  // Set up the mosaic priority, either the input cube will be
  // placed ontop of the mosaic or beneath it   
  MosaicPriority priority;
  string sType;	
  if (ui.GetString("PRIORITY") == "BENEATH") {
    priority = mosaic;
  }
  else if (ui.GetString("PRIORITY") == "ONTOP") {
	priority = input;
  }
  else {	
    priority = band;	
	sType = ui.GetString("TYPE");
	if (sType=="BANDNUMBER") {		
		m.SetBandNumber(ui.GetInteger("NUMBER"));
	}
	else { 
		// Key Name & Value
		m.SetBandKeyWord(ui.GetString("KEYNAME"), ui.GetString("KEYVALUE")); 		
	}
	// Band Criteria
	BandCriteria eCriteria = Lesser;
	if (ui.GetString("CRITERIA") == "GREATER")
		eCriteria = Greater;						    	  
	m.SetBandCriteria(eCriteria);
  }
  
  // Set priority
  m.SetPriority(priority);

  if (priority == input || priority == band) {
	  m.SetHighSaturationFlag(ui.GetBoolean("HIGHSATURATION"));
	  m.SetLowSaturationFlag (ui.GetBoolean("LOWSATURATION"));
	  m.SetNullFlag(ui.GetBoolean("NULL"));
  }   	
  
	// Start Process
  if(!m.StartProcess(ui.GetFilename("FROM"))) {
		// Logs the cube if it falls outside of the given mosaic	
		PvlGroup outsiders("Outside");
        outsiders += PvlKeyword("File", ui.GetFilename("FROM"));
		Application::Log(outsiders);
  }
	else {
		// Logs the input file location in the mosaic
		PvlGroup imgPosition("ImageLocation");
		int iStartLine   =  m.GetInputStartLine();
		int iStartSample =  m.GetInputStartSample();
		imgPosition   += PvlKeyword("File", ui.GetFilename("FROM"));
		imgPosition   += PvlKeyword("StartSample", iStartSample);
		imgPosition   += PvlKeyword("StartLine", iStartLine);
		Application::Log(imgPosition);
	}

  if (bTrack != m.GetTrackFlag()) {
		ui.Clear("TRACK");	   	 
		ui.PutBoolean("TRACK", m.GetTrackFlag());
  }      

  m.EndProcess();  
}


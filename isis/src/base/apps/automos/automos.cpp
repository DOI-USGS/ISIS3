#define GUIHELPERS

#include "Isis.h"
#include "ProcessMapMosaic.h"
#include "FileList.h"
#include "iException.h"
#include "SpecialPixel.h"
#include "ProjectionFactory.h"

using namespace std; 
using namespace Isis;

void calcRange(double &minLat, double &maxLat, double &minLon, double &maxLon);
void helperButtonCalcRange ();

map <string,void*> GuiHelpers()
{
  map <string,void*> helper;
  helper ["helperButtonCalcRange"] = (void*) helperButtonCalcRange;
  return helper;
}

void IsisMain() 
{  
  FileList list;
  UserInterface &ui = Application::GetUserInterface();  

	// Get the list of cubes to mosaic
  list.Read(ui.GetFilename("FROMLIST"));
  if (list.size() < 1) {
    string msg = "The list file [" + ui.GetFilename("FROMLIST") +	"does not contain any data";
    throw iException::Message(iException::User,msg,_FILEINFO_);
  }

  ProcessMapMosaic m;  

  // Set the create flag-mosaic is always created in automos
  m.SetCreateFlag(true);

	// Get the Track Flag
  bool bTrack = ui.GetBoolean("TRACK");
  m.SetTrackFlag(bTrack);

  CubeAttributeOutput &oAtt = ui.GetOutputAttribute("MOSAIC");
  if (ui.GetString("GRANGE") == "USER") {
    m.SetOutputCube(list, 
                    ui.GetDouble("MINLAT"), ui.GetDouble("MAXLAT"),
                    ui.GetDouble("MINLON"), ui.GetDouble("MAXLON"),
                    oAtt, ui.GetFilename("MOSAIC"));
  }
  else {
    m.SetOutputCube(list, oAtt, ui.GetFilename("MOSAIC"));
  }

  // Set up the mosaic priority, either the input cubes will be
  // placed ontop of each other in the mosaic or beneath each other or
  // placed based on band with "Lesser" or "Greater" criteria
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
			// Key name & value
			m.SetBandKeyWord(ui.GetString("KEYNAME"), ui.GetString("KEYVALUE")); 		
		}
		// Band Criteria
		BandCriteria eCriteria = Lesser;
		if (ui.GetString("CRITERIA") == "GREATER")
			eCriteria = Greater;						    	  
		m.SetBandCriteria(eCriteria);
  }
  
  // Priority
  m.SetPriority(priority);

	m.SetHighSaturationFlag(ui.GetBoolean("HIGHSATURATION"));
	m.SetLowSaturationFlag (ui.GetBoolean("LOWSATURATION"));
	m.SetNullFlag(ui.GetBoolean("NULL")); 

  // Loop for each input file and place it in the output mosaic
  
  m.SetBandBinMatch(ui.GetBoolean("MATCHBANDBIN"));

  for (unsigned int i=0; i<list.size(); i++) {
    if(!m.StartProcess(list[i])) {
			PvlGroup outsiders("Outside");
      outsiders += PvlKeyword("File", list[i]); 
			Application::Log(outsiders); 
    }
		else {
			PvlGroup imgPosition("ImageLocation");
			int iStartLine   = m.GetInputStartLine();
			int iStartSample = m.GetInputStartSample();
			imgPosition += PvlKeyword("File", list[i]);
			imgPosition += PvlKeyword("StartSample", iStartSample);
			imgPosition += PvlKeyword("StartLine", iStartLine);
			Application::Log(imgPosition);
		}
		if (!i) {
			// Mosaic is already created, use the existing mosaic
			m.SetCreateFlag(false);
		}
  }

  m.EndProcess();  
}

// Function to calculate the ground range from multiple inputs (list of images)
void calcRange(double &minLat, double &maxLat, double &minLon, double &maxLon) 
{
  UserInterface &ui = Application::GetUserInterface();
  FileList list(ui.GetFilename("FROMLIST"));
  minLat = DBL_MAX;
  maxLat = -DBL_MAX;
  minLon = DBL_MAX;
  maxLon = -DBL_MAX;
  // We will loop through each input cube and do some 
  // computations needed for mosaicking
  int nbands = 0;
  Projection *firstProj = NULL;

  for (unsigned int i=0; i<list.size(); i++) {
    // Open the cube and get the maximum number of band in all cubes
    Cube cube;
    cube.Open(list[i]);
    if (cube.Bands() > nbands) nbands = cube.Bands();

    // See if the cube has a projection and make sure it matches
    // previous input cubes
    Projection *proj = Isis::ProjectionFactory::CreateFromCube(*(cube.Label()));
    if (firstProj == NULL) {
      firstProj = proj;
    }
    else if (*proj != *firstProj) {
      string msg = "Mapping groups do not match between cubes [" + list[0] + "] and [" + list[i] + "]";
      throw iException::Message(iException::User,msg,_FILEINFO_);
    }

    if (proj->HasGroundRange()) {
      if (proj->MinimumLatitude() < minLat) minLat = proj->MinimumLatitude();
      if (proj->MaximumLatitude() > maxLat) maxLat = proj->MaximumLatitude();
      if (proj->MinimumLongitude() < minLon) minLon = proj->MinimumLongitude();
      if (proj->MaximumLongitude() > maxLon) maxLon = proj->MaximumLongitude();
    }

    // Cleanup
    cube.Close();
    if (proj != firstProj) 
			delete proj;
  }
}

// Helper function to run calcRange function.
void helperButtonCalcRange () 
{
  UserInterface &ui = Application::GetUserInterface();
  double minLat;
  double maxLat;
  double minLon;
  double maxLon;

  // Run the function calcRange of calculate range info
  calcRange(minLat,maxLat,minLon,maxLon);

  // Write ranges to the GUI
  string use = "USER";
  ui.Clear("GRANGE");
  ui.PutAsString("GRANGE",use);
  ui.Clear("MINLAT");
  ui.PutDouble("MINLAT",minLat);
  ui.Clear("MAXLAT");
  ui.PutDouble("MAXLAT",maxLat);
  ui.Clear("MINLON");
  ui.PutDouble("MINLON",minLon);
  ui.Clear("MAXLON");
  ui.PutDouble("MAXLON",maxLon);
}

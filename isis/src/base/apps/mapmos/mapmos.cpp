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

  // Get the MatchDEM Flag
  m.SetMatchDEM(ui.GetBoolean("MATCHDEM"));

  // Get the track flag
  bool bTrack = ui.GetBoolean("TRACK");
  m.SetTrackFlag(bTrack);

  // Gets the input file along with attributes
  string sInputFile = ui.GetAsString("FROM");

  ProcessMosaic::ImageOverlay overlay = ProcessMosaic::StringToOverlay(
      ui.GetString("PRIORITY"));

  if (overlay == ProcessMapMosaic::UseBandPlacementCriteria) {
    if(ui.GetString("TYPE") == "BANDNUMBER") {
      m.SetBandNumber(ui.GetInteger("NUMBER"));
    }
    else {
      // Key name & value
      m.SetBandKeyword(ui.GetString("KEYNAME"), ui.GetString("KEYVALUE"));
    }
    // Band Criteria
    m.SetBandUseMaxValue( (ui.GetString("CRITERIA") == "GREATER") );
  }

  // Priority
  m.SetImageOverlay(overlay);

  // Get the output projection set up properly
  if(ui.GetBoolean("CREATE")) {
    Cube inCube;
    inCube.open(ui.GetFileName("FROM"));

    // Set the create flag
    m.SetCreateFlag(true);

    // Use the input projection as a starting point for the mosaic
    PvlGroup mapGroup = inCube.getLabel()->FindGroup("Mapping", Pvl::Traverse);
    inCube.close();

    mapGroup.AddKeyword(PvlKeyword("MinimumLatitude",  ui.GetDouble("MINLAT")), Pvl::Replace);
    mapGroup.AddKeyword(PvlKeyword("MaximumLatitude",  ui.GetDouble("MAXLAT")), Pvl::Replace);
    mapGroup.AddKeyword(PvlKeyword("MinimumLongitude", ui.GetDouble("MINLON")), Pvl::Replace);
    mapGroup.AddKeyword(PvlKeyword("MaximumLongitude", ui.GetDouble("MAXLON")), Pvl::Replace);

    CubeAttributeOutput oAtt = ui.GetOutputAttribute("MOSAIC");

    m.SetOutputCube(sInputFile, mapGroup, oAtt, ui.GetFileName("MOSAIC"));
  }
  else {
    m.SetOutputCube(ui.GetFileName("MOSAIC"));
  }


  m.SetHighSaturationFlag(ui.GetBoolean("HIGHSATURATION"));
  m.SetLowSaturationFlag(ui.GetBoolean("LOWSATURATION"));
  m.SetNullFlag(ui.GetBoolean("NULL"));

  // Start Process
  if(!m.StartProcess(sInputFile)) {
    // Logs the cube if it falls outside of the given mosaic
    PvlGroup outsiders("Outside");
    outsiders += PvlKeyword("File", ui.GetFileName("FROM"));
    Application::Log(outsiders);
  }
  else {
    // Logs the input file location in the mosaic
    PvlGroup imgPosition("ImageLocation");
    int iStartLine   =  m.GetInputStartLineInMosaic();
    int iStartSample =  m.GetInputStartSampleInMosaic();
    imgPosition   += PvlKeyword("File", ui.GetFileName("FROM"));
    imgPosition   += PvlKeyword("StartSample", iStartSample);
    imgPosition   += PvlKeyword("StartLine", iStartLine);
    Application::Log(imgPosition);
  }

  if(bTrack != m.GetTrackFlag()) {
    ui.Clear("TRACK");
    ui.PutBoolean("TRACK", m.GetTrackFlag());
  }

  m.EndProcess();
}


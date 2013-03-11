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
  QString sInputFile = ui.GetAsString("FROM");

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
    PvlGroup mapGroup = inCube.label()->findGroup("Mapping", Pvl::Traverse);
    inCube.close();

    mapGroup.addKeyword(PvlKeyword("MinimumLatitude",  toString(ui.GetDouble("MINLAT"))), Pvl::Replace);
    mapGroup.addKeyword(PvlKeyword("MaximumLatitude",  toString(ui.GetDouble("MAXLAT"))), Pvl::Replace);
    mapGroup.addKeyword(PvlKeyword("MinimumLongitude", toString(ui.GetDouble("MINLON"))), Pvl::Replace);
    mapGroup.addKeyword(PvlKeyword("MaximumLongitude", toString(ui.GetDouble("MAXLON"))), Pvl::Replace);

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
    for (int i = 0; i < m.imagePositions().groups(); i++) {
      Application::Log(m.imagePositions().group(i));
    }
  }


  if(bTrack != m.GetTrackFlag()) {
    ui.Clear("TRACK");
    ui.PutBoolean("TRACK", m.GetTrackFlag());
  }

  m.EndProcess();
}


#define GUIHELPERS

#include "Isis.h"
#include "ProcessMapMosaic.h"
#include "FileList.h"
#include "IException.h"
#include "SpecialPixel.h"
#include "TProjection.h"
#include "ProjectionFactory.h"

using namespace std;
using namespace Isis;

void calcRange(double &minLat, double &maxLat, double &minLon, double &maxLon);
void helperButtonCalcRange();

map <QString, void *> GuiHelpers() {
  map <QString, void *> helper;
  helper ["helperButtonCalcRange"] = (void *) helperButtonCalcRange;
  return helper;
}

void IsisMain() {
  FileList list;
  UserInterface &ui = Application::GetUserInterface();

  // Get the list of cubes to mosaic
  list.read(FileName(ui.GetFileName("FROMLIST")));
  if(list.size() < 1) {
    QString msg = "The list file [" + ui.GetFileName("FROMLIST") +"does not contain any data";
    throw IException(IException::User, msg, _FILEINFO_);
  }

  fstream os;
  bool olistFlag = false;
  if (ui.WasEntered("TOLIST")){
    QString olist = ui.GetFileName("TOLIST");
    olistFlag = true;
    os.open(olist.toLatin1().data(), std::ios::out);
  }

  ProcessMapMosaic m;

  // Set the create flag-mosaic is always created in automos
  m.SetCreateFlag(true);

  // Get the Track Flag
  bool bTrack = ui.GetBoolean("TRACK");
  m.SetTrackFlag(bTrack);

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

  CubeAttributeOutput &oAtt = ui.GetOutputAttribute("MOSAIC");
  if(ui.GetString("GRANGE") == "USER") {
    m.SetOutputCube(list,
                    ui.GetDouble("MINLAT"), ui.GetDouble("MAXLAT"),
                    ui.GetDouble("MINLON"), ui.GetDouble("MAXLON"),
                    oAtt, ui.GetFileName("MOSAIC"));
  }
  else {
    m.SetOutputCube(list, oAtt, ui.GetFileName("MOSAIC"));
  }

  m.SetHighSaturationFlag(ui.GetBoolean("HIGHSATURATION"));
  m.SetLowSaturationFlag(ui.GetBoolean("LOWSATURATION"));
  m.SetNullFlag(ui.GetBoolean("NULL"));

  // Loop for each input file and place it in the output mosaic

  m.SetBandBinMatch(ui.GetBoolean("MATCHBANDBIN"));

  // Get the MatchDEM Flag
  m.SetMatchDEM(ui.GetBoolean("MATCHDEM"));

  bool mosaicCreated = false;
  for (int i = 0; i < list.size(); i++) {
    if (!m.StartProcess(list[i].toString())) {
      PvlGroup outsiders("Outside");
      outsiders += PvlKeyword("File", list[i].toString());
      Application::Log(outsiders);
    }
    else {
      mosaicCreated = true;
      if(olistFlag) {
        os << list[i].toString() << endl;
      }
    }
    if(mosaicCreated) {
      // Mosaic is already created, use the existing mosaic
      m.SetCreateFlag(false);
    }
  }
  // Logs the input file location in the mosaic
  for (int i = 0; i < m.imagePositions().groups(); i++) {
    Application::Log(m.imagePositions().group(i));
  }

  if(olistFlag) {
    os.close();
  }

  m.EndProcess();
}

// Function to calculate the ground range from multiple inputs (list of images)
void calcRange(double &minLat, double &maxLat, double &minLon, double &maxLon) {
  UserInterface &ui = Application::GetUserInterface();
  FileList list(FileName(ui.GetFileName("FROMLIST")));
  minLat = DBL_MAX;
  maxLat = -DBL_MAX;
  minLon = DBL_MAX;
  maxLon = -DBL_MAX;
  // We will loop through each input cube and do some
  // computations needed for mosaicking
  int nbands = 0;
  TProjection *firstProj = NULL;

  for(int i = 0; i < list.size(); i++) {
    // Open the cube and get the maximum number of band in all cubes
    Cube cube;
    cube.open(list[i].toString());
    if(cube.bandCount() > nbands) nbands = cube.bandCount();

    // See if the cube has a projection and make sure it matches
    // previous input cubes
    TProjection *proj = (TProjection *) Isis::ProjectionFactory::CreateFromCube(*(cube.label()));
    if(firstProj == NULL) {
      firstProj = proj;
    }
    else if(*proj != *firstProj) {
      QString msg = "Mapping groups do not match between cubes [" + list[0].toString() +
                    "] and [" + list[i].toString() + "]";
      throw IException(IException::User, msg, _FILEINFO_);
    }

    if(proj->HasGroundRange()) {
      if(proj->MinimumLatitude() < minLat) minLat = proj->MinimumLatitude();
      if(proj->MaximumLatitude() > maxLat) maxLat = proj->MaximumLatitude();
      if(proj->MinimumLongitude() < minLon) minLon = proj->MinimumLongitude();
      if(proj->MaximumLongitude() > maxLon) maxLon = proj->MaximumLongitude();
    }

    // Cleanup
    cube.close();
    if(proj != firstProj)
      delete proj;
  }
}

// Helper function to run calcRange function.
void helperButtonCalcRange() {
  UserInterface &ui = Application::GetUserInterface();
  double minLat;
  double maxLat;
  double minLon;
  double maxLon;

  // Run the function calcRange of calculate range info
  calcRange(minLat, maxLat, minLon, maxLon);

  // Write ranges to the GUI
  QString use = "USER";
  ui.Clear("GRANGE");
  ui.PutAsString("GRANGE", use);
  ui.Clear("MINLAT");
  ui.PutDouble("MINLAT", minLat);
  ui.Clear("MAXLAT");
  ui.PutDouble("MAXLAT", maxLat);
  ui.Clear("MINLON");
  ui.PutDouble("MINLON", minLon);
  ui.Clear("MAXLON");
  ui.PutDouble("MAXLON", maxLon);
}

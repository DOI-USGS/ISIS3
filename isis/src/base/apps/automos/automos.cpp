#include "automos.h"
#include "ProcessMapMosaic.h"
#include "FileList.h"
#include "IException.h"
#include "SpecialPixel.h"
#include "TProjection.h"
#include "ProjectionFactory.h"

using namespace std;

namespace Isis {
  void automos(UserInterface &ui, Pvl *log) {
    FileList list;

    // Get the list of cubes to mosaic
    list.read(FileName(ui.GetFileName("FROMLIST")));

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
        if (log) {
          log->addGroup(outsiders);
        }
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
      if (log) {
        log->addGroup(m.imagePositions().group(i));
      }
    }

    if(olistFlag) {
      os.close();
    }

    m.EndProcess();
  }
}

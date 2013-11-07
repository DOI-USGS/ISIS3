#define GUIHELPERS

#include "Isis.h"

#include <QString>
#include <cmath>

#include "Brick.h"
#include "FileName.h"
#include "IException.h"
#include "IString.h"
#include "Process.h"
#include "TProjection.h"
#include "ProjectionFactory.h"
#include "SpecialPixel.h"

using namespace std;
using namespace Isis;

void PrintMap();

map <QString, void *> GuiHelpers() {
  map <QString, void *> helper;
  helper ["PrintMap"] = (void *) PrintMap;
  return helper;
}

void IsisMain() {
  // Use a regular Process
  Process p;

  // Open the input cube and initialize the projection
  Cube *icube = p.SetInputCube("FROM");
  TProjection *proj = (TProjection *) icube->projection();

  // Get the coordinate
  UserInterface &ui = Application::GetUserInterface();

  // Get the sample/line position if we have an image point
  if(ui.GetString("TYPE") == "IMAGE") {
    double samp = ui.GetDouble("SAMPLE");
    double line = ui.GetDouble("LINE");
    proj->SetWorld(samp, line);
  }

  // Get the lat/lon position if we have a ground point
  else if(ui.GetString("TYPE") == "GROUND") {
    double lat = ui.GetDouble("LATITUDE");
    double lon = ui.GetDouble("LONGITUDE");

    // Make sure we have a valid latitude value
    if(fabs(lat) > 90.0) {
      QString msg = "Invalid value for LATITUDE ["
                   + toString(lat) + "] outside range of ";
      msg += "[-90,90]";
      throw IException(IException::User, msg, _FILEINFO_);
    }

    IString coordsys = ui.GetString("COORDSYS");
    coordsys.UpCase();

    // All of these ifs will finish by setting the ground in the projection,
    // there are 4 options, Universal, InputFileSystem, Mapfile, and
    // Userdefined.

    // Positive East, 0-360, Planetocentric
    if(coordsys == "UNIVERSAL") {
      proj->SetUniversalGround(lat, lon);
    }

    // Use the coordinate system of the input file
    else if(coordsys == "INPUTFILESYS") {
      proj->SetGround(lat, lon);
    }

    // Use the mapping group from a given file
    else if(coordsys == "MAP") {
      FileName mapFile = ui.GetFileName("MAP");

      // Does it exist?
      if(!mapFile.fileExists()) {
        QString msg = "Filename [" + ui.GetFileName("MAP") + "] does not exist";
        throw IException(IException::User, msg, _FILEINFO_);
      }

      // Load it up into a new projection
      Pvl mapPvl;
      mapPvl.read(mapFile.expanded());
      TProjection *altmap = (TProjection *) ProjectionFactory::CreateFromCube(mapPvl);

      // Set lat and lon in its system
      altmap->SetGround(lat, lon);

      // Set universal in input cube from universal from given
      // mapping/projection
      proj->SetUniversalGround(
        altmap->UniversalLatitude(), altmap->UniversalLongitude());

      // I think this is right, no cube owns it, and the factory doesn't.
      delete altmap;
      altmap = NULL;
    }

    // User defined coordinate system, 8 possible combinations
    // we only have to make changes for some of them.
    // Convert from given system to universal and then set.
    else if(coordsys == "USERDEFINED") {
      double lat2 = lat;
      double lon2 = lon;

      if(ui.GetString("LATTYPE") == "PLANETOGRAPHIC") {
        lat2 = proj->ToPlanetocentric(lat);
      }

      if(ui.GetString("LONDOM") == "180") {
        lon2 = proj->To360Domain(lon);
      }

      if(ui.GetString("LONDIR") == "POSITIVEWEST") {
        // Use lon2, we know its already in 0-360
        lon2 = proj->ToPositiveEast(lon2, 360);
      }

      proj->SetUniversalGround(lat2, lon2);
    }
  }

  // Get the x/y position if we have a projection point
  else {
    double x = ui.GetDouble("X");
    double y = ui.GetDouble("Y");
    proj->SetCoordinate(x, y);
  }

  // Create Brick on samp, line to get the dn value of the pixel
  Brick b(1, 1, 1, icube->pixelType());
  int intSamp = (int)(proj->WorldX() + 0.5);
  int intLine = (int)(proj->WorldY() + 0.5);
  b.SetBasePosition(intSamp, intLine, 1);
  icube->read(b);

  // Log the position
  if(proj->IsGood()) {
    PvlGroup results("Results");
    results += PvlKeyword("Filename",
                          FileName(ui.GetFileName("FROM")).expanded());
    results += PvlKeyword("Sample", toString(proj->WorldX()));
    results += PvlKeyword("Line", toString(proj->WorldY()));
    results += PvlKeyword("PixelValue", PixelToString(b[0]));
    results += PvlKeyword("X", toString(proj->XCoord()));
    results += PvlKeyword("Y", toString(proj->YCoord()));

    // Put together all the keywords for different coordinate systems.
    PvlKeyword centLat =
      PvlKeyword("PlanetocentricLatitude", toString(proj->UniversalLatitude()));

    PvlKeyword graphLat =
      PvlKeyword("PlanetographicLatitude",
                 toString(proj->ToPlanetographic(proj->UniversalLatitude())));

    PvlKeyword pE360 =
      PvlKeyword("PositiveEast360Longitude", toString(proj->UniversalLongitude()));

    PvlKeyword pW360 =
      PvlKeyword("PositiveWest360Longitude",
                 toString(proj->ToPositiveWest(proj->UniversalLongitude(), 360)));

    PvlKeyword pE180 =
      PvlKeyword("PositiveEast180Longitude",
                 toString(proj->To180Domain(proj->UniversalLongitude())));

    PvlKeyword pW180 =
      PvlKeyword("PositiveWest180Longitude",
                 toString(proj->To180Domain(proj->ToPositiveEast(
                            proj->UniversalLongitude(), 360))));

    // Input map coordinate system location
    // Latitude
    if(proj->IsPlanetocentric()) {
      centLat.addComment("Input map coordinate system");
      results += centLat;
    }
    else {
      graphLat.addComment("Input map coordinate system");
      results += graphLat;
    }

    // Longitude
    if(proj->IsPositiveEast()) {
      if(proj->Has360Domain()) {
        results += pE360;
      }
      else {
        results += pE180;
      }
    }
    else {
      if(proj->Has360Domain()) {
        results += pW360;
      }
      else {
        results += pW180;
      }
    }

    // Non input corrdinate system locations
    // Latitude
    if(proj->IsPlanetocentric()) {
      graphLat.addComment("Location in other coordinate systems");
      results += graphLat;
    }
    else {
      centLat.addComment("Location in other coordinate systems");
      results += centLat;
    }

    // Longitude
    if(proj->IsPositiveEast()) {
      if(proj->Has360Domain()) {
        results += pW360;
        results += pE180;
        results += pW180;
      }
      else {
        results += pE360;
        results += pW360;
        results += pW180;
      }
    }
    else {
      if(proj->Has360Domain()) {
        results += pE360;
        results += pE180;
        results += pW180;
      }
      else {
        results += pE360;
        results += pE180;
        results += pW360;
      }
    }

    Application::Log(results);

    // Write an output label file if necessary
    if(ui.WasEntered("TO")) {
      // Get user params from ui
      QString outFile = FileName(ui.GetFileName("TO")).expanded();
      bool exists = FileName(outFile).fileExists();
      bool append = ui.GetBoolean("APPEND");

      // Write the pvl group out to the file
      if(ui.GetString("FORMAT") == "PVL") {
        Pvl temp;
        temp.addGroup(results);
        if(append) {
          temp.append(outFile);
        }
        else {
          temp.write(outFile);
        }
      }

      // Create a flatfile of the same data
      // The flatfile is comma delimited and can be imported into Excel
      else {
        ofstream os;
        bool writeHeader = false;
        if(append) {
          os.open(outFile.toAscii().data(), ios::app);
          if(!exists) {
            writeHeader = true;
          }
        }
        else {
          os.open(outFile.toAscii().data(), ios::out);
          writeHeader = true;
        }

        if(writeHeader) {
          for(int i = 0; i < results.keywords(); i++) {
            os << results[i].name();

            if(i < results.keywords() - 1) {
              os << ",";
            }
          }
          os << endl;
        }

        for(int i = 0; i < results.keywords(); i++) {
          os << (QString)results[i];

          if(i < results.keywords() - 1) {
            os << ",";
          }
        }
        os << endl;
      }
    }
    else if(ui.GetString("FORMAT") == "FLAT") {
      QString msg = "Flat file must have a name.";
      throw IException(IException::User, msg, _FILEINFO_);
    }
  }
  else {
    QString msg = "Could not project requested position";
    throw IException(IException::Unknown, msg, _FILEINFO_);
  }
}

// Helper function to print out mapfile to session log
void PrintMap() {
  UserInterface &ui = Application::GetUserInterface();

  // Get mapping group from map file
  Pvl userMap;
  userMap.read(ui.GetFileName("MAP"));
  PvlGroup &userGrp = userMap.findGroup("Mapping", Pvl::Traverse);

  //Write map file out to the log
  Isis::Application::GuiLog(userGrp);
}

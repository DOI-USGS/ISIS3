/** This is free and unencumbered software released into the public domain.

The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */

#include "maplab.h"

#include <QString>

#include "Application.h"
#include "Cube.h"
#include "History.h"
#include "IException.h"
#include "Pvl.h"
#include "ProjectionFactory.h"
#include "TProjection.h"

using namespace std;

namespace Isis {

  /**
   * Add or update the Mapping group in the labels of a map-projected cube.
   *
   * The cube is geo-referenced from a user-supplied map file by aligning a known
   * (X,Y) or (LATITUDE,LONGITUDE) coordinate to a given (SAMPLE,LINE) coordinate,
   * computing the UpperLeftCornerX/Y origin, and writing the resulting Mapping
   * group (with both PixelResolution and Scale) into the cube label.
   *
   * @param ui User interface with application parameters
   */
  void maplab(UserInterface &ui) {
    // Open the input cube
    Cube cube;
    cube.open(ui.GetCubeName("FROM"), "rw");

    // Get the map projection file provided by the user
    Pvl userMap;
    userMap.read(ui.GetFileName("MAP"));
    PvlGroup &mapGrp = userMap.findGroup("Mapping", Pvl::Traverse);

    // Error checking to ensure the map projection file provided contains
    // information pertaining to a target, body radius, and longitude direction
    if(!mapGrp.hasKeyword("TargetName")) {
      QString msg = "The given MAP [" + userMap.name() +
                    "] does not have the TargetName keyword.";
      throw IException(IException::User, msg, _FILEINFO_);
    }
    else if(!mapGrp.hasKeyword("EquatorialRadius") ||
            !mapGrp.hasKeyword("PolarRadius")) {
      QString msg = "The given MAP [" + userMap.name() +
                    "] does not have the EquatorialRadius and PolarRadius keywords.";
      throw IException(IException::User, msg, _FILEINFO_);
    }
    else if(!mapGrp.hasKeyword("LongitudeDomain")) {
      QString msg = "The given MAP [" + userMap.name() +
                    "] does not have the LongitudeDomain keyword.";
      throw IException(IException::User, msg, _FILEINFO_);
    }


    // Get user entered option
    QString option = ui.GetString("COORDINATES");

    double x = 0.0;
    double y = 0.0;
    TProjection * proj = (TProjection *) ProjectionFactory::Create(userMap, false);
    if(option == "XY") {
      x = ui.GetDouble("X");
      y = ui.GetDouble("Y");
    }
    else if(option == "LATLON") {
      proj->SetGround(ui.GetDouble("LAT"), ui.GetDouble("LON"));
      x = proj->XCoord();
      y = proj->YCoord();
    }
    else {
      QString message = "Invalid option [" + option + "] for parameter COORDINATES";
      throw IException(IException::User, message, _FILEINFO_);
    }

    double res = 0.0;
    double scale = 0.0;

    if(mapGrp.hasKeyword("PixelResolution")) {
      double localRadius = proj->LocalRadius(proj->TrueScaleLatitude());
      res = mapGrp.findKeyword("PixelResolution");
      scale = (2.0 * Isis::PI * localRadius) / (360.0 * res);
    }
    else if(mapGrp.hasKeyword("Scale")) {
      double localRadius = proj->LocalRadius(proj->TrueScaleLatitude());
      scale = mapGrp.findKeyword("Scale");
      res = (2.0 * Isis::PI * localRadius) / (360.0 * scale);
    }
    else {
      QString msg = "The given MAP[" + userMap.name() +
                   "] does not have the PixelResolution or Scale keywords.";
      throw IException(IException::User, msg, _FILEINFO_);
    }

    //Read in line and sample inputs
    double line = ui.GetDouble("LINE");
    double samp = ui.GetDouble("SAMPLE");
    x = x - res * (samp - 0.5);
    y = y + res * (line - 0.5);

    //add origen values to Mapping Group
    mapGrp.addKeyword(PvlKeyword("UpperLeftCornerX", toString(x), "meters"), Pvl::Replace);
    mapGrp.addKeyword(PvlKeyword("UpperLeftCornerY", toString(y), "meters"), Pvl::Replace);

    if(!mapGrp.hasKeyword("PixelResolution")) {
      mapGrp.addKeyword(PvlKeyword("PixelResolution", toString(res), "meters"));
    }
    if(!mapGrp.hasKeyword("Scale")) {
      mapGrp.addKeyword(PvlKeyword("Scale", toString(scale), "pixels/degree"));
    }


    // Output the mapping group used to the Gui session log. GuiLog and the
    // History entry below both rely on the Application singleton, so they are
    // guarded for application execution (iApp set). Under direct callable/test
    // use iApp is null and these steps are skipped, mirroring
    // Process::WriteHistory.
    if(iApp != NULL) {
      Application::GuiLog(userMap);
    }

    // Extract label from cube file
    Pvl *label = cube.label();
    PvlObject &o = label->findObject("IsisCube");
    // Add Mapping Group to input cube
    if(o.hasGroup("Mapping")) {
      o.deleteGroup("Mapping");
    }
    o.addGroup(mapGrp);

    // keep track of change to labels in history
    if(iApp != NULL) {
      History hist = cube.readHistory();
      hist.AddEntry();
      cube.write(hist);
    }

    cube.close();
  }
}

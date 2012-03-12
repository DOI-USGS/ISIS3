#include "Isis.h"

#include "Application.h"
#include "Cube.h"
#include "History.h"
#include "Pvl.h"
#include "PvlGroup.h"
#include "PvlObject.h"
#include "SubArea.h"
#include "Table.h"
#include "UserInterface.h"

#include <cmath>

using namespace std;
using namespace Isis;

bool copyGroup(Pvl * source, Pvl * mergeTo, iString name);
bool copyBlob(Cube * from, Cube * to, iString type, iString name, iString fname);

void IsisMain() {
  UserInterface &ui = Application::GetUserInterface();

  // Make cube and open Read/Write
  Cube inOut;
  inOut.open(ui.GetFilename("From"),"rw");

  Pvl * mergeTo = NULL;
  Pvl * source = NULL;
  iString sourceFileName = ui.GetFilename("Source");
  mergeTo = inOut.getLabel();
  source = new Pvl(sourceFileName);

  // We have 3 possible running options, those are
  // We received labels as SOURCE
  //   This is the all out blind copy, no checks are possible high possiblity
  //   of making an f-ed up cube, can only protect users so far.
  // We received a cube of matching size
  //   The safest option
  // We received a cube of equal sample and line scale
  //   If sample and line scales don't match, we stop, depending on
  //   what they choose to copy.
  Cube sourceCube;
  bool isACube = true;
  if (!source->HasObject("IsisCube") || !source->FindObject("IsisCube").HasObject("Core")) {
    isACube = false;
  }
  else {
    // We may or may not be using sourceCube later
    sourceCube.open(sourceFileName,"r");
    if (source != NULL) {
      delete source;
      source = NULL;
    }
    source = sourceCube.getLabel();
  }

  // Check if we need an alpha cube group.
  bool sameSize = false;
  bool xyScaleMatch = false;
  int sourceSamps = -1;
  int sourceLines = -1;
  int outSamps = -1;
  int outLines = -1;
  double sampScale = 0.0;
  double lineScale = 0.0;

  // Check, do we have the same size cubes? how about same sample line scale?
  if (isACube) {
    sourceSamps = sourceCube.getSampleCount();
    sourceLines = sourceCube.getLineCount();
    outSamps = inOut.getSampleCount();
    outLines = inOut.getLineCount();
    sampScale = (double)outSamps/(double)sourceSamps;
    lineScale = (double)outLines/(double)sourceLines;
    if (isACube) {
      if (sourceSamps == outSamps && sourceLines == outLines) {
        sameSize = true;
      }
      if (abs(sampScale - lineScale) < 1e-14) {
        lineScale = sampScale;
        xyScaleMatch = true;
      }
    }
  }

  // We'll be recording everything that we copy in here
  PvlGroup results("Results");

  // Copy Instruments group if requested
  if (ui.GetBoolean("Instrument")) {
    // If we can't check safety, plow onward
    if (isACube && !xyScaleMatch) {
      // Can't copy Instrument group if the the scale factors don't match
      string msg = "Cannot copy Instruments group when the sample scaling"
                   " factor and line scaling factor do not match";
      throw IException(IException::User, msg, _FILEINFO_);
    }
    bool success = copyGroup(source, mergeTo, "Instrument");
    results += PvlKeyword("Instrument", success ? "true" : "false");
  }

  // Copy BandBin group
  if (ui.GetBoolean("Bandbin")) {
    if (isACube) {
      // If the number of bands doesn't match, we can't continue
      if (inOut.getBandCount() != sourceCube.getBandCount()) {
        string msg = "Cannot copy BandBin group when the number of bands does"
                     " not match";
        throw IException(IException::User, msg, _FILEINFO_);
      }
    }
    bool success = copyGroup(source, mergeTo, "Bandbin");
    results += PvlKeyword("BandBin", success ? "true" : "false");
  }

  // Kernels group, no safetys here
  // If they exist, we'll also be copying the 4 tables associated with kernels
  if (ui.GetBoolean("Kernels")) {
    bool success = copyGroup(source, mergeTo, "Kernels");
    results += PvlKeyword("Kernels", success ? "true" : "false");

    if (isACube) {
      iString fname = sourceFileName;
      bool insPoint = copyBlob(&sourceCube, &inOut, "InstrumentPointing","Table", sourceFileName);
      bool insPos = copyBlob(&sourceCube, &inOut, "InstrumentPosition","Table", sourceFileName);
      bool bodyRot = copyBlob(&sourceCube, &inOut, "BodyRotation","Table", sourceFileName);
      bool sunPos = copyBlob(&sourceCube, &inOut, "SunPosition","Table", sourceFileName);

      results += PvlKeyword("Table:InstrumentPointing", insPoint ? "true" : "false");
      results += PvlKeyword("Table:InstrumentPosition", insPos ? "true" : "false");
      results += PvlKeyword("Table:BodyRotation", bodyRot ? "true" : "false");
      results += PvlKeyword("Table:SunPosition", sunPos ? "true" : "false");
    }
    else {
      results += PvlKeyword("Table:InstrumentPointing", "false");
      results += PvlKeyword("Table:InstrumentPosition", "false");
      results += PvlKeyword("Table:BodyRotation", "false");
      results += PvlKeyword("Table:SunPosition", "false");
    }
  }

  // Mapping group
  if (ui.GetBoolean("Mapping")) {
    if (isACube && !xyScaleMatch) {
      // Can't copy Mapping group if the the scale factors don't match
      string msg = "Cannot copy Mapping group when the sample scaling"
                   " factor and line scaling factor do not match";
      throw IException(IException::User, msg, _FILEINFO_);
    }
    bool success = copyGroup(source, mergeTo, "Mapping");
    results += PvlKeyword("Mapping", success ? "true" : "false");
  }

  // Radiometry group
  if (ui.GetBoolean("Radiometry")) {
    bool success = copyGroup(source, mergeTo, "Radiometry");
    results += PvlKeyword("Radiometry", success ? "true" : "false");
  }

  // Polygons Blob
  if (ui.GetBoolean("Polygon")) {
    if (isACube) {
      bool success = copyBlob(&sourceCube, &inOut, "Footprint", "Polygon", sourceFileName);
      results += PvlKeyword("Polygon:Footprint", success ? "true" : "false");
    }
    else {
      results += PvlKeyword("Polygon:Footprint", "false");
    }
  }

  // Camera Statistics Blob
  if (ui.GetBoolean("Camstats")) {
    if (isACube) {
      bool success = copyBlob(&sourceCube, &inOut, "CameraStatistics", "Table", sourceFileName);
      results += PvlKeyword("Table:CameraStatistics", success ? "true" : "false");
    }
    else {
      results += PvlKeyword("Table:CameraStatistics", "false");
    }
  }

  // Any other requested groups
  if (ui.WasEntered("Groups")) {
    QString grps = iString(ui.GetString("Groups")).Remove(" ").ToQt();
    QStringList list = grps.split(",");
    QString grp;
    foreach (grp, list) {
      if (grp.size() != 0) {
        bool success = copyGroup(source, mergeTo, grp.toStdString());
        results += PvlKeyword(grp.toStdString(), success ? "true" : "false");
      }
    }
  }

  // Any other requested blobs
  // Expected format is: <Object name>:<Name keyword>
  if (ui.WasEntered("Blobs")) {
    QString blobs = iString(ui.GetString("Blobs")).Remove(" ").ToQt();
    QStringList list = blobs.split(",");
    QString blob;
    foreach (blob, list) {
      if (isACube) {
        QStringList brk = blob.split(":");
        if (brk.size() != 2) {
          string msg = "The blob name [" + blob.toStdString() + "] is"
                       " improperly formatted";
          throw IException(IException::User, msg, _FILEINFO_);
        }
        bool success = copyBlob(&sourceCube, &inOut, brk[1].toStdString(),
                         brk[0].toStdString(), sourceFileName);
        results += PvlKeyword(blob.toStdString(), success ? "true" : "false");
      }
      else {
        results += PvlKeyword(blob.toStdString(), "false");
      }
    }
  }

  if (!sameSize) {
    // Delete old AlphaCube group if it did exist, it'll just screw us up.
    if (mergeTo->FindObject("IsisCube").HasGroup("AlphaCube"))
      mergeTo->FindObject("IsisCube").DeleteGroup("AlphaCube");

    SubArea subarea;
    subarea.SetSubArea(sourceSamps, sourceLines, 1, 1, sourceSamps, sourceLines, lineScale, sampScale);
    subarea.UpdateLabel(&inOut, &inOut, results);
  }

  // Add History
  bool found = false;
  for (int i = 0; i < mergeTo->Objects() && !found; i++) {
    if (mergeTo->Object(i).IsNamed("History")) {
      History his((string)mergeTo->Object(i)["Name"]);
      inOut.read(his);
      his.AddEntry();
      inOut.write(his);
      found = true;
    }
  }
  if (!found) {
    History his("IsisCube");
    his.AddEntry();
    inOut.write(his);
  }

  inOut.close();
  sourceCube.close();

  Application::Log(results);
}

// Copy a group from the IsisCube object in one cube to the other
// If it exists in the source, we'll copy it, if it exists in the
// mergeTo Pvl, we'll overwrite it.
bool copyGroup(Pvl * source, Pvl * mergeTo, iString name) {
  try {
    // The call we're looking to get an exception on is the one just below.
    PvlGroup & toCopy = source->FindGroup(name, Pvl::Traverse);
    PvlObject & isiscube = mergeTo->FindObject("IsisCube");
    if (isiscube.HasGroup(name))
      isiscube.DeleteGroup(name);
    isiscube.AddGroup(toCopy);
    return true;
  }
  catch (IException &) {
    return false;
  }
}

bool copyBlob(Cube * from, Cube * to, iString name, iString type, iString fname) {
  try {
    Blob blob(name, type, fname);
    from->read(blob);
    to->write(blob);
    return true;
  }
  catch (IException &) {
    return false;
  }
}


#include "Isis.h"

#include "Application.h"
#include "Cube.h"
#include "CubeAttribute.h"
#include "FileName.h"
#include "Portal.h"
#include "ProcessByLine.h"
#include "Pvl.h"
#include "SpecialPixel.h"
#include "UserInterface.h"

#include <QString>

using namespace std;
using namespace Isis;

// Taken from ProcessMosaic
const int FLOAT_MIN = -16777215;
const int FLOAT_MAX = 16777216;

void findTrackBand(UserInterface ui, QString &copyBands, QString &trackBand);
void createMosaicCube(UserInterface ui, QString bands);
void createTrackCube(UserInterface ui, QString trackingBand);
void copyPixels(Buffer &in, Buffer &out);
void copyTrackPixels(Buffer &in, Buffer &out);

void IsisMain() {
  UserInterface &ui = Application::GetUserInterface();
  QString copyBands;
  QString trackBand;
  findTrackBand(ui, copyBands, trackBand);
  createMosaicCube(ui, copyBands);
  createTrackCube(ui, trackBand);
}


/**
 * Finds the index of the tracking band in the BandBin group and stores the index
 * in trackBand. The other bands' indices are stored in copyBands.
 * We store the indices in QStrings because we use them as cube attributes when
 * we process the mosaic and tracking cubes.
 *
 * @param ui        The user interface
 * @param copyBands Indices of the bands to copy over to the mosaic cube
 * @param trackBand Index of the tracking band
 */
void findTrackBand(UserInterface ui, QString &copyBands, QString &trackBand) {
  Cube inputCube = Cube(ui.GetFileName("FROM"));
  Pvl *inputLabel = inputCube.label();

  if (inputLabel->hasObject("IsisCube")) {
    PvlObject &cubeObject = inputLabel->findObject("IsisCube");
    if (cubeObject.hasGroup("BandBin")) {
      PvlGroup &bandBinGroup = cubeObject.findGroup("BandBin");
      PvlKeyword &currentKeyword = bandBinGroup[0];
      for (int i = 0; i < currentKeyword.size(); i++) {
        if (currentKeyword[i] != "TRACKING") {
          copyBands += QString::number(i + 1); // Make it 1 based
          copyBands += ",";
        }
        else {
          trackBand = QString::number(i + 1);
        }
      }
    }
  }
  if (trackBand == "") {
    QString msg = "The input cube [" + ui.GetFileName("FROM") + "] does not have a tracking band.";
    msg += " If you want to create a tracking cube, run a mosaic program.";
    throw IException(IException::Programmer, msg, _FILEINFO_);
  }
}


/**
 * Creates the mosaic cube by copying the input cube without the tracking band.
 * Then, removes the tracking table from the mosaic label and adds a group pointing
 * to the tracking cube.
 *
 * @param ui    The user interface
 * @param bands The indices of the bands that are not the tracking band
 */
void createMosaicCube(UserInterface ui, QString bands) {
  ProcessByLine p;
  CubeAttributeInput inAtt = CubeAttributeInput("+" + bands);
  p.SetInputCube(ui.GetFileName("FROM"), inAtt);
  p.SetOutputCube("TO");
  p.StartProcess(copyPixels);
  p.EndProcess();

  Cube mosaicCube;
  try {
    mosaicCube.open(ui.GetFileName("TO"),"rw");
  }
  catch (IException &e) {
    throw IException(IException::User,
                     "Unable to open the file [" + ui.GetFileName("TO") + "] as a cube.",
                     _FILEINFO_);
  }
  Pvl *mosaicLabel = mosaicCube.label();

  if (mosaicLabel->hasObject("Table")) {
    PvlObject &tableObject = mosaicLabel->findObject("Table");
    if (tableObject.hasKeyword("Name")) {
      PvlKeyword &tableName = tableObject.findKeyword("Name", Pvl::Traverse);
      if (tableName[0] == "InputImages") {
        mosaicLabel->deleteObject("Table");
      }
    }
  }

  // Add Tracking Group to the mosaic cube
  PvlGroup trackingGroup = PvlGroup("Tracking");
  PvlKeyword trackingName = PvlKeyword("Filename");
  FileName cubeName = FileName(ui.GetFileName("TO"));
  trackingName.setValue(cubeName.baseName() + "_tracking.cub"); //Strip off path and add _tracking
  trackingGroup.addKeyword(trackingName);
  mosaicLabel->addGroup(trackingGroup);

  mosaicCube.close();
}


/**
 * Creates the tracking cube by copying the input cube with only the tracking band.
 * Then, goes through each pixel and subtracts the input cube's pixel type's min value
 * because ProcesMosaic used to add the min value to each pixel.
 *
 * @param ui           The user interface
 * @param trackingBand The index of the tracking band
 */
void createTrackCube(UserInterface ui, QString trackBand) {
  ProcessByLine p;

  CubeAttributeInput inAtt = CubeAttributeInput("+" + trackBand);
  p.SetInputCube(ui.GetFileName("FROM"), inAtt);

  FileName cubeName = FileName(ui.GetFileName("TO"));
  // Strip off any extensions and add _tracking
  QString trackingName = cubeName.path() + "/" + cubeName.baseName() + "_tracking.cub";
  Cube inputCube = Cube(ui.GetFileName("FROM"));
  int numSample = inputCube.sampleCount();
  int numLine = inputCube.lineCount();

  QString outAttString = "+UNSIGNEDINTEGER+" + QString::number(VALID_MINUI4) + ":";
  outAttString += QString::number(VALID_MAXUI4);
  CubeAttributeOutput outAtt = CubeAttributeOutput(outAttString);

  p.SetOutputCube(trackingName, outAtt, numSample, numLine);

  p.StartProcess(copyTrackPixels);
  p.EndProcess();
}


/**
 * Copies DN's from the input cube to the mosaic cube.
 *
 * @param in  Input cube
 * @param out Mosaic cube
 */
void copyPixels(Buffer &in, Buffer &out) {
  for (int i = 0; i < in.size(); i++) {
    out[i] = in[i];
  }
}


/**
 * Copies DN's from the input cube to the tracking cube.
 * Because each pixel is offsetted by the min value of the input cube's pixel type,
 * we have to get that min value and subtract it from each pixel. Then, we add the min
 * of an unsigned int to each pixel for the new offset.
 * The default value is set in ProcessMosaic for pixels who are not from a cube.
 * If a pixel's value is the default value, we set it to Null.
 *
 * @param in  Input cube
 * @param out Tracking cube
 */
void copyTrackPixels(Buffer &in, Buffer &out) {
  int offset = 0;
  int defaultVal = 0;
  switch (SizeOf(in.PixelType())) {
    case 1:
      offset = VALID_MIN1;
      defaultVal = NULL1;
      break;

    case 2:
      offset = VALID_MIN2;
      defaultVal = NULL2;
      break;

    case 4:
      offset = FLOAT_MIN;
      defaultVal = INULL4;
      break;

    default:
      QString msg = "Invalid Pixel Type [" + QString(in.PixelType()) + "]";
      throw IException(IException::Programmer, msg, _FILEINFO_);
  }

  for (int i = 0; i < in.size(); i++) {
    if (in[i] == (float) defaultVal) {
      out[i] = NULLUI4;  // Set to the unsigned 4 byte Null value
    }
    else {
      out[i] = ((int) in[i]) - offset + VALID_MINUI4;
    }
  }
}

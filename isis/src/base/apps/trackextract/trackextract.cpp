#include "Isis.h"

#include "Application.h"
#include "Cube.h"
#include "CubeAttribute.h"
#include "Portal.h"
#include "ProcessByLine.h"
#include "Pvl.h"
#include "UserInterface.h"

#include <QString>

using namespace std;
using namespace Isis;

#define FLOAT_MIN -16777215

void findTrackBand(UserInterface ui, QString &copyBands, QString &trackBand);
void createMosaicCube(UserInterface ui, QString bands);
void createTrackCube(UserInterface ui, QString trackingBand);
void processCube(Buffer &in, Buffer &out);

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
  Pvl *originalLabel = inputCube.label();

  if (originalLabel->hasObject("IsisCube")) {
    PvlObject &cubeObject = originalLabel->findObject("IsisCube");
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
  p.StartProcess(processCube);
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

  // Find the Table object and remove it from the mosaic cube
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
  trackingName.setValue(ui.GetFileName("TO"));
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

  QString cubeName = ui.GetFileName("TO") + "_tracking";
  // CubeAttributeOutput outAtt = CubeAttributeOutput("+Real");
  Cube inputCube = Cube(ui.GetFileName("FROM"));
  int numSample = inputCube.sampleCount();
  int numLine = inputCube.lineCount();
  CubeAttributeOutput outAtt = CubeAttributeOutput();
  p.SetOutputCube(cubeName, outAtt, numSample, numLine);

  p.StartProcess(processCube);
  p.EndProcess();

  Cube trackCube;
  try {
    trackCube.open(cubeName,"rw");
  }
  catch (IException &e) {
    throw IException(IException::User,
                     "Unable to open the file [" + cubeName + "] as a cube.",
                     _FILEINFO_);
  }

  // Convert the old DN to the new DN by subtracting the minimum value of the pixel type
  int offset = 0;
  int defaultVal = 0;
  switch (SizeOf(inputCube.pixelType())) {
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
    QString msg = "Invalid Pixel Type [" + QString(trackCube.pixelType()) + "]";
    throw IException(IException::Programmer, msg, _FILEINFO_);
  }

  Portal trackPortal(trackCube.sampleCount(), 1, trackCube.pixelType());
  for (int lineCount = 1; lineCount <= trackCube.lineCount(); lineCount++) {
    trackPortal.SetPosition(1, lineCount, 1); //Cube has only 1 band
    trackCube.read(trackPortal);
    for (int pixel = 0; pixel < trackPortal.size(); pixel++) {
      if (trackPortal[pixel] == (float) defaultVal) { // Skip if it is not part of the mosaic.
          trackPortal[pixel] = defaultVal;
      }
      else {
        trackPortal[pixel] = (int) trackPortal[pixel] - offset;
      }
    }
    trackCube.write(trackPortal);
  }
  trackCube.close();
}

/**
 * Copies DN's from the input cube to the output cube.
 *
 * @param in  Input cube
 * @param out Output cube
 */
void processCube(Buffer &in, Buffer &out) {
  for(int i = 0; i < in.size(); i++) {
    out[i] = in[i];
  }
}

#include "Isis.h"

#include <QString>

#include "Application.h"
#include "Cube.h"
#include "CubeAttribute.h"
#include "FileName.h"
#include "ProcessByLine.h"
#include "Pvl.h"
#include "SpecialPixel.h"
#include "TrackingTable.h"
#include "UserInterface.h"

using namespace std;
using namespace Isis;

// Taken from ProcessMosaic
const int FLOAT_MIN = -16777215;

void findTrackBand(QString inputName, QVector<QString> &copyBands, int &trackBand);
void copyPixels(Buffer &in, Buffer &out);


/**
 * Functor that copies DN's from the input cube to the tracking cube.
 * Because each pixel is offsetted by the min value of the input cube's pixel type,
 * we have to subtract the offset from each pixel. Then, we add the min
 * of an unsigned int to each pixel for the new offset.
 * The default value is set in ProcessMosaic for pixels who are not taken from a cube.
 * If a pixel's value is the default value, we set it to Null.
 *
 * @author 2018-07-30 Kaitlyn Lee
 * @internal
 *   @history 2018-07-30 Kaitlyn Lee - Original Version.
 *
 */
class CopyPixelsFunctor {
  private:
    int m_offset;
    int m_defaultValue;

  public:
    /**
     * Default Constructor
     * @param offset       The minimum value of the input cube's pixel type
     * @param defaultValue Value used for pixels that are not taken from a cube
     */
    CopyPixelsFunctor(int offset, int defaultValue) {
      m_offset = offset;
      m_defaultValue = defaultValue;
    }

    /**
     * Copies DN's from the input cube to the tracking cube, subtracts the old offset, and adds
     * the new offset to each pixel.
     *
     * @param in  Input cube
     * @param out Mosaic cube
     */
    void operator()(Buffer &in, Buffer &out) const {
      for (int i = 0; i < in.size(); i++) {
        if (in[i] == (float) m_defaultValue) {
          out[i] = Isis::Null;
        }
        else {
          out[i] = ((int) in[i]) - m_offset + VALID_MINUI4;
        }
      }
    }
};


void IsisMain() {
  UserInterface &ui = Application::GetUserInterface();
  QString inputName = ui.GetCubeName("FROM");

  // Confirm that the input mosaic is of pixel-type "Real" as trackextract does not work on other
  // bit types due to corruption of these files
  Cube inputCube(inputName);
  PixelType pixelType = inputCube.pixelType();
  if (pixelType != Real) {
    QString msg = "The input mosaic [" + inputName + "] is of pixel type ["
    + PixelTypeName(pixelType) + "]. This application only works for mosaics of pixel type Real.";
    throw IException(IException::User, msg, _FILEINFO_);
  }

  QVector<QString> copyBands;
  int trackBand;
  findTrackBand(inputName, copyBands, trackBand);

  // Begin process to create the output mosaic file
  ProcessByLine p;

  CubeAttributeInput inAtt = CubeAttributeInput();
  inAtt.setBands(std::vector<QString>(copyBands.begin(), copyBands.end()));

  p.SetInputCube(inputName, inAtt);
  Cube *mosaicCube = p.SetOutputCube("TO");
  p.StartProcess(copyPixels);

  if (!mosaicCube->deleteBlob("InputImages", "Table")) {
    QString msg = "The input cube [" + inputName + "] does not have a tracking table.";
    throw IException(IException::Programmer, msg, _FILEINFO_);
  }

  p.EndProcess();

  // Begin process for creating tracking file
  inAtt = CubeAttributeInput("+" + QString::number(trackBand));
  p.SetInputCube(inputName, inAtt);

  FileName outputFileName = FileName(ui.GetCubeName("TO"));

  // Strip off any extensions and add _tracking
  QString trackingFileString = outputFileName.path() + "/" + outputFileName.baseName() + "_tracking";

  int numSample = inputCube.sampleCount();
  int numLine = inputCube.lineCount();

  // Create cube attributes off of the original cube
  CubeAttributeOutput outAtt = ui.GetOutputAttribute("TO");
  outAtt.setPixelType(UnsignedInteger);
  outAtt.setMinimum(VALID_MINUI4);
  outAtt.setMaximum(VALID_MAXUI4);

  Cube *trackCube = p.SetOutputCube(trackingFileString, outAtt, numSample, numLine);

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
      QString msg = "Invalid Pixel Type [" + QString::number(inputCube.pixelType()) + "]";
      throw IException(IException::Programmer, msg, _FILEINFO_);
  }
  CopyPixelsFunctor copyTrackPixels(offset, defaultVal);

  p.ProcessCube(copyTrackPixels);

  // Create new tracking table with updated data and delete the old table
  if (trackCube->hasTable("InputImages")) {
    Table oldTable = trackCube->readTable("InputImages");
    trackCube->deleteBlob("Table", "InputImages");

    TrackingTable newTrackTable(oldTable);
    Table newTable = newTrackTable.toTable();
    trackCube->write(newTable);
  }
  else {
    QString msg = "The tracking cube [" + trackCube->fileName() + "] does not have a tracking table.";
    throw IException(IException::Programmer, msg, _FILEINFO_);
  }

  mosaicCube->reopen("rw");

  // Add Tracking Group to the mosaic cube
  PvlGroup trackingGroup = PvlGroup("Tracking");
  PvlKeyword trackingName = PvlKeyword("Filename");
  FileName trackingFileName = FileName(trackCube->fileName());
  trackingName.setValue(trackingFileName.name());
  trackingGroup.addKeyword(trackingName);
  mosaicCube->putGroup(trackingGroup);
  mosaicCube->close();

  p.EndProcess();
}


/**
 * Finds the index of the tracking band in the BandBin group and stores the index
 * in trackBand. The other bands' indices are stored in copyBands.
 * We store the indices in QStrings because we use them as cube attributes when
 * we process the mosaic and tracking cubes.
 *
 * @param inputName The name of the input cube with the tracking band
 * @param copyBands Indices of the bands to copy over to the mosaic cube
 * @param trackBand Index of the tracking band
 */
void findTrackBand(QString inputName, QVector<QString> &copyBands, int &trackBand) {
  Cube inputCube = Cube(inputName);
  if (inputCube.hasGroup("BandBin")) {
    PvlGroup &bandBinGroup = inputCube.group("BandBin");
    try {
      PvlKeyword &currentKeyword = bandBinGroup[0];
      trackBand = -1;
      for (int i = 0; i < currentKeyword.size(); i++) {
        if (currentKeyword[i] != "TRACKING") {
          copyBands.append(QString::number(i + 1)); // Make it 1 based
        }
        else {
          trackBand = i + 1;
        }
      }
    }
    catch (IException &e) {
      QString msg = "The input cube [" + inputName + "] does not have any keywords";
      msg += " in the BandBin group. Make sure TRACKING is a keyword in the BandBin group.";
      throw IException(IException::Programmer, msg, _FILEINFO_);
    }
  }
  else {
    QString msg = "The input cube [" + inputName + "] does not have a BandBin group.";
    throw IException(IException::Programmer, msg, _FILEINFO_);
  }
  if (trackBand == -1) {
    QString msg = "The input cube [" + inputName + "] does not have a tracking band.";
    msg += " If you want to create a tracking cube, run a mosaic program.";
    throw IException(IException::Programmer, msg, _FILEINFO_);
  }
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

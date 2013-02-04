/**
 * @file
 * $Revision: 1.4 $
 * $Date: 2007/09/14 16:44:07 $
 *
 *   Unless noted otherwise, the portions of Isis written by the USGS are
 *   public domain. See individual third-party library and package descriptions
 *   for intellectual property information, user agreements, and related
 *   information.
 *
 *   Although Isis has been used by the USGS, no warranty, expressed or
 *   implied, is made by the USGS as to the accuracy and functioning of such
 *   software and related material nor shall the fact of distribution
 *   constitute any such warranty, and no responsibility is assumed by the
 *   USGS in connection therewith.
 *
 *   For additional information, launch
 *   $ISISROOT/doc//documents/Disclaimers/Disclaimers.html
 *   in a browser or see the Privacy &amp; Disclaimers page on the Isis website,
 *   http://isis.astrogeology.usgs.gov, and the USGS privacy and disclaimers on
 *   http://www.usgs.gov/privacy.html.
 */

#include "CubeTileHandler.h"

#include <QFile>

#include "IException.h"
#include "Pvl.h"
#include "PvlObject.h"
#include "PvlKeyword.h"
#include "RawCubeChunk.h"

using namespace std;

namespace Isis {
  /**
   * Construct a tile handler. This will determine a good chunk size to put
   *   into the output cube.
   *
   * @param dataFile The file with cube DN data in it
   * @param virtualBandList The mapping from virtual band to physical band, see
   *          CubeIoHandler's description.
   * @param labels The Pvl labels for the cube
   * @param alreadyOnDisk True if the cube is allocated on the disk, false
   *          otherwise
   */
  CubeTileHandler::CubeTileHandler(QFile * dataFile,
      const QList<int> *virtualBandList, const Pvl &labels, bool alreadyOnDisk)
      : CubeIoHandler(dataFile, virtualBandList, labels, alreadyOnDisk) {

    const PvlObject &core = labels.FindObject("IsisCube").FindObject("Core");

    if(core.HasKeyword("Format")) {
      setChunkSizes(core["TileSamples"], core["TileLines"], 1);
    }
    else {
      // up to 1MB chunks
      int sampleChunkSize =
          findGoodSize(512 * 4 / SizeOf(pixelType()), sampleCount());
      int lineChunkSize =
          findGoodSize(512 * 4 / SizeOf(pixelType()), lineCount());

      setChunkSizes(sampleChunkSize, lineChunkSize, 1);
    }
  }


  /**
   * Writes all data from memory to disk.
   */
  CubeTileHandler::~CubeTileHandler() {
    clearCache();
  }


  /**
   * Update the cube labels so that this cube indicates what tile size it used.
   *
   * @param labels The "Core" object in this Pvl will be updated
   */
  void CubeTileHandler::updateLabels(Pvl &labels) {
    PvlObject &core = labels.FindObject("IsisCube").FindObject("Core");
    core.AddKeyword(PvlKeyword("Format", "Tile"),
                    PvlContainer::Replace);
    core.AddKeyword(PvlKeyword("TileSamples", toString(getSampleCountInChunk())),
                    PvlContainer::Replace);
    core.AddKeyword(PvlKeyword("TileLines", toString(getLineCountInChunk())),
                    PvlContainer::Replace);
  }


  void CubeTileHandler::readRaw(RawCubeChunk &chunkToFill) {
    BigInt startByte = getTileStartByte(chunkToFill);

    bool success = false;

    QFile * dataFile = getDataFile();
    if(dataFile->seek(startByte)) {
      QByteArray binaryData = dataFile->read(chunkToFill.getByteCount());

      if(binaryData.size() == chunkToFill.getByteCount()) {
        chunkToFill.setRawData(binaryData);
        success = true;
      }
    }

    if(!success) {
      IString msg = "Reading from the file [" + dataFile->fileName() + "] "
          "failed with reading [" +
          QString::number(chunkToFill.getByteCount()) +
          "] bytes at position [" + QString::number(startByte) + "]";
      throw IException(IException::Io, msg, _FILEINFO_);
    }
  }


  void CubeTileHandler::writeRaw(const RawCubeChunk &chunkToWrite) {
    BigInt startByte = getTileStartByte(chunkToWrite);
    bool success = false;

    QFile * dataFile = getDataFile();
    if(dataFile->seek(startByte)) {
      BigInt dataWritten = dataFile->write(chunkToWrite.getRawData());

      if(dataWritten == chunkToWrite.getByteCount()) {
        success = true;
      }
    }

    if(!success) {
      IString msg = "Writing to the file [" + dataFile->fileName() + "] "
          "failed with writing [" +
          QString::number(chunkToWrite.getByteCount()) +
          "] bytes at position [" + QString::number(startByte) + "]";
      throw IException(IException::Io, msg, _FILEINFO_);
    }
  }


  /**
   * This is a helper method that tries to compute a good tile size for
   *   one of the cube's dimensions (sample or line). Band tile size is always
   *   1 for this format currently.
   *
   * @param maxSize The largest allowed size
   * @param dimensionSize The cube's size in the dimension we're figuring out
   *     (that is, number of samples or number of lines).
   * @return The tile size that should be used for the dimension
   */
  int CubeTileHandler::findGoodSize(int maxSize, int dimensionSize) const {
    int ideal = 128;

    if(dimensionSize <= maxSize) {
      ideal = dimensionSize;
    }
    else {
      int greatestDividend = maxSize;

      while(greatestDividend > ideal) {
        if(dimensionSize % greatestDividend == 0) {
          ideal = greatestDividend;
        }

        greatestDividend --;
      }
    }

    return ideal;
  }


  /**
   * This is a helper method that goes from chunk to file position.
   *
   * @param chunk The chunk to locate in the file.
   * @returns The position to start reading or writing at
   */
  BigInt CubeTileHandler::getTileStartByte(const RawCubeChunk &chunk) const {
    return getDataStartByte() + getChunkIndex(chunk) * getBytesPerChunk();
  }
}

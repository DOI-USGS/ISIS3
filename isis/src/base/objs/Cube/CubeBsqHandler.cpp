/**
 * @file
 * $Revision: 1.6 $
 * $Date: 2008/09/03 16:21:02 $
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

#include "CubeBsqHandler.h"

#include <iostream>

#include <QFile>

#include "IException.h"
#include "Pvl.h"
#include "PvlKeyword.h"
#include "PvlObject.h"
#include "RawCubeChunk.h"

using namespace std;

namespace Isis {
  /**
   * Construct a BSQ IO handler. This will determine a good chunk size to use
   *   that does not result in the cube being enlarged or misordered.
   *
   * @param dataFile The file with cube DN data in it
   * @param virtualBandList The mapping from virtual band to physical band, see
   *        CubeIoHandler's description.
   * @param labels The Pvl labels for the cube
   * @param alreadyOnDisk True if the cube is allocated on the disk, false
   *     otherwise
   */
  CubeBsqHandler::CubeBsqHandler(QFile * dataFile,
      const QList<int> *virtualBandList, const Pvl &labels, bool alreadyOnDisk)
      : CubeIoHandler(dataFile, virtualBandList, labels, alreadyOnDisk) {
    int numSamplesInChunk = sampleCount();
    int numLinesInChunk = 1;

    // The chunk size must evenly divide into the cube size...
    if(lineCount() < 1024)
      numLinesInChunk = lineCount();
    else {
      int attemptedSize = 1024;

      while(numLinesInChunk == 1 && attemptedSize > 1) {
        if(lineCount() % attemptedSize == 0)
          numLinesInChunk = attemptedSize;
        else
          attemptedSize /= 2;
      }
    }

    setChunkSizes(numSamplesInChunk, numLinesInChunk, 1);
  }


  /**
   * The destructor writes all cached data to disk.
   */
  CubeBsqHandler::~CubeBsqHandler() {
    clearCache();
  }


  void CubeBsqHandler::updateLabels(Pvl &label) {
    PvlObject &core = label.findObject("IsisCube").findObject("Core");
    core.addKeyword(PvlKeyword("Format", "BandSequential"),
                    PvlContainer::Replace);
  }


  void CubeBsqHandler::readRaw(RawCubeChunk &chunkToFill) {
    BigInt startByte = getChunkStartByte(chunkToFill);

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


  void CubeBsqHandler::writeRaw(const RawCubeChunk &chunkToWrite) {
    BigInt startByte = getChunkStartByte(chunkToWrite);

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
   * This is a helper method that goes from chunk to file position.
   *
   * @param chunk The chunk to locate in the file.
   * @return The file position to start reading or writing at
   */
  BigInt CubeBsqHandler::getChunkStartByte(const RawCubeChunk &chunk) const {
    return getDataStartByte() + getChunkIndex(chunk) * getBytesPerChunk();
  }
}

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
    QList<int> primeFactors;
    
    // we want our chunk sizes to be less than 1GB
    int sizeLimit = 1024 * 1024 * 1024;
    int maxNumLines = (sizeLimit) / (SizeOf(pixelType()) * numSamplesInChunk);
    
    // we've exceed our sizeLimit; increase our limit so we can process an entire line
    if (maxNumLines == 0) 
      maxNumLines = 1;
    
    numLinesInChunk = findGoodSize(maxNumLines, lineCount());
    
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
   * This method attempts to compute a good chunk line size. Chunk band size is
   * always 1 and chunk sample size is always number of samples in the cube for this format.
   * 
   * @param maxSize The largest allowed size of a chunk dimension
   * @param dimensionSize The cube's size for the chunk size we are trying to calculate
   *     (number of lines)
   * @return The calculated chunk size for the dimension given
   */
  int CubeBsqHandler::findGoodSize(int maxSize, int dimensionSize) const {
    int chunkDimensionSize;
    
    if (dimensionSize <= maxSize) {
      chunkDimensionSize = dimensionSize;
    }
    else {
      // find largest divisor of dimension size so chunks fit into cube uniformly
      int greatestDivisor = maxSize;
      while (dimensionSize % greatestDivisor > 0) {
        greatestDivisor--;
      }
      chunkDimensionSize = greatestDivisor;
    }
    return chunkDimensionSize;
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

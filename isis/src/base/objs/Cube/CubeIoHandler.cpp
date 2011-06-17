/**
 * @file
 * $Revision: 1.2 $
 * $Date: 2007/01/30 22:12:22 $
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

#include "IsisDebug.h"
#include "CubeIoHandler.h"

#include <algorithm>
#include <cmath>

#include <QFile>
#include <QListIterator>
#include <QMapIterator>
#include <QRect>

#include "Brick.h"
#include "CubeCachingAlgorithm.h"
#include "Displacement.h"
#include "Distance.h"
#include "Endian.h"
#include "EndianSwapper.h"
#include "iException.h"
#include "iString.h"
#include "PixelType.h"
#include "Pvl.h"
#include "PvlGroup.h"
#include "PvlObject.h"
#include "RawCubeChunk.h"
#include "RegionalCachingAlgorithm.h"
#include "SpecialPixel.h"

using namespace std;

namespace Isis {
  /**
   * This is the constructor.
   *
   * @param dataFile The file that contains cube data.
   * @param virtualBandList A list where the indices are the vbands and the
   *          values are the physical bands.
   * @param label The label which contains the "Pixels" and "Core" groups.
   * @param alreadyOnDisk True if the cube exists; false ensures all NULLs are
   *          initialized into the file.
   */
  CubeIoHandler::CubeIoHandler(QFile * dataFile,  QList<int> *virtualBandList,
      const Pvl &label, bool alreadyOnDisk) {
    m_byteSwapper = NULL;
    m_cachingAlgorithms = NULL;
    m_dataIsOnDiskMap = NULL;
    m_rawData = NULL;
    m_virtualBands = NULL;
    m_nullChunkData = NULL;
    m_lastProcessByLineChunks = NULL;

    m_cachingAlgorithms = new QList<CubeCachingAlgorithm *>;
    m_rawData = new QMap<int, RawCubeChunk *>;

    m_cachingAlgorithms->append(new RegionalCachingAlgorithm);

    m_dataFile = dataFile;

    const PvlObject &core = label.FindObject("IsisCube").FindObject("Core");
    const PvlGroup &pixelGroup = core.FindGroup("Pixels");

    iString byteOrderStr = pixelGroup.FindKeyword("ByteOrder")[0];
    m_byteSwapper = new EndianSwapper(
        byteOrderStr.UpCase());
    m_base = pixelGroup.FindKeyword("Base");
    m_multiplier = pixelGroup.FindKeyword("Multiplier");
    m_pixelType = PixelTypeEnumeration(pixelGroup.FindKeyword("Type"));

    if(!m_byteSwapper->willSwap()) {
      delete m_byteSwapper;
      m_byteSwapper = NULL;
    }

    const PvlGroup &dimensions = core.FindGroup("Dimensions");
    m_numSamples = dimensions.FindKeyword("Samples");
    m_numLines = dimensions.FindKeyword("Lines");
    m_numBands = dimensions.FindKeyword("Bands");

    m_startByte = (int)core.FindKeyword("StartByte") - 1;

    m_samplesInChunk = -1;
    m_linesInChunk = -1;
    m_bandsInChunk = -1;

    if(!alreadyOnDisk) {
      m_dataIsOnDiskMap = new QMap<int, bool>;
    }

    setVirtualBands(virtualBandList);
  }


  /**
   * Cleans up all allocated memory. Child destructors must call clearCache()
   *   because we can no longer do IO by the time this destructor is called.
   */
  CubeIoHandler::~CubeIoHandler() {
    ASSERT( m_rawData ? m_rawData->size() == 0 : 1 );

    if(m_dataIsOnDiskMap) {
      delete m_dataIsOnDiskMap;
      m_dataIsOnDiskMap = NULL;
    }

    if (m_cachingAlgorithms) {
      QListIterator<CubeCachingAlgorithm *> it(*m_cachingAlgorithms);
      while (it.hasNext()) {
        delete it.next();
      }
      delete m_cachingAlgorithms;
      m_cachingAlgorithms = NULL;
    }

    if (m_rawData) {
      QMapIterator<int, RawCubeChunk *> it(*m_rawData);
      while (it.hasNext()) {
        // Unwritten data here means it cannot be written :(
        ASSERT(0);
        it.next();

        if(it.value())
          delete it.value();
      }
      delete m_rawData;
      m_rawData = NULL;
    }


    if (m_byteSwapper) {
      delete m_byteSwapper;
      m_byteSwapper = NULL;
    }

    if(m_virtualBands) {
      delete m_virtualBands;
      m_virtualBands = NULL;
    }

    if(m_nullChunkData) {
      delete m_nullChunkData;
      m_nullChunkData = NULL;
    }

    if(m_lastProcessByLineChunks) {
      delete m_lastProcessByLineChunks;
      m_lastProcessByLineChunks = NULL;
    }

    m_dataFile = NULL;
  }


  /**
   * Read cube data from disk into the buffer.
   *
   * This is not const because it caches the read cube chunks from the
   *   disk.
   *
   * @param bufferToFill The buffer to populate with cube data.
   */
  void CubeIoHandler::read(Buffer &bufferToFill) {
    // This works by finding the cube chunks intersecting the buffer,
    //   initializing the buffer to NULL, and then going through the
    //   intersecting cube chunks and putting their respective data
    //   into the buffer. Finally, extra cube chunks in memory are
    //   freed.
    QList<RawCubeChunk *> cubeChunks = findCubeChunks(
        bufferToFill.Sample(), bufferToFill.SampleDimension(),
        bufferToFill.Line(), bufferToFill.LineDimension(),
        bufferToFill.Band(), bufferToFill.BandDimension());

    for(int i = 0; i < bufferToFill.size(); i++)
      bufferToFill[i] = Null;

    RawCubeChunk * fileData;
    foreach (fileData, cubeChunks) {
      writeIntoDouble(*fileData, bufferToFill);
    }

    minimizeCache(cubeChunks, bufferToFill);
  }


  /**
   * Write buffer data into the cube data on disk.
   *
   * This could do no IO if the cube chunks required are already in memory
   *   and our caching algorithms say to not free any of the cube chunks
   *   afterwards.
   *
   * @param bufferToWrite The buffer to get cube data from.
   */
  void CubeIoHandler::write(const Buffer &bufferToWrite) {
    // This works by finding the cube chunks intersecting the buffer,
    //   going through the intersecting cube chunks and putting the buffer
    //   data into the respective chunks. Finally, extra cube chunks in memory
    //   are freed.
    QList<RawCubeChunk *> cubeChunks;

    int bufferSampleCount = bufferToWrite.SampleDimension();
    int bufferLineCount = bufferToWrite.LineDimension();
    int bufferBandCount = bufferToWrite.BandDimension();

    // process by line optimization
    if(m_lastProcessByLineChunks && m_lastProcessByLineChunks->size()) {
      // Not optimized yet, let's see if we can optimize
      if(bufferToWrite.Sample() == 1 &&
         bufferSampleCount == getSampleCount() &&
         bufferLineCount == 1 &&
         bufferBandCount == 1) {
        // We look like a process by line, are we using the same chunks as
        //   before?
        int bufferLine = bufferToWrite.Line();
        int chunkStartLine = (*m_lastProcessByLineChunks)[0]->getStartLine();
        int chunkLines = (*m_lastProcessByLineChunks)[0]->getLineCount();
        int bufferBand = bufferToWrite.Band();
        int chunkStartBand = (*m_lastProcessByLineChunks)[0]->getStartBand();
        int chunkBands = (*m_lastProcessByLineChunks)[0]->getBandCount();

        if(bufferLine >= chunkStartLine &&
           bufferLine <= chunkStartLine + chunkLines - 1 &&
           bufferBand >= chunkStartBand &&
           bufferBand <= chunkStartBand + chunkBands - 1) {
          cubeChunks = *m_lastProcessByLineChunks;
        }
      }
    }

    if(cubeChunks.empty()) {
      cubeChunks = findCubeChunks(
         bufferToWrite.Sample(), bufferSampleCount,
         bufferToWrite.Line(), bufferLineCount,
         bufferToWrite.Band(), bufferBandCount);
    }

    // process by line optimization
    if(bufferToWrite.Sample() == 1 &&
        bufferSampleCount == getSampleCount() &&
        bufferLineCount == 1 &&
        bufferBandCount == 1) {
      if(!m_lastProcessByLineChunks)
        m_lastProcessByLineChunks = new QList<RawCubeChunk *>(cubeChunks);
      else
        *m_lastProcessByLineChunks = cubeChunks;
    }

    for(int i = 0; i < cubeChunks.size(); i++) {
      writeIntoRaw(bufferToWrite, *cubeChunks[i]);
    }

    minimizeCache(cubeChunks, bufferToWrite);
  }


  /**
   * Free all cube chunks (cached cube data) from memory and write them to
   *   disk. Child destructors need to call this method.
   *
   * This method should only be called otherwise when lots of cubes are in
   *   memory and the many caches cause problems with system RAM limitations.
   */
  void CubeIoHandler::clearCache() {
    // If this map is allocated, then this is a brand new cube and we need to
    //   make sure it's filled with data or NULLs.
    if(m_dataIsOnDiskMap) {
      writeNullDataToDisk();
    }

    // This should be allocated. This is a list of the cached cube data.
    //   Write it all to disk.
    if (m_rawData) {
      QMapIterator<int, RawCubeChunk *> it(*m_rawData);
      while (it.hasNext()) {
        it.next();

        if(it.value()) {
          if(it.value()->isDirty()) {
            writeRaw(*it.value());
          }

          delete it.value();
        }
      }

      m_rawData->clear();
    }

    if(m_lastProcessByLineChunks) {
      delete m_lastProcessByLineChunks;
      m_lastProcessByLineChunks = NULL;
    }
  }


  /**
   * Get the number of bytes that the cube DNs will take up. This includes
   *   padding caused by the cube chunks not aligning with the cube dimensions.
   */
  BigInt CubeIoHandler::getDataSize() const {
    return (BigInt)getChunkCountInSampleDimension() *
           (BigInt)getChunkCountInLineDimension() *
           (BigInt)getChunkCountInBandDimension() *
           (BigInt)getBytesPerChunk();
  }


  /**
   * This changes the virtual band list.
   *
   * @param virtualBandList A list where the indices are the vbands and the
   *          values are the physical bands.
   */
  void CubeIoHandler::setVirtualBands(QList<int> *virtualBandList) {
    if(m_virtualBands) {
      delete m_virtualBands;
      m_virtualBands = NULL;
    }

    if(virtualBandList && !virtualBandList->empty())
      m_virtualBands = new QList<int>(*virtualBandList);
  }


  /**
   * Children should probably implement this method. This is called to allow the
   *   IO Handling algorithm to change the labels to include things like
   *   TileSamples/TileLines for example.
   *
   * @param labels The PVL cube label to be updated to reflect information
   *                  that the child IO handlers need to properly re-read the
   *                  cube.
   */
  void CubeIoHandler::updateLabels(Pvl &labels) {
  }


  /**
   * Get the number of physical bands in the cube.
   */
  int CubeIoHandler::getBandCount() const {
    return m_numBands;
  }


  /**
   * Get the number of bands per chunk for this cube.
   */
  int CubeIoHandler::getBandCountInChunk() const {
    return m_bandsInChunk;
  }


  /**
   * Get the byte size of each chunk in the cube. Currently they must be
   *   constant size, but this is planned to be changed at some point in
   *   time.
   */
  BigInt CubeIoHandler::getBytesPerChunk() const {
    return m_samplesInChunk * m_linesInChunk * m_bandsInChunk *
        SizeOf(m_pixelType);
  }


  /**
   * Get the total number of chunks in the band (Z) dimension. This is always
   *   enough to contain every band in the cube.
   */
  int CubeIoHandler::getChunkCountInBandDimension() const {
    return (int)ceil((double)m_numBands / (double)m_bandsInChunk);
  }


  /**
   * Get the total number of chunks in the line (Y) dimension. This is always
   *   enough to contain every line in the cube.
   */
  int CubeIoHandler::getChunkCountInLineDimension() const {
    return (int)ceil((double)m_numLines / (double)m_linesInChunk);
  }


  /**
   * Get the total number of chunks in the sample (X) dimension. This is always
   *   enough to contain every sample in the cube.
   */
  int CubeIoHandler::getChunkCountInSampleDimension() const {
    return (int)ceil((double)m_numSamples / (double)m_samplesInChunk);
  }


  /**
   * Given a chunk, what's its index in the file. Chunks are ordered from
   *   left to right, then top to bottom, then front to back (BSQ). The
   *   first chunk is at the top left of band 1 and is index 0, for example. In
   *   other words, this is going from the value of m_rawData to the key.
   *
   * Chunks which sit outside of the cube entirely must not be passed into this
   *   method; the results will be wrong.
   */
  int CubeIoHandler::getChunkIndex(const RawCubeChunk &chunk)  const {
//     ASSERT(chunk.getStartSample() <= getSampleCount());
//     ASSERT(chunk.getStartLine() <= getLineCount());
//     ASSERT(chunk.getStartBand() <= getBandCount());

    int sampleIndex = (chunk.getStartSample() - 1) / getSampleCountInChunk();
    int lineIndex = (chunk.getStartLine() - 1) / getLineCountInChunk();
    int bandIndex = (chunk.getStartBand() - 1) / getBandCountInChunk();

    int indexInBand =
        sampleIndex + lineIndex * getChunkCountInSampleDimension();
    int indexOffsetToBand = bandIndex * getChunkCountInSampleDimension() *
        getChunkCountInLineDimension();

    return indexOffsetToBand + indexInBand;
  }


  /**
   * Get the byte offset to the beginning of the cube data.
   */
  BigInt CubeIoHandler::getDataStartByte() const {
    return m_startByte;
  }


  /**
   * Get the QFile containing cube data. This is what should be read from and
   *   written to.
   */
  QFile * CubeIoHandler::getDataFile() const {
    return m_dataFile;
  }


  /**
   * Get the number of lines in the cube. This does not include lines created
   *   by the chunk overflowing the line dimension.
   */
  int CubeIoHandler::getLineCount() const {
    return m_numLines;
  }


  /**
   * Get the number of lines in each chunk of the cube.
   */
  int CubeIoHandler::getLineCountInChunk() const {
    return m_linesInChunk;
  }


  /**
   * Get the physical cube DN format.
   */
  PixelType CubeIoHandler::getPixelType() const {
    return m_pixelType;
  }


  /**
   * Get the number of samples in the cube. This does not include samples
   *   created by the chunk overflowing the sample dimension.
   */
  int CubeIoHandler::getSampleCount() const {
    return m_numSamples;
  }


  /**
   * Get the number of samples in each chunk of the cube.
   */
  int CubeIoHandler::getSampleCountInChunk() const {
    return m_samplesInChunk;
  }


  /**
   * This should be called once from the child constructor. This determines the
   *   chunk sizes used for the cube and often should remain constant for a
   *   particular cube (BSQ, for example, doesn't need it to be constant but
   *   Tile does). These being large can cause excessive use of disk space that
   *   stores no cube data. These being large can also drastically increase the
   *   amount of RAM consumed.
   *
   * @param numSamples The chunk size in the sample dimension
   * @param numLines The chunk size in the line dimension
   * @param numBands The chunk size in the band dimension
   */
  void CubeIoHandler::setChunkSizes(
      int numSamples, int numLines, int numBands) {
    bool success = false;
    iString msg;

    if(m_samplesInChunk != -1 || m_linesInChunk != -1 || m_bandsInChunk != -1) {
      iString msg = "You cannot change the chunk sizes once set";
    }
    else if(numSamples < 1) {
      msg = "Negative and zero chunk sizes are not supported, samples per chunk"
          " cannot be [" + iString(numSamples) + "]";
    }
    else if(numLines < 1) {
      msg = "Negative and zero chunk sizes are not supported, lines per chunk "
            "cannot be [" + iString(numLines) + "]";
    }
    else if(numBands < 1) {
      msg = "Negative and zero chunk sizes are not supported, lines per chunk "
            "cannot be [" + iString(numBands) + "]";
    }
    else {
      success = true;
    }

    if(success) {
      m_samplesInChunk = numSamples;
      m_linesInChunk = numLines;
      m_bandsInChunk = numBands;

      if(m_dataIsOnDiskMap) {
        m_dataFile->resize(getDataStartByte() + getDataSize());
      }
      else if(m_dataFile->size() < getDataStartByte() + getDataSize()) {
        success = false;
        msg = "File size [" + iString((BigInt)m_dataFile->size()) +
            " bytes] not big enough to hold data [" +
            iString(getDataStartByte() + getDataSize()) + " bytes] where the "
            "offset to the cube data is [" + iString(getDataStartByte()) +
            " bytes]";
      }
    }
    else {
      throw iException::Message(iException::Programmer, msg, _FILEINFO_);
    }
  }


  /**
   * Get the cube chunks that correspond to the given cube area.
   *   This will create and initialize the chunks if they are not already in
   *   memory.
   *
   * @param startSample The starting sample of the cube data
   * @param numSamples The number of samples of cube data
   * @param startLine The starting line of the cube data
   * @param numLines The number of lines of cube data
   * @param startBand The starting band of the cube data
   * @param numBands The number of bands of cube data
   */
  QList<RawCubeChunk *> CubeIoHandler::findCubeChunks(int startSample,
      int numSamples, int startLine, int numLines, int startBand,
      int numBands) {
    QList<RawCubeChunk *> results;

    int initialBand = max(startBand, 1);
    int lastBand = min(startBand + numBands - 1,
                       getBandCount());

    QRect areaInBand(
        QPoint(max(startSample, 1),
                max(startLine, 1)),
        QPoint(min(startSample + numSamples - 1,
                   getSampleCount()),
                min(startLine + numLines - 1,
                    getLineCount())));

    // We are considering only 1 band at a time.. we can't use m_bandsInChunk
    //   because of virtual bands, but every extra loop should not need extra
    //   IO.
    for(int band = initialBand; band <= lastBand; band ++) {
      // This is the user-requested area in this band
      QRect areaLeftInBand(areaInBand);

      int actualBand = band;

      if(m_virtualBands) {
        actualBand = (m_virtualBands->at(band - 1) - 1) / m_bandsInChunk + 1;
      }

      // We will be consuming areaLeftInBand until we've got all of the area
      //   requested.
      while(!areaLeftInBand.isEmpty()) {
        /**
        *
        * The user has requested a slice of a cube (a rectangle):
        *    -----------------------
        *    |         Cube        |
        *    |                     |
        *    |   --------          |
        *    |   |      | <------------ Requested Area
        *    |   |      |          |
        *    |   --------          |
        *    |                     |
        *    -----------------------
        *
        * But our cube is split into "chunks" which may not line up to what
        *   was requested:
        *
        *    ------------------------
        *    |       |       |     ||
        *    |---------------------||
        *    |   --------    |     ||
        *    |---|------|----------|| (User requested area intersected with
        *    |   |   |  |    |     ||  cube chunks)
        *    |---------------------||
        *    |       |       |     ||
        *    ------------------------
        *
        *    ------------------------
        *    |   1   |   2   |  3  ||
        *    |---------------------||
        *    |   4   |   5   |  6  ||
        *    |---------------------|| (User requested area intersected with
        *    |   7   |   8   |  9  ||  cube chunks)
        *    |---------------------||
        *    |  10   |  11   | 12  ||
        *    ------------------------
        *
        * (In this case we want chunks 4, 5, 7, and 8)
        *
        * So we want to find which cube chunks line up to the requested area.
        *   To do this, we find which chunk intersects the top left of the
        *   requested cube area. That chunk is a given that we need it. Then
        *   we search in the positive X direction for more chunks that 
        *   intersect the user-requested area until we no longer do. Then we
        *   know all of the chunks that intersect a whole y-range of the
        *   requested area. We shrink the requested area (remove the found
        *   y-range) and repeat.
        */
        int areaStartLine = areaLeftInBand.top();
        int areaStartSample = areaLeftInBand.left();

        int initialChunkXPos = (areaStartSample - 1) / m_samplesInChunk;
        int initialChunkYPos = (areaStartLine - 1)   / m_linesInChunk;
        int initialChunkZPos = (actualBand - 1) / m_bandsInChunk;
        int initialChunkBand = initialChunkZPos * m_bandsInChunk + 1;

        QRect chunkRect(initialChunkXPos * m_samplesInChunk + 1,
                      initialChunkYPos * m_linesInChunk + 1,
                      m_samplesInChunk, m_linesInChunk);

        while(chunkRect.intersects(areaLeftInBand)) {
          int chunkXPos = (chunkRect.left() - 1) / m_samplesInChunk;
          int chunkYPos = (chunkRect.top() - 1) / m_linesInChunk;
          int chunkZPos = initialChunkZPos;

          // We now have an X,Y,Z position for the chunk. What's its index?
          int chunkIndex = chunkXPos +
              (chunkYPos * getChunkCountInSampleDimension()) +
              (chunkZPos * getChunkCountInSampleDimension() *
                          getChunkCountInLineDimension());

          RawCubeChunk * newChunk = getChunk(chunkIndex);

          if(!newChunk) {
            if(m_dataIsOnDiskMap && !(*m_dataIsOnDiskMap)[chunkIndex]) {
              newChunk = getNullChunk(chunkIndex);
              (*m_dataIsOnDiskMap)[chunkIndex] = true;
            }
            else {
              newChunk = new RawCubeChunk(
                chunkRect.left(), chunkRect.top(), initialChunkBand,
                chunkRect.right(), chunkRect.bottom(),
                initialChunkBand + m_bandsInChunk - 1,
                getBytesPerChunk());

              readRaw(*newChunk);
              newChunk->setDirty(false);
            }

            (*m_rawData)[chunkIndex] = newChunk;
          }

          if(newChunk)
            results.append(newChunk);

          chunkRect.moveLeft(chunkRect.right() + 1);
        }

        areaLeftInBand.setTop(chunkRect.bottom() + 1);
      }
    }

    return results;
  }


  /**
   * Find the intersection between the buffer area and the cube chunk. This
   *   accounts for virtual bands when considering the buffer and may return
   *   an area where all of the bands don't actually intersect due to virtual
   *   bands. This is implemented the way it is for performance reasons -
   *   so much as returning an Area3D or a QList<int> caused undesirable
   *   slowdowns.
   *
   * @param cube1 The cube chunk to intersect
   * @param cube2 The buffer (in virtual band space) to intersect
   * @param startX (output) The leftmost sample position (inclusive)
   * @param startY (output) The topmost line position (inclusive)
   * @param startZ (output) The frontmost band position (inclusive)
   * @param endX (output) The rightmost sample position (inclusive)
   * @param endY (output) The bottommost line position (inclusive)
   * @param endZ (output) The backmost band position (inclusive)
   */
  void CubeIoHandler::findIntersection(
      const RawCubeChunk &cube1, const Buffer &cube2,
      int &startX, int &startY, int &startZ,
      int &endX, int &endY, int &endZ) const {
    // So we have 2 3D "cubes" (not Cube cubes but 3d areas) we need to
    //   intersect in order to figure out what chunk data goes into the output
    //   buffer.

    // To find the band range we actually have to map all of the bands from
    //   virtual bands to physical bands
    int startVBand = cube2.Band();
    int endVBand = startVBand + cube2.BandDimension() - 1;

    int startPhysicalBand = 0;
    int endPhysicalBand = 0;

    for(int virtualBand = startVBand; virtualBand <= endVBand; virtualBand ++) {
      int physicalBand = virtualBand;

      if(m_virtualBands) {
        physicalBand = m_virtualBands->at(virtualBand - 1);
      }

      if(virtualBand == startVBand) {
        startPhysicalBand = physicalBand;
        endPhysicalBand = physicalBand;
      }
      else {
        if(physicalBand < startPhysicalBand)
          startPhysicalBand = physicalBand;

        if(physicalBand > endPhysicalBand)
          endPhysicalBand = physicalBand;
      }
    }

    startX = max(cube1.getStartSample(), cube2.Sample());
    startY = max(cube1.getStartLine(), cube2.Line());
    startZ = max(cube1.getStartBand(), startPhysicalBand);
    endX = min(cube1.getStartSample() + cube1.getSampleCount() - 1,
               cube2.Sample() + cube2.SampleDimension() - 1);
    endY = min(cube1.getStartLine() + cube1.getLineCount() - 1,
               cube2.Line() + cube2.LineDimension() - 1);
    endZ = min(cube1.getStartBand() + cube1.getBandCount() - 1,
               endPhysicalBand);
  }


  /**
   * If the chunk is dirty, then we write it to disk. Regardless, we then
   *   free it from memory.
   *
   * @param chunkToFree The chunk we're removing from memory
   */
  void CubeIoHandler::freeChunk(RawCubeChunk *chunkToFree) {
    if(chunkToFree && m_rawData) {
      int chunkIndex = getChunkIndex(*chunkToFree);

      m_rawData->erase(m_rawData->find(chunkIndex));

      if(chunkToFree->isDirty())
        writeRaw(*chunkToFree);

      delete chunkToFree;

      if(m_lastProcessByLineChunks) {
        delete m_lastProcessByLineChunks;
        m_lastProcessByLineChunks = NULL;
      }
    }
  }


  /**
   * Retrieve the cached chunk at the given chunk index, if there is one.
   *
   * @param chunkIndex The position of the chunk in the cube
   * @return NULL if data is not cached, otherwise the cube file data
   */
  RawCubeChunk *CubeIoHandler::getChunk(int chunkIndex) const {
    RawCubeChunk *chunk = NULL;

    if(m_rawData) {
      chunk = m_rawData->value(chunkIndex);
    }

    return chunk;
  }


  /**
   * Get the number of chunks that are required to encapsulate all of the cube
   *   data.
   */
  int CubeIoHandler::getChunkCount() const {
    return getChunkCountInSampleDimension() *
           getChunkCountInLineDimension() *
           getChunkCountInBandDimension();
  }


  /**
   * Get the X/Y/Z (Sample,Line,Band) range of the chunk at the given index.
   *
   * @param chunkIndex The chunk number in the cube file
   * @param startSample (output) The leftmost sample position (inclusive)
   * @param startLine (output) The topmost line position (inclusive)
   * @param startBand (output) The frontmost band position (inclusive)
   * @param endSample (output) The rightmost sample position (inclusive)
   * @param endLine (output) The bottommost line position (inclusive)
   * @param endBand (output) The backmost band position (inclusive)
   */
  void CubeIoHandler::getChunkPlacement(int chunkIndex,
      int &startSample, int &startLine, int &startBand,
      int &endSample, int &endLine, int &endBand) const {
    int chunkSampleIndex = chunkIndex % getChunkCountInSampleDimension();

    chunkIndex =
        (chunkIndex - chunkSampleIndex) / getChunkCountInSampleDimension();

    int chunkLineIndex = chunkIndex % getChunkCountInLineDimension();
    chunkIndex = (chunkIndex - chunkLineIndex) / getChunkCountInLineDimension();

    int chunkBandIndex = chunkIndex;

    startSample = chunkSampleIndex * getSampleCountInChunk() + 1;
    endSample = startSample + getSampleCountInChunk() - 1;
    startLine = chunkLineIndex * getLineCountInChunk() + 1;
    endLine = startLine + getLineCountInChunk() - 1;
    startBand = chunkBandIndex * getBandCountInChunk() + 1;
    endBand = startBand + getBandCountInChunk() - 1;
  }


  /**
   * This creates a chunk filled with NULLs whose placement is at chunkIndex's
   *   position. This is used for getting NULL-filled chunks the first time
   *   the chunk is requested.
   *
   * Ownership of the return value is given to the caller.
   *
   * @param chunkIndex The chunk's index which provides it's positioning.
   */
  RawCubeChunk *CubeIoHandler::getNullChunk(int chunkIndex) {
    // Shouldn't ask for null chunks when the area has already been allocated
//     ASSERT(getChunk(chunkIndex) == NULL);

    int startSample = 0;
    int startLine = 0;
    int startBand = 0;

    int endSample = 0;
    int endLine = 0;
    int endBand = 0;

    getChunkPlacement(chunkIndex, startSample, startLine, startBand,
                      endSample, endLine, endBand);

    RawCubeChunk *result = new RawCubeChunk(startSample, startLine, startBand,
        endSample, endLine, endBand, getBytesPerChunk());

    if(!m_nullChunkData) {
      // the pixel type doesn't really matter, so pick something small
      Brick nullBuffer(result->getSampleCount(),
                      result->getLineCount(),
                      result->getBandCount(),
                      UnsignedByte);

      nullBuffer.SetBasePosition(result->getStartSample(),
                            result->getStartLine(),
                            result->getStartBand());
      for(int i = 0; i < nullBuffer.size(); i++) {
        nullBuffer[i] = Null;
      }

      writeIntoRaw(nullBuffer, *result);
      m_nullChunkData = new QByteArray(result->getRawData());
    }
    else {
      result->setRawData(*m_nullChunkData);
    }

    result->setDirty(true);
    return result;
  }


  /**
   * Apply the caching algorithms and get rid of excess cube data in memory.
   *   This is intended to be called after every IO operation.
   *
   * @param justUsed The cube chunks that were used in the IO operation that
   *     is calling this method.
   * @param justRequested The buffer that was used in the IO operation that
   *     is calling this method.
   */
  void CubeIoHandler::minimizeCache(const QList<RawCubeChunk *> &justUsed,
                                    const Buffer &justRequested) {
    // Don't try to minimize the cache every time. Only try if we have 1MB+
    //   in memory.
    if(m_rawData->size() * getBytesPerChunk() > 1 * 1024 * 1024) {
      bool algorithmAccepted = false;

      int algorithmIndex = 0;
      while(!algorithmAccepted &&
            algorithmIndex < m_cachingAlgorithms->size()) {
        CubeCachingAlgorithm *algorithm = (*m_cachingAlgorithms)[algorithmIndex];

        CubeCachingAlgorithm::CacheResult result =
            algorithm->recommendChunksToFree(m_rawData->values(), justUsed,
                                             justRequested);

        algorithmAccepted = result.algorithmUnderstoodData();

        if(algorithmAccepted) {
          QList<RawCubeChunk *> chunksToFree = result.getChunksToFree();

          RawCubeChunk *chunkToFree;
          foreach(chunkToFree, chunksToFree) {
            freeChunk(chunkToFree);
          }
        }

        algorithmIndex ++;
      }

      // Fall back - no algorithms liked us :(
      if(!algorithmAccepted && m_rawData->size() > 100) {
        clearCache();
      }
    }
  }


  /**
   * Write the intersecting area of the chunk into the buffer.
   *
   * @param chunk The data source
   * @param output The data destination
   */
  void CubeIoHandler::writeIntoDouble(const RawCubeChunk &chunk,
                                      Buffer &output) const {
    // The code in this method is highly optimized. Even the order of the if
    //   statements will have a significant impact on performance if changed.
    //   Also, there is a lot of duplicate code in both writeIntoDouble(...) and
    //   writeIntoRaw(...). This is needed for performance gain. Any function
    //   or method calls from within the x loop cause significant performance
    //   decreases.
    int startX = 0;
    int startY = 0;
    int startZ = 0;

    int endX = 0;
    int endY = 0;
    int endZ = 0;

    findIntersection(chunk, output, startX, startY, startZ, endX, endY, endZ);

    int bufferBand = output.Band();
    int bufferBands = output.BandDimension();
    int chunkStartSample = chunk.getStartSample();
    int chunkStartLine = chunk.getStartLine();
    int chunkStartBand = chunk.getStartBand();
    int chunkLineSize = chunk.getSampleCount();
    int chunkBandSize = chunkLineSize * chunk.getLineCount();
    double *buffersDoubleBuf = output.DoubleBuffer();
    const char *chunkBuf = chunk.getRawData().data();
    char *buffersRawBuf = (char *)output.RawBuffer();

    for(int z = startZ; z <= endZ; z++) {
      const int &bandIntoChunk = z - chunkStartBand;
      int virtualBand = z;

      if(m_virtualBands) {
        virtualBand = m_virtualBands->indexOf(virtualBand) + 1;
      }

      if(virtualBand >= bufferBand &&
         virtualBand <= bufferBand + bufferBands - 1) {

        for(int y = startY; y <= endY; y++) {
          const int &lineIntoChunk = y - chunkStartLine;
          int bufferIndex = output.Index(startX, y, virtualBand);

          for(int x = startX; x <= endX; x++) {
            const int &sampleIntoChunk = x - chunkStartSample;

            const int &chunkIndex = sampleIntoChunk +
                (chunkLineSize * lineIntoChunk) +
                (chunkBandSize * bandIntoChunk);

            double &bufferVal = buffersDoubleBuf[bufferIndex];

            if(m_pixelType == Real) {
              float raw = ((float *)chunkBuf)[chunkIndex];
              if(m_byteSwapper)
                raw = m_byteSwapper->Float(&raw);

              if(raw >= VALID_MIN4) {
                bufferVal = (double) raw;
              }
              else {
                if(raw == NULL4)
                  bufferVal = NULL8;
                else if(raw == LOW_INSTR_SAT4)
                  bufferVal = LOW_INSTR_SAT8;
                else if(raw == LOW_REPR_SAT4)
                  bufferVal = LOW_REPR_SAT8; 
                else if(raw == HIGH_INSTR_SAT4)
                  bufferVal = HIGH_INSTR_SAT8;
                else if(raw == HIGH_REPR_SAT4)
                  bufferVal = HIGH_REPR_SAT8;
                else
                  bufferVal = LOW_REPR_SAT8;
              }

              ((float *)buffersRawBuf)[bufferIndex] = raw;
            }
            else if(m_pixelType == SignedWord) {
              short raw = ((short *)chunkBuf)[chunkIndex];
              if(m_byteSwapper)
                raw = m_byteSwapper->ShortInt(&raw);

              if(raw >= VALID_MIN2) {
                bufferVal = (double) raw * m_multiplier + m_base;
              }
              else {
                if(raw == NULL2)
                  bufferVal = NULL8;
                else if(raw == LOW_INSTR_SAT2)
                  bufferVal = LOW_INSTR_SAT8;
                else if(raw == LOW_REPR_SAT2)
                  bufferVal = LOW_REPR_SAT8;
                else if(raw == HIGH_INSTR_SAT2)
                  bufferVal = HIGH_INSTR_SAT8;
                else if(raw == HIGH_REPR_SAT2)
                  bufferVal = HIGH_REPR_SAT8;
                else
                  bufferVal = LOW_REPR_SAT8;
              }

              ((short *)buffersRawBuf)[bufferIndex] = raw;
            }
            else if(m_pixelType == UnsignedByte) {
              unsigned char raw = ((unsigned char *)chunkBuf)[chunkIndex];

              if(raw == NULL1) {
                bufferVal = NULL8;
              }
              else if(raw == HIGH_REPR_SAT1) {
                bufferVal = HIGH_REPR_SAT8;
              }
              else {
                bufferVal = (double) raw * m_multiplier + m_base;
              }

              ((unsigned char *)buffersRawBuf)[bufferIndex] = raw;
            }

            bufferIndex ++;
          }
        }
      }
    }
  }


  /**
   * Write the intersecting area of the buffer into the chunk.
   *
   * @param buffer The data source
   * @param output The data destination
   */
  void CubeIoHandler::writeIntoRaw(const Buffer &buffer, RawCubeChunk &output)
      const {
    // The code in this method is highly optimized. Even the order of the if
    //   statements will have a significant impact on performance if changed.
    //   Also, there is a lot of duplicate code in both writeIntoDouble(...) and
    //   writeIntoRaw(...). This is needed for performance gain. Any function
    //   or method calls from within the x loop cause significant performance
    //   decreases.
    int startX = 0;
    int startY = 0;
    int startZ = 0;

    int endX = 0;
    int endY = 0;
    int endZ = 0;

    output.setDirty(true);
    findIntersection(output, buffer, startX, startY, startZ, endX, endY, endZ);

    int bufferBand = buffer.Band();
    int bufferBands = buffer.BandDimension();
    int outputStartSample = output.getStartSample();
    int outputStartLine = output.getStartLine();
    int outputStartBand = output.getStartBand();
    int lineSize = output.getSampleCount();
    int bandSize = lineSize * output.getLineCount();
    double *buffersDoubleBuf = buffer.DoubleBuffer();
    char *chunkBuf = output.getRawData().data();

    for(int z = startZ; z <= endZ; z++) {
      const int &bandIntoChunk = z - outputStartBand;
      int virtualBand = z;

      if(m_virtualBands) {
        virtualBand = m_virtualBands->indexOf(virtualBand) + 1;
      }

      if(virtualBand >= bufferBand &&
         virtualBand <= bufferBand + bufferBands - 1) {

        for(int y = startY; y <= endY; y++) {
          const int &lineIntoChunk = y - outputStartLine;
          int bufferIndex = buffer.Index(startX, y, virtualBand);

          for(int x = startX; x <= endX; x++) {
            const int &sampleIntoChunk = x - outputStartSample;

            const int &chunkIndex = sampleIntoChunk +
                (lineSize * lineIntoChunk) + (bandSize * bandIntoChunk);

            double bufferVal = buffersDoubleBuf[bufferIndex];

            if(m_pixelType == Real) {
              float raw = 0;

              if(bufferVal >= VALID_MIN8) {
                double filePixelValueDbl = (bufferVal - m_base) /
                    m_multiplier;

                if(filePixelValueDbl < (double) VALID_MIN4) {
                  raw = LOW_REPR_SAT4;
                }
                else if(filePixelValueDbl > (double) VALID_MAX4) {
                  raw = HIGH_REPR_SAT4;
                }
                else {
                  raw = (float) filePixelValueDbl;
                }
              }
              else {
                if(bufferVal == NULL8)
                  raw = NULL4;
                else if(bufferVal == LOW_INSTR_SAT8)
                  raw = LOW_INSTR_SAT4;
                else if(bufferVal == LOW_REPR_SAT8)
                  raw = LOW_REPR_SAT4;
                else if(bufferVal == HIGH_INSTR_SAT8)
                  raw = HIGH_INSTR_SAT4;
                else if(bufferVal == HIGH_REPR_SAT8)
                  raw = HIGH_REPR_SAT4;
                else
                  raw = LOW_REPR_SAT4;
              }
              ((float *)chunkBuf)[chunkIndex] =
                  m_byteSwapper ? m_byteSwapper->Float(&raw) : raw;
            }
            else if(m_pixelType == SignedWord) {
              short raw;

              if(bufferVal >= VALID_MIN8) {
                double filePixelValueDbl = (bufferVal - m_base) /
                    m_multiplier;
                if(filePixelValueDbl < VALID_MIN2 - 0.5) {
                  raw = LOW_REPR_SAT2;
                }
                if(filePixelValueDbl > VALID_MAX2 + 0.5) {
                  raw = HIGH_REPR_SAT2;
                }
                else {
                  int filePixelValue = (int)round(filePixelValueDbl);

                  if(filePixelValue < VALID_MIN2) {
                    raw = LOW_REPR_SAT2;
                  }
                  else if(filePixelValue > VALID_MAX2) {
                    raw = HIGH_REPR_SAT2;
                  }
                  else {
                    raw = filePixelValue;
                  }
                }
              }
              else {
                if(bufferVal == NULL8)
                  raw = NULL2;
                else if(bufferVal == LOW_INSTR_SAT8)
                  raw = LOW_INSTR_SAT2;
                else if(bufferVal == LOW_REPR_SAT8)
                  raw = LOW_REPR_SAT2;
                else if(bufferVal == HIGH_INSTR_SAT8)
                  raw = HIGH_INSTR_SAT2;
                else if(bufferVal == HIGH_REPR_SAT8)
                  raw = HIGH_REPR_SAT2;
                else 
                  raw = LOW_REPR_SAT2;
              }

              ((short *)chunkBuf)[chunkIndex] =
                  m_byteSwapper ? m_byteSwapper->ShortInt(&raw) : raw;
            }
            else if(m_pixelType == UnsignedByte) {
              unsigned char raw;

              if(bufferVal >= VALID_MIN8) {
                double filePixelValueDbl = (bufferVal - m_base) /
                    m_multiplier;
                if(filePixelValueDbl < VALID_MIN1 - 0.5) {
                  raw = LOW_REPR_SAT1;
                }
                else if(filePixelValueDbl > VALID_MAX1 + 0.5) {
                  raw = HIGH_REPR_SAT1;
                }
                else {
                  int filePixelValue = (int)(filePixelValueDbl + 0.5);
                  if(filePixelValue < VALID_MIN1) {
                    raw = LOW_REPR_SAT1;
                  }
                  else if(filePixelValue > VALID_MAX1) {
                    raw = HIGH_REPR_SAT1;
                  }
                  else {
                    raw = (unsigned char)(filePixelValue);
                  }
                }
              }
              else {
                if(bufferVal == NULL8)
                  raw = NULL1;
                else if(bufferVal == LOW_INSTR_SAT8)
                  raw = LOW_INSTR_SAT1;
                else if(bufferVal == LOW_REPR_SAT8)
                  raw = LOW_REPR_SAT1;
                else if(bufferVal == HIGH_INSTR_SAT8)
                  raw = HIGH_INSTR_SAT1;
                else if(bufferVal == HIGH_REPR_SAT8) 
                  raw = HIGH_REPR_SAT1;
                else
                  raw = LOW_REPR_SAT1;
              }

              ((unsigned char *)chunkBuf)[chunkIndex] = raw;
            }

            bufferIndex ++;
          }
        }
      }
    }
  }


  /**
   * Write all NULL cube chunks that have not yet been accessed to disk.
   */
  void CubeIoHandler::writeNullDataToDisk() {
    if(!m_dataIsOnDiskMap) {
      iString msg = "Cannot call CubeIoHandler::writeNullDataToDisk unless "
          "data is not already on disk (Cube::Create was called)";
      throw iException::Message(iException::Programmer, msg, _FILEINFO_);
    }

    int numChunks = getChunkCount();
    for(int i = 0; i < numChunks; i++) {
      if(!(*m_dataIsOnDiskMap)[i]) {
        RawCubeChunk *nullChunk = getNullChunk(i);
        writeRaw(*nullChunk);
        (*m_dataIsOnDiskMap)[i] = true;

        delete nullChunk;
        nullChunk = NULL;
      }
    }
  }
}


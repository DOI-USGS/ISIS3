/**
 * @file
 * $Revision: 1.1.1.1 $
 * $Date: 2006/10/31 23:18:06 $
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

#ifndef CubeIoHandler_h
#define CubeIoHandler_h

#include "Constants.h"
#include "Endian.h"
#include "PixelType.h"

class QFile;
template <typename A> class QList;

namespace Isis {
  class Buffer;
  class CubeCachingAlgorithm;
  class EndianSwapper;
  class Pvl;
  class RawCubeChunk;

  /**
   * @ingroup Low Level Cube IO
   *
   * @brief Handles converting buffers to and from disk.
   *
   * This class handles converting buffers to and from disk. This class holds
   *   the cube chunks in memory and is capable of reading and writing them. It
   *   asks the caching algorithms to recommend cube chunks to not keep in
   *   memory. Children need to call setChunkSizes() in their constructor.
   *
   * This class handles all of the virtual band conversions.
   *
   * @author Jai Rideout and Steven Lambright
   *
   * @internal
   *   @history 2011-06-27 Steven Lambright - Added addCachingAlgorithm()
   *   @history 2011-07-18 Jai Rideout and Steven Lambright - Added
   *                           unimplemented copy constructor and assignment
   *                           operator.
   */
  class CubeIoHandler {
    public:
      CubeIoHandler(QFile * dataFile, QList<int> *virtualBandList,
          const Pvl &label, bool alreadyOnDisk);
      virtual ~CubeIoHandler();

      void read(Buffer &bufferToFill);
      void write(const Buffer &bufferToWrite);

      void addCachingAlgorithm(CubeCachingAlgorithm *algorithm);
      void clearCache();
      BigInt getDataSize() const;
      void setVirtualBands(QList<int> *virtualBandList);
      virtual void updateLabels(Pvl &labels);

    protected:
      int getBandCount() const;
      int getBandCountInChunk() const;
      BigInt getBytesPerChunk() const;
      int getChunkCountInBandDimension() const;
      int getChunkCountInLineDimension() const;
      int getChunkCountInSampleDimension() const;
      int getChunkIndex(const RawCubeChunk &)  const;
      BigInt getDataStartByte() const;
      QFile * getDataFile() const;
      int getLineCount() const;
      int getLineCountInChunk() const;
      PixelType getPixelType() const;
      int getSampleCount() const;
      int getSampleCountInChunk() const;

      void setChunkSizes(int numSamples, int numLines, int numBands);

      /**
       * This needs to populate the chunkToFill with unswapped raw bytes from
       *   the disk.
       *
       * @param chunkToFill The container that needs to be filled with cube
       *                    data.
       */
      virtual void readRaw(RawCubeChunk &chunkToFill) const = 0;

      /**
       * This needs to write the chunkToWrite directly to disk with no
       *   modifications to the data itself.
       *
       * @param chunkToWrite The container that needs to be put on disk.
       */
      virtual void writeRaw(const RawCubeChunk &chunkToWrite) const = 0;

    private:
      /**
       * Disallow copying of this object.
       *
       * @param other The object to copy.
       */
      CubeIoHandler(const CubeIoHandler &other);

      /**
       * Disallow assignments of this object
       *
       * @param other The CubeIoHandler on the right-hand side of the assignment
       *              that we are copying into *this.
       * @return A reference to *this.
       */
      CubeIoHandler &operator=(const CubeIoHandler &other);

      QList<RawCubeChunk *> findCubeChunks(int startSample,
          int numSamples, int startLine, int numLines, int startBand,
          int numBands);

      void findIntersection(const RawCubeChunk &cube1,
          const Buffer &cube2, int &startX, int &startY, int &startZ,
          int &endX, int &endY, int &endZ) const;

      void freeChunk(RawCubeChunk *chunkToFree);

      RawCubeChunk *getChunk(int chunkIndex) const;

      int getChunkCount() const;

      void getChunkPlacement(int chunkIndex,
        int &startSample, int &startLine, int &startBand,
        int &endSample, int &endLine, int &endBand) const;

      RawCubeChunk *getNullChunk(int chunkIndex);

      void minimizeCache(const QList<RawCubeChunk *> &justUsed,
                         const Buffer &justRequested);

      void writeIntoDouble(const RawCubeChunk &chunk, Buffer &output) const;

      void writeIntoRaw(const Buffer &buffer, RawCubeChunk &output) const;

      void writeNullDataToDisk();

    private:
      //! The file containing cube data.
      QFile * m_dataFile;

      /**
       * The start byte of the cube data. This is 0-based (i.e. a value of 0
       *   means write data into the first byte of the file). Usually the label
       *   is between 0 and m_startByte.
       */
      BigInt m_startByte;

      //! The format of each DN in the cube.
      PixelType m_pixelType;

      //! The additive offset of the data on disk.
      double m_base;

      //! The multiplicative factor of the data on disk.
      double m_multiplier;

      //! The byte order (endianness) of the data on disk.
      ByteOrder m_byteOrder;

      //! A helper that swaps byte order to and from file order.
      EndianSwapper * m_byteSwapper;

      //! The number of samples in the cube.
      int m_numSamples;

      //! The number of lines in the cube.
      int m_numLines;

      //! The number of physical bands in the cube.
      int m_numBands;

      //! The caching algorithms to use, in order of priority.
      QList<CubeCachingAlgorithm *> * m_cachingAlgorithms;

      //! The map from chunk index to chunk for cached data.
      QMap<int, RawCubeChunk *> * m_rawData;

      //! The map from chunk index to on-disk status, all true if not allocated.
      QMap<int, bool> * m_dataIsOnDiskMap;

      //! Converts from virtual band to physical band.
      QList<int> * m_virtualBands;

      //! The number of samples in a cube chunk.
      int m_samplesInChunk;

      //! The number of lines in a cube chunk.
      int m_linesInChunk;

      //! The number of physical bands in a cube chunk.
      int m_bandsInChunk;

      /**
       * This is an optimization for process by line. It relies on chunks found
       *   often being the exact same between multiple reads or multiple writes.
       */
      QList<RawCubeChunk *> *m_lastProcessByLineChunks;

      //! A raw cube chunk's data when it was all NULL's. Used for speed.
      QByteArray *m_nullChunkData;
  };
}

#endif


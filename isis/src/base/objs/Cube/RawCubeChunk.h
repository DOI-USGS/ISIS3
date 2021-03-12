#ifndef RawCubeChunk_h
#define RawCubeChunk_h
/** This is free and unencumbered software released into the public domain.
The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */

class QByteArray;
namespace Isis {
  class Area3D;

  /**
   * @ingroup LowLevelCubeIO
   * @brief A section of raw data on the disk
   *
   * This encapsulates a chunk's data. The data stored in this class is
   *   literally what is stored on disk - an unswapped byte array.
   *   These should only be used by CubeIoHandler and it's children to manage
   *   what is in memory versus what is on disk.
   *
   * @author 2011-06-15 Steven Lambright and Jai Rideout
   *
   * @internal
   */
  class RawCubeChunk {
    public:
      RawCubeChunk(const Area3D &placement, int numBytes);
      RawCubeChunk(int startSample, int startLine, int startBand,
                   int endSample, int endLine, int endBand, int numBytes);
      virtual ~RawCubeChunk();
      bool isDirty() const;

      /**
       * @returns a reference to the raw data in this cube chunk.
       */
      QByteArray &getRawData() const {
        return *m_rawBuffer;
      }

      void setRawData(QByteArray rawData);

      unsigned char getChar(int offset) const;
      short getShort(int offset) const;
      float getFloat(int offset) const;

      // The following methods are inlined for proven performance gain.

      /**
       * @returns the first (inclusive) sample of the cube chunk.
       */
      int getStartSample() const {
        return m_startSample;
      }

      /**
       * @returns the first (inclusive) line of the cube chunk.
       */
      int getStartLine() const {
        return m_startLine;
      }

      /**
       * @returns the first (inclusive) band of the cube chunk.
       */
      int getStartBand() const {
        return m_startBand;
      }

      /**
       * @returns the number of samples in the cube chunk.
       */
      int sampleCount() const {
        return m_sampleCount;
      }

      /**
       * @returns the number of lines in the cube chunk.
       */
      int lineCount() const {
        return m_lineCount;
      }

      /**
       * @returns the number of bands in the cube chunk.
       */
      int bandCount() const {
        return m_bandCount;
      }

      int getByteCount() const;

      void setData(unsigned char value, int offset);
      void setData(short value, int offset);
      void setData(const float &value, const int &offset);
      void setDirty(bool dirty);

    private:
      /**
       * The copy constructor is disabled.
       *
       * @param other the other chunk to be copied from
       */
      RawCubeChunk(const RawCubeChunk &other);

      /**
       * The assignment operator is disabled.
       *
       * @param other the other chunk to be copied from
       * @returns A reference to *this
       */
      RawCubeChunk& operator=(const RawCubeChunk &other);

    private:
      //! True if the data does not match what is on disk.
      bool m_dirty;

      //! This is the raw data to be put on disk.
      QByteArray *m_rawBuffer;
      //! This is the internal pointer to the raw buffer for performance.
      char *m_rawBufferInternalPtr;

      //! The number of samples in the cube chunk.
      int m_sampleCount;
      //! The number of lines in the cube chunk.
      int m_lineCount;
      //! The number of bands in the cube chunk.
      int m_bandCount;

      //! The one-based (inclusive) start sample of the cube chunk.
      int m_startSample;
      //! The one-based (inclusive) start line of the cube chunk.
      int m_startLine;
      //! The one-based (inclusive) start band of the cube chunk.
      int m_startBand;
  };
}

#endif


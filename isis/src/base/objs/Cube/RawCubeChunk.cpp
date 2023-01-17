/** This is free and unencumbered software released into the public domain.
The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */
#include "RawCubeChunk.h"

#include <cmath>
#include <iostream>

#include <QByteArray>

#include "Area3D.h"
#include "Displacement.h"
#include "Distance.h"
#include "IException.h"
#include "IString.h"

namespace Isis {
  /**
   * This constructor creates a new cube chunk based on the provided placement
   *   and data size.
   *
   * @param placement the 3D "cube" that this chunk will cover (inclusive)
   * @param numBytes the number of raw data bytes in the chunk
   */
  RawCubeChunk::RawCubeChunk(const Area3D &placement, int numBytes) {
    m_dirty = false;

    m_rawBuffer = new QByteArray(numBytes, '\0');
    m_rawBufferInternalPtr = m_rawBuffer->data();

    m_sampleCount = (int)round(placement.getWidth().pixels()) + 1;
    m_lineCount = (int)round(placement.getHeight().pixels()) + 1;
    m_bandCount = (int)round(placement.getDepth().pixels()) + 1;

    m_startSample = (int)round(placement.getStartX().pixels());
    m_startLine = (int)round(placement.getStartY().pixels());
    m_startBand = (int)round(placement.getStartZ().pixels());
  }

  /**
   * This constructor creates a new cube chunk based on the provided
   *   coordinates and data size. This constructor is provided in addition to
   *   the previous one for performance gain.
   *
   * @param startSample the starting sample of the chunk (inclusive)
   * @param startLine the starting line of the chunk (inclusive)
   * @param startBand the starting band of the chunk (inclusive)
   * @param endSample the ending sample of the chunk (inclusive)
   * @param endLine the ending line of the chunk (inclusive)
   * @param endBand the ending band of the chunk (inclusive)
   * @param numBytes the number of raw data bytes in the chunk
   */
  RawCubeChunk::RawCubeChunk(int startSample, int startLine, int startBand,
                             int endSample, int endLine, int endBand,
                             int numBytes) {
    m_dirty = false;

    m_rawBuffer = new QByteArray(numBytes, '\0');
    m_rawBufferInternalPtr = m_rawBuffer->data();

    m_sampleCount = endSample - startSample + 1;
    m_lineCount = endLine - startLine + 1;
    m_bandCount = endBand - startBand + 1;

    m_startSample = startSample;
    m_startLine = startLine;
    m_startBand = startBand;
  }


  /**
   * The destructor.
   */
  RawCubeChunk::~RawCubeChunk() {
    if(m_rawBuffer) {
      delete m_rawBuffer;
      m_rawBuffer = NULL;
      m_rawBufferInternalPtr = NULL;
    }
  }


  /**
   * @returns true if the data in this chunk does not match what is on disk.
   */
  bool RawCubeChunk::isDirty() const {
    return m_dirty;
  }


  /**
   * Sets the chunk's raw data. This size of the new raw data must match that
   *   of the chunk's current raw data buffer.
   *
   * @param rawData the raw data
   */
  void RawCubeChunk::setRawData(QByteArray rawData) {
    if(rawData.size() != m_rawBuffer->size()) {
      IString msg = "Cannot set raw data on a RawCubeChunk with a differently "
          "sized data array";
      throw IException(IException::Programmer, msg, _FILEINFO_);
    }

    m_dirty = true;
    *m_rawBuffer = rawData;
    m_rawBufferInternalPtr = m_rawBuffer->data();
  }


  /**
   * This method is currently not in use due to a faster way of getting data
   *   from the buffer (through the internal pointer).
   *
   * @param offset the offset into the raw data buffer
   * @returns the char at the position indicated by the given offset.
   */
  unsigned char RawCubeChunk::getChar(int offset) const {
    return ((unsigned char *)m_rawBuffer->data())[offset];
  }

  /**
   * @returns the short at the position indicated by the given offset. The value
   *   returned is not byte swapped. This method is currently not in use due to
   *   a faster way of getting data from the buffer (through the internal
   *   pointer).
   *
   * @param offset the offset into the raw data buffer
   */
  short RawCubeChunk::getShort(int offset) const {
    return ((short *)m_rawBuffer->data())[offset];
  }

  /**
   * @returns the float at the position indicated by the given offset. The value
   *   returned is not byte swapped. This method is currently not in use due to
   *   a faster way of getting data from the buffer (through the internal
   *   pointer).
   *
   * @param offset the offset into the raw data buffer
   */
  float RawCubeChunk::getFloat(int offset) const {
    return ((float *)m_rawBuffer->data())[offset];
  }


  /**
   * @returns the size (in bytes) of the raw data buffer. If the raw data buffer
   * is null, -1 is returned.
   */
  int RawCubeChunk::getByteCount() const {
    if(m_rawBuffer)
      return m_rawBuffer->size();
    else
      return -1;
  }


  /**
   * Sets the char at the given offset in the raw data buffer of this chunk.
   *   The chunk's dirty flag is set to true.
   *
   * @param value the new char value
   * @param offset the position to place the new value at
   */
  void RawCubeChunk::setData(unsigned char value, int offset) {

    m_dirty = true;
    m_rawBufferInternalPtr[offset] = value;
  }


  /**
   * Sets the short at the given offset in the raw data buffer of this chunk.
   *   The chunk's dirty flag is set to true.
   *
   * @param value the new short value
   * @param offset the position to place the new value at
   */
  void RawCubeChunk::setData(short value, int offset) {

    m_dirty = true;
    ((short *)m_rawBufferInternalPtr)[offset] = value;
  }


  /**
   * Sets the float at the given offset in the raw data buffer of this chunk.
   *   The chunk's dirty flag is set to true.
   *
   * @param value the new float value
   * @param offset the position to place the new value at
   */
  void RawCubeChunk::setData(const float &value, const int &offset) {

    m_dirty = true;
    ((float *)m_rawBufferInternalPtr)[offset] = value;
  }


  /**
   * Sets the chunk's dirty flag, indicating whether or not the chunk's data
   * matches the data that is on disk.
   *
   * @param dirty bool indicating whether the chunk is dirty or not
   */
  void RawCubeChunk::setDirty(bool dirty) {
    m_dirty = dirty;
  }
}


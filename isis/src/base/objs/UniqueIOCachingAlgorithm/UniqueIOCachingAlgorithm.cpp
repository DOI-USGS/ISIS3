/** This is free and unencumbered software released into the public domain.
The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */

#include "UniqueIOCachingAlgorithm.h"

#include <algorithm>
#include <iostream>

#include <QList>
#include <QQueue>

#include "IException.h"
#include "IString.h"
#include "RawCubeChunk.h"

namespace Isis {
  /**
   * Construct a new UniqueIOCachingAlgorithm. The last numUniqueIOs will be
   *   kept in the cache, while the other chunks in the cache will all be
   *   tossed.
   *
   * @param numUniqueIOs The number of unique IO operations to keep the chunks
   *                     around for.
   */
  UniqueIOCachingAlgorithm::UniqueIOCachingAlgorithm(int numUniqueIOs) {
    m_uniqueIOs = NULL;
    m_uniqueIOs = new QQueue < QList <RawCubeChunk *> >;

    if (numUniqueIOs <= 0) {
      IString msg = "At least one unique IO must be used when using the unique "
          "IO cube caching algorithm";
      throw IException(IException::Programmer, msg, _FILEINFO_);
    }

    while (m_uniqueIOs->size() < numUniqueIOs)
      m_uniqueIOs->enqueue( QList<RawCubeChunk *>() );
  }


  /**
   * Frees the memory allocated by this caching algorithm.
   */
  UniqueIOCachingAlgorithm::~UniqueIOCachingAlgorithm() {
    if (m_uniqueIOs) {
      delete m_uniqueIOs;
      m_uniqueIOs = NULL;
    }
  }


  /**
   * Please see the class description for how this algorithm works.
   *
   * @param allocated All of the allocated cube chunks
   * @param justUsed The cube chunks used in the last I/O
   * @param justRequested The buffer passed into the last I/O
   *
   * @returns The chunks that should be removed from memory
   */
  CubeCachingAlgorithm::CacheResult
      UniqueIOCachingAlgorithm::recommendChunksToFree(
      QList <RawCubeChunk *> allocated, QList <RawCubeChunk *> justUsed,
          const Buffer &justRequested) {
    QList <RawCubeChunk *> chunksToToss;
    CacheResult result(chunksToToss);

    // This read has different chunks than before...
    if (justUsed.size()) {
      // If any of our unique reads are not yet populated, then populate them
      //   with our current IO.
      bool foundAHome = false;

      for (int uniqueIONum = 0;
           !foundAHome && uniqueIONum < m_uniqueIOs->size();
           uniqueIONum++) {
        QList <RawCubeChunk *> &uniqueIO = (*m_uniqueIOs)[uniqueIONum];

        if (uniqueIO.empty()) {
          foundAHome = true;
          uniqueIO = justUsed;
        }
        else if (uniqueIO == justUsed) {
          foundAHome = true;
          m_uniqueIOs->enqueue(m_uniqueIOs->takeAt(uniqueIONum));
        }
      }

      if (!foundAHome) {
        m_uniqueIOs->enqueue(justUsed);
        m_uniqueIOs->dequeue();
      }

      // We don't know if Cube tossed any of the chunks, so we really need to
      //   look in the allocated list for things to toss.
      QListIterator <RawCubeChunk *> allocatedIterator(allocated);

      while (allocatedIterator.hasNext()) {
        RawCubeChunk *chunk = allocatedIterator.next();

        bool found = false;

        foreach (QList <RawCubeChunk *> chunksForIo, *m_uniqueIOs) {
          if (!found)
            found = chunksForIo.indexOf(chunk) != -1;
        }

        if (!found) {
          chunksToToss.append(chunk);
        }
      }

      result = CacheResult(chunksToToss);
    }

    return result;
  }
}


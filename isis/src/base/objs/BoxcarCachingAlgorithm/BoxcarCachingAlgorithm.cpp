/** This is free and unencumbered software released into the public domain.
The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */

#include "BoxcarCachingAlgorithm.h"

#include <algorithm>
#include <iostream>

#include <QList>
#include <QQueue>

#include "IException.h"
#include "IString.h"
#include "RawCubeChunk.h"

namespace Isis {
  /**
   * Construct a new BoxcarCachingAlgorithm. The last numUniqueIOs will be
   *   kept in the cache, while the other chunks in the cache will all be
   *   tossed.
   *
   */
  BoxcarCachingAlgorithm::BoxcarCachingAlgorithm() {
    m_minLine = 1;
  }


  /**
   * Frees the memory allocated by this caching algorithm.
   */
  BoxcarCachingAlgorithm::~BoxcarCachingAlgorithm() {
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
      BoxcarCachingAlgorithm::recommendChunksToFree(
      QList <RawCubeChunk *> allocated, QList <RawCubeChunk *> justUsed,
          const Buffer &justRequested) {
    (void) justRequested; // unused, for fullfilling virtual function signiture 
    QList<RawCubeChunk *> chunksToToss;

    if (justUsed.size() > 0) {
      // TODO bands
      int minLine = justUsed[0]->getStartLine();

      QListIterator<RawCubeChunk *> justUsedIterator(justUsed);
      while (justUsedIterator.hasNext()) {
        RawCubeChunk *chunk = justUsedIterator.next();
        int currentStart = chunk->getStartLine();
        if (currentStart < minLine) minLine = currentStart;
      }

      if (minLine > m_minLine) {
        m_minLine = minLine;
        QListIterator<RawCubeChunk *> allocatedIterator(allocated);
        while (allocatedIterator.hasNext()) {
          RawCubeChunk *chunk = allocatedIterator.next();
          int currentEnd = chunk->getStartLine() + chunk->lineCount() - 1;

          if (currentEnd < minLine) {
            chunksToToss.append(chunk);
          }
        }
      }
    }

    return CacheResult(chunksToToss);
  }
}

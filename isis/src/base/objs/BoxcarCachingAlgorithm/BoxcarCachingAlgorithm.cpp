/**
 * @file
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

#include "BoxcarCachingAlgorithm.h"

#include <algorithm>
#include <iostream>

#include <QList>
#include <QQueue>

#include "IException.h"
#include "iString.h"
#include "RawCubeChunk.h"

namespace Isis {
  /**
   * Construct a new BoxcarCachingAlgorithm. The last numUniqueIOs will be
   *   kept in the cache, while the other chunks in the cache will all be
   *   tossed.
   *
   * @param numUniqueIOs The number of unique IO operations to keep the chunks
   *                     around for.
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

    QList<RawCubeChunk *> chunksToToss;

    //std::cerr << "begin" << std::endl;

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
          int currentEnd = chunk->getStartLine() + chunk->getLineCount() - 1;

          if (currentEnd < minLine) {
            //std::cerr << "minLine = " << minLine << " | maxLine = " << maxLine << std::endl;
            //std::cerr << "currentStart = " << currentStart << " | currentEnd = " << currentEnd << std::endl;
            chunksToToss.append(chunk);
          }
        }
      }

      //std::cerr << "allocated chunks = " << allocated.size() << std::endl;
      //std::cerr << "num chunks = " << chunksToToss.size() << std::endl;
    }

    return CacheResult(chunksToToss);
  }
}


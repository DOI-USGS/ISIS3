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

#include "FilterCachingAlgorithm.h"

#include <algorithm>
#include <iostream>

#include <QList>

#include "Buffer.h"
#include "Distance.h"
#include "RawCubeChunk.h"
#include "Statistics.h"

using std::max;

namespace Isis {
  /**
   * Construct a new FilterCachingAlgorithm. The last numParallelIOs will be
   *   kept in the cache, while the other chunks in the cache will all be
   *   tossed.
   *
   * @param numParallelIOs The number of IO operations to keep the chunks around
   *                       for.
   */
  FilterCachingAlgorithm::FilterCachingAlgorithm(int numParallelIOs) {
    m_chunksToKeep = NULL;
    m_chunksToKeep = new QList< QList< RawCubeChunk * > >;

    while(m_chunksToKeep->size() < numParallelIOs)
      m_chunksToKeep->append( QList<RawCubeChunk *>() );

    m_currentIo = 0;
  }


  /**
   * Frees the memory allocated by this caching algorithm.
   */
  FilterCachingAlgorithm::~FilterCachingAlgorithm() {
    if(m_chunksToKeep) {
      delete m_chunksToKeep;
      m_chunksToKeep = NULL;
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
      FilterCachingAlgorithm::recommendChunksToFree(
      QList<RawCubeChunk *> allocated, QList<RawCubeChunk *> justUsed,
          const Buffer &justRequested) {
    QList<RawCubeChunk *> chunksToToss;
    CacheResult result(chunksToToss);

    // This read has different chunks than before...
    if(!justUsed.size() ||
       !(*m_chunksToKeep)[m_currentIo].size() ||
         ((*m_chunksToKeep)[m_currentIo][0] != justUsed[0] &&
          (*m_chunksToKeep)[m_currentIo] != justUsed)) {
      (*m_chunksToKeep)[m_currentIo] = justUsed;

      // We don't know if Cube tossed any of the chunks, so we really need to
      //   look in the allocated list for things to toss. Let's work this by
      //   getting a list of things to keep and then freeing everything
      //   that is not in that list.
      QListIterator<RawCubeChunk *> allocatedIterator(allocated);

      while(allocatedIterator.hasNext()) {
        RawCubeChunk *chunk = allocatedIterator.next();

        bool found = false;

        foreach(QList<RawCubeChunk *> chunksForIo, *m_chunksToKeep) {
          if(!found)
            found = chunksForIo.indexOf(chunk) != -1;
        }

        if(!found) {
          chunksToToss.append(chunk);
        }
      }

      result = CacheResult(chunksToToss);
    }

    m_currentIo ++;

    if(m_currentIo >= m_chunksToKeep->size())
      m_currentIo = 0;

    return result;
  }
}


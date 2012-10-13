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


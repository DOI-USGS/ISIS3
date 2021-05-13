/** This is free and unencumbered software released into the public domain.
The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */

#ifndef RegionalCachingAlgorithm_h
#define RegionalCachingAlgorithm_h

#include "CubeCachingAlgorithm.h"

namespace Isis {
  /**
   * This algorithm is designed for applications that use ProcessByQuickFilter
   *   or very similar I/O patterns to cache cube data appropriately. The last
   *   numParallelIOs worth of I/Os will be left in the cache.
   *
   * @author ????-??-?? Jai Rideout and Steven Lambright
   *
   * @internal
   */
  class FilterCachingAlgorithm : public CubeCachingAlgorithm {
    public:
      FilterCachingAlgorithm(int numParallelIOs);
      virtual ~FilterCachingAlgorithm();

      virtual CacheResult recommendChunksToFree(
          QList<RawCubeChunk *> allocated, QList<RawCubeChunk *> justUsed,
          const Buffer &justRequested);

    private:
      /**
       * This is stored from parallel read # -> list of chunks for that read.
       *   All chunks not in this list are freed from memory.
       */
      QList< QList< RawCubeChunk * > > * m_chunksToKeep;

      /**
       * This keeps track of our position inside of m_chunksToKeep
       */
      int m_currentIo;
  };
}

#endif

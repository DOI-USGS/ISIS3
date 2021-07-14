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
   * @ingroup Low Level Cube IO
   *
   * This algorithm recommends chunks to be freed that are not within the last
   *   IO. Once the 10MB limit is reached, it recommends more chunks to free in
   *   order to stay below this limit, as long as the chunks weren't in the
   *   last IO.
   *
   * @author ????-??-?? Jai Rideout and Steven Lambright
   *
   * @internal
   */
  class RegionalCachingAlgorithm : public CubeCachingAlgorithm {
    public:
      /**
       * @see CubeCachingAlgorithm::recommendChunksToFree()
       * @param allocated
       * @param justUsed
       * @param justRequested
       * @return
       */
      virtual CacheResult recommendChunksToFree(
          QList<RawCubeChunk *> allocated, QList<RawCubeChunk *> justUsed,
          const Buffer &justRequested);
  };
}

#endif

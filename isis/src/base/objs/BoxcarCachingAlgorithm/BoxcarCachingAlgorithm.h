/** This is free and unencumbered software released into the public domain.
The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */

#ifndef BoxcarCachingAlgorithm_h
#define BoxcarCachingAlgorithm_h

#include "CubeCachingAlgorithm.h"

template <typename A> class QList;
template <typename A> class QQueue;

namespace Isis {
  /**
   * This algorithm is designed for applications that jump around between a
   *   couple of spots in the cube with a difficult to predict pattern but
   *   always the same places in the cube.
   *
   * This was designed for ProcessMosaic which jumps between band 1 and band n
   *   in the possible patterns (where A is a line on band 1 and B is a line on
   *    band N):
   *   A,A or A,B,A or A,B,B,A
   *
   * @author ????-??-?? Jai Rideout and Steven Lambright
   *
   * @internal
   *   @history 2017-08-30 Summer Stapleton - Updated documentation. References #4807.
   */
  class BoxcarCachingAlgorithm : public CubeCachingAlgorithm {
    public:
      BoxcarCachingAlgorithm();
      virtual ~BoxcarCachingAlgorithm();

      virtual CacheResult recommendChunksToFree(
          QList <RawCubeChunk *> allocated, QList <RawCubeChunk *> justUsed,
          const Buffer &justRequested);

    private:
      int m_minLine;  //!< Used to calculate what lines to cache
  };
}

#endif

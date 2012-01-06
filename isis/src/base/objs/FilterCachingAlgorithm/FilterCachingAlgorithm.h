/**
 * @file
 * $Revision: 1.1.1.1 $
 * $Date: 2006/10/31 23:18:06 $
 *
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

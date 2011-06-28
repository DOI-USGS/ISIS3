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

#ifndef UniqueIOCachingAlgorithm_h
#define UniqueIOCachingAlgorithm_h

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
   * @author Jai Rideout and Steven Lambright
   *
   * @internal
   */
  class UniqueIOCachingAlgorithm : public CubeCachingAlgorithm {
    public:
      UniqueIOCachingAlgorithm(int numUniqueIOs);
      virtual ~UniqueIOCachingAlgorithm();

      virtual CacheResult recommendChunksToFree(
          QList <RawCubeChunk *> allocated, QList <RawCubeChunk *> justUsed,
          const Buffer &justRequested);

    private:
      /**
       * This is the set of past unique IOs. All chunks not in this set of
       *   lists are freed from memory.
       */
      QQueue < QList <RawCubeChunk *> > * m_uniqueIOs;
  };
}

#endif

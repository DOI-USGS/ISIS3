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

#ifndef CubeCachingAlgorithm_h
#define CubeCachingAlgorithm_h

template <typename T> class QList;

namespace Isis {
  class Buffer;
  class RawCubeChunk;

  /**
   * @ingroup Low Level Cube IO
   * @brief This is the parent of the caching algorithms
   *
   * The caching algorithms are given some limited/easy to acquire data about
   *   recent IOs and the allocated cube chunks. Their job is to quickly
   *   determine which allocated cube chunks should be put on disk or just
   *   freed from memory. These will not be called until there is at least
   *   a few allocated chunks in memory.
   *
   * @author ????-??-?? Jai Rideout and Steven Lambright
   *
   * @internal
   */
  class CubeCachingAlgorithm {
    public:
      CubeCachingAlgorithm();
      virtual ~CubeCachingAlgorithm();

      /**
       * @brief This stores the results of the caching algorithm.
       *
       * @author ????-??-?? Jai Rideout and Steven Lambright
       *
       * @internal
       *   @history 2011-08-26 Steven Lambright and Jai Rideout - Fixed memory
       *                           leak.
       */
      class CacheResult {
        public:
          CacheResult();
          CacheResult(QList<RawCubeChunk *>);
          CacheResult(const CacheResult &other);
          virtual ~CacheResult();

          bool algorithmUnderstoodData() const;
          QList<RawCubeChunk *> getChunksToFree() const;

          CacheResult &operator=(const CacheResult &other);

        private:
          /**
           * If NULL, the algorithm did not succeed. If allocated, then this
           *   is a valid list of which chunks should be freed from memory.
           */
          QList<RawCubeChunk *> *m_chunksToFree;
      };

      /**
       * Call this to determine which chunks should be freed from memory.
       *
       * @param allocated This is an unordered list of all of the allocated
       *   chunks.
       * @param justUsed This should be the chunks required in the current read
       *   or write. Many algorithms will use this to not clean it up.
       * @param justRequested This must be the buffer area requested
       * @returns A list of chunks to be freed from memory
       *
       */
      virtual CacheResult recommendChunksToFree(
          QList<RawCubeChunk *> allocated, QList<RawCubeChunk *> justUsed,
          const Buffer &justRequested) = 0;
  };
}

#endif

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

#include "CubeCachingAlgorithm.h"

#include <cstring>

#include <QList>

#include "RawCubeChunk.h"

namespace Isis {
  /**
   * Construct a caching algorithm.
   */
  CubeCachingAlgorithm::CubeCachingAlgorithm() {
  }


  /**
   * Cleans up after a caching algorithm.
   */
  CubeCachingAlgorithm::~CubeCachingAlgorithm() {
  }


  /**
   * Construct a cache algorithm result with the idea that the algorithm did
   *   not understand/was unable to determine a good result for what to free.
   */
  CubeCachingAlgorithm::CacheResult::CacheResult() {
    m_chunksToFree = NULL;
  }


  /**
   * Construct a cache algorithm result with the idea that the algorithm did
   *   understand/was able to determine a good result for what to free. The
   *   list may be empty. Typically, if your result uses this constructor,
   *   other algorithms will not be subsequently called.
   */
  CubeCachingAlgorithm::CacheResult::CacheResult(QList<RawCubeChunk *> free) {
    m_chunksToFree = NULL;
    m_chunksToFree = new QList<RawCubeChunk *>(free);
  }


  /**
   * Copy a CacheResult.
   *
   * @param other The result we're copying into ourselves
   */
  CubeCachingAlgorithm::CacheResult::CacheResult(const CacheResult &other) {
    m_chunksToFree = NULL;

    if(other.m_chunksToFree)
      m_chunksToFree = new QList<RawCubeChunk *>(*other.m_chunksToFree);
  }


  /**
   * Free allocated memory.
   */
  CubeCachingAlgorithm::CacheResult::~CacheResult() {
    if(m_chunksToFree) {
      delete m_chunksToFree;
      m_chunksToFree = NULL;
    }
  }


  /**
   * If this is true, then the results (be them empty or not) should be
   *   considered valid. If this is false, then the results are empty
   *   and the caching algorithm failed.
   * 
   * @returns True if the recommendation is valid
   */
  bool CubeCachingAlgorithm::CacheResult::algorithmUnderstoodData() const {
    return (m_chunksToFree != NULL);
  }


  /**
   * @return list of RawCubeChunks to remove from RAM according to this
   *   caching algorithm. Valid if algorithmUnderstoodData() is true.
   */
  QList<RawCubeChunk *> CubeCachingAlgorithm::CacheResult::getChunksToFree()
      const {
    QList<RawCubeChunk *> recommended;

    if(m_chunksToFree != NULL)
      recommended = *m_chunksToFree;

    return recommended;
  }


  /**
   * Assign one cache result to another.
   *
   * @param other The RHS of the assignment operator; the cache result we're
   *     copying from.
   * @returns A reference to *this
   */
  CubeCachingAlgorithm::CacheResult &
      CubeCachingAlgorithm::CacheResult::operator=(const CacheResult &other) {
    delete m_chunksToFree;
    m_chunksToFree = NULL;

    if(other.m_chunksToFree) {
      m_chunksToFree = new QList<RawCubeChunk *>;
      *m_chunksToFree = *other.m_chunksToFree;
    }

    return *this;
  }
}


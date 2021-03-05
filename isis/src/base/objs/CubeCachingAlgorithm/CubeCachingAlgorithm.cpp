/** This is free and unencumbered software released into the public domain.
The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */

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


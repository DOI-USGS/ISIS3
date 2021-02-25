/** This is free and unencumbered software released into the public domain.
The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */

#include "RegionalCachingAlgorithm.h"

#include <algorithm>
#include <iostream>

#include <QList>

#include "Buffer.h"
#include "Distance.h"
#include "RawCubeChunk.h"
#include "Statistics.h"

using std::max;

namespace Isis {
  CubeCachingAlgorithm::CacheResult
      RegionalCachingAlgorithm::recommendChunksToFree(
      QList<RawCubeChunk *> allocated, QList<RawCubeChunk *> justUsed,
          const Buffer &justRequested) {
    CacheResult result;
    if(allocated.size() && allocated[0] != NULL) {
      double avgLargestDim = max( max(justRequested.SampleDimension(),
                                      justRequested.LineDimension()),
                                  justRequested.BandDimension());

      // They'll all be roughly the same size, so the first one is good enough
      int largestChunkDim = max( max( allocated[0]->sampleCount(),
                                      allocated[0]->lineCount()),
                                 allocated[0]->bandCount());
      // The average needed per request ought to be
      //   avgLargestDim / largestChunkDim. Let's keep an extra few around
      //   since it's cheap, and because we are uncertain of request patterns.
      //   40X with a maximum should keep a reasonable number of results
      //   around.
      int numToKeep = (int)ceil(avgLargestDim / largestChunkDim) * 1;

      // Limit to ~10MB
      int approxBytesPerChunk = allocated[0]->getByteCount();

      int tenMB = 10 * 1024 * 1024; // 10MB in bytes
      if(numToKeep * approxBytesPerChunk > tenMB) {
        numToKeep = tenMB / approxBytesPerChunk;
      }

      if(numToKeep < justUsed.size())
        numToKeep = justUsed.size();

      int numToToss = allocated.size() - numToKeep;

      QList<RawCubeChunk *> chunksToToss;

      QListIterator<RawCubeChunk *> allocatedIterator(allocated);

      while(numToToss > 0 && allocatedIterator.hasNext()) {
        RawCubeChunk *chunk = allocatedIterator.next();

        if(justUsed.indexOf(chunk) == -1) {
          numToToss --;
          chunksToToss.append(chunk);
        }
      }

      result = CacheResult(chunksToToss);
    }

    return result;
  }
}


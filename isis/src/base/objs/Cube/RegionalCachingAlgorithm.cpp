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
      int largestChunkDim = max( max( allocated[0]->getSampleCount(),
                                      allocated[0]->getLineCount()),
                                 allocated[0]->getBandCount());
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


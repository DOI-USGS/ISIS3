/** This is free and unencumbered software released into the public domain.
The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */

#include <iostream>

#include <QList>

#include "Buffer.h"
#include "FilterCachingAlgorithm.h"
#include "RawCubeChunk.h"

using namespace Isis;
using namespace std;

int main() {
  const int parallelIos = 5;
  const int ioSeparation = 10000;

  FilterCachingAlgorithm alg(parallelIos);

  QList<RawCubeChunk *> allocatedChunks;
  // algorithm doesn't use the buffer so we don't need to initialize this
  //   properly
  Buffer ioBufferTmp;

  for(int readNum = 0; readNum < 20; readNum ++) {
    QList<RawCubeChunk *> ioUsedChunks;
    for(int ioNum = 0; ioNum < parallelIos; ioNum ++) {
      int cubeLine = readNum + ioNum * ioSeparation + 1;
      RawCubeChunk *ioChunk = new RawCubeChunk(1, cubeLine, 1,
                                               2, cubeLine, 1, 0);
      allocatedChunks.append(ioChunk);
      ioUsedChunks.append(ioChunk);
    }

    CubeCachingAlgorithm::CacheResult result = alg.recommendChunksToFree(
        allocatedChunks, ioUsedChunks, ioBufferTmp);

    std::cerr << "Cache result:\n";
    std::cerr << "    Understood data? " <<
        result.algorithmUnderstoodData() << "\n";
    if(result.algorithmUnderstoodData()) {
      QList<RawCubeChunk *> toFree = result.getChunksToFree();
      std::cerr << "    Number of chunks to free = " << toFree.size();

      if(toFree.size()) {
        std::cerr << " @ lines = ";

        RawCubeChunk *chunkForPrintingLine;
        bool started = false;
        foreach(chunkForPrintingLine, toFree) {
          if(started)
            std::cerr << ", ";
          else
            started = true;

          std::cerr << chunkForPrintingLine->getStartLine();
        }
      }

      std::cerr << "\n";

      RawCubeChunk *tmp;
      foreach(tmp, toFree) {
        allocatedChunks.removeAll(tmp);
      }

      std::cerr << "    Number of chunks left = "
                << allocatedChunks.size() << "\n";
    }
  }

  std::cerr << "\n";

  return 0;
}

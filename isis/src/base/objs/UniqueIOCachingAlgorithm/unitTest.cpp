/** This is free and unencumbered software released into the public domain.
The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */

#include <iostream>

#include <QList>

#include "Buffer.h"
#include "UniqueIOCachingAlgorithm.h"
#include "RawCubeChunk.h"

using namespace Isis;
using namespace std;

int main() {
  const int uniqueIos = 2;
  const int ioSeparation = 10;

  UniqueIOCachingAlgorithm alg(uniqueIos);

  QList <RawCubeChunk *> allocatedChunks;
  // algorithm doesn't use the buffer so we don't need to initialize this
  //   properly
  Buffer ioBufferTmp;

  QList <RawCubeChunk *> ioUsedChunks;

  for (int ioNum = 0; ioNum < uniqueIos * 2; ioNum ++) {
    int cubeLine = (ioNum + 1) * ioSeparation + 1;
    RawCubeChunk *ioChunk = new RawCubeChunk(1, cubeLine, 1,
                                             2, cubeLine, 1, 0);
    allocatedChunks.append(ioChunk);
  }


  QList <int> indicesToTry;

  for (int i = 0; i < 4; i++) {
    indicesToTry << 0;
  }

  for (int i = 0; i < 4; i++) {
    indicesToTry << i % uniqueIos;
  }

  for (int i = 0; i < 4; i++) {
    indicesToTry << i % (uniqueIos * 2);
  }

  for (int i = 0; i < 4; i++) {
    indicesToTry << (4 - i - 1) % (uniqueIos * 2);
  }

  QList <RawCubeChunk *> allocatedUsedSoFar;

  for (int i = 0; i < indicesToTry.size(); i++) {
    QList <RawCubeChunk *> used;
    used.append(allocatedChunks[indicesToTry[i]]);

    if (allocatedUsedSoFar.indexOf(used[0]) == -1)
      allocatedUsedSoFar.append(used[0]);

    CubeCachingAlgorithm::CacheResult result = alg.recommendChunksToFree(
        allocatedUsedSoFar, used, ioBufferTmp);

    std::cerr << "Cache result (input was "
              << (used[0]->getStartLine()) << "):\n";
    std::cerr << "    Understood data? " <<
        result.algorithmUnderstoodData() << "\n";
    if (result.algorithmUnderstoodData()) {
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
          allocatedUsedSoFar.removeAll(chunkForPrintingLine);
        }
      }

      std::cerr << "\n";
    }
  }

  std::cerr << "\n";

  return 0;
}

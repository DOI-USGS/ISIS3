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

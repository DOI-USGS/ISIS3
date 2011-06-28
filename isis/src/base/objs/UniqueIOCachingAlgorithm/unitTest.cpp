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

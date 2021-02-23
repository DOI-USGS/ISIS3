/** This is free and unencumbered software released into the public domain.
The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */

#include <iostream>

#include <QList>

#include "CubeCachingAlgorithm.h"
#include "RawCubeChunk.h"

using namespace Isis;
using namespace std;

int main() {
  std::cerr << "This class is pure virtual and does literally nothing.\n";
  std::cerr << "So let's test a cache result!\n";

  QList <RawCubeChunk *> tmp;
  tmp.append(NULL);
  CubeCachingAlgorithm::CacheResult res;

  std::cerr << "Test 1 - understood: " << res.algorithmUnderstoodData() << "\n";
  std::cerr << "Test 2 - copy: "
      << CubeCachingAlgorithm::CacheResult(
             res).algorithmUnderstoodData() << "\n";

  res = CubeCachingAlgorithm::CacheResult(tmp);
  std::cerr << "Test 3 - understood: " << res.algorithmUnderstoodData() << "\n";
  std::cerr << "Test 4 - size: " << res.getChunksToFree().size() << "\n";

  return 0;
}

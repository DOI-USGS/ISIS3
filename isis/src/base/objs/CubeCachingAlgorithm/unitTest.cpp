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

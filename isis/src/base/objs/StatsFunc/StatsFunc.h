#include <iostream>
#include <vector>

#include <QString>

#include "Cube.h"

namespace Isis {
  extern void stats(std::vector<char*>&);
  extern Pvl stats(
        Cube *cube,
        double validMin,
        double validMax);
  extern void writeStatsStream(
        Pvl statsPvl,
        bool writeHeader,
        std::ostream *stream);
}

#include <iostream>
#include <vector>

#include <QString>

#include "Cube.h"
#include "UserInterface.h"

namespace Isis {
  extern void stats(UserInterface &ui);
  extern Pvl stats(
        Cube *cube,
        double validMin,
        double validMax);
  extern void writeStatsStream(
        Pvl statsPvl,
        bool writeHeader,
        std::ostream *stream);
}

#ifndef stats_h
#define stats_h

#include <iostream>
#include <vector>

#include <QString>

#include "Cube.h"
#include "UserInterface.h"

namespace Isis {
  extern void stats(UserInterface &ui);
  extern void stats(Cube *inputCube, UserInterface &ui); 
  extern Pvl stats(
        Cube *cube,
        double validMin,
        double validMax);
  extern void writeStatsStream(
        const Pvl &statsPvl,
        bool writeHeader,
        std::ostream *stream);
}

#endif

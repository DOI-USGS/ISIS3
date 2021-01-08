#ifndef cubeatt_h
#define cubeatt_h

#include "Cube.h"
#include "CubeAttribute.h"
#include "UserInterface.h"

namespace Isis{
  extern void cubeatt(Cube *icube, QString outputCubePath, CubeAttributeOutput outputAttributes, bool propTables=false);
  extern void cubeatt(QString inputCubePath, CubeAttributeInput inputAttributes, QString outputCubePath, CubeAttributeOutput outputAttributes, bool propTables=false);
  extern void cubeatt(Cube *icube, UserInterface &ui);
  extern void cubeatt(UserInterface &ui);
}

#endif
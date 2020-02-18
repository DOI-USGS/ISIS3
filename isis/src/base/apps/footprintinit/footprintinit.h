#include "Cube.h"
#include "Pvl.h"
#include "UserInterface.h"

using namespace std;
using namespace Isis;

namespace Isis {

  extern void footprintinit(UserInterface &ui, Pvl *log=nullptr);

  extern void footprintinit(Cube *cube, UserInterface &ui, Pvl *log=nullptr);
}

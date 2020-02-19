#ifndef autoseed_h
#define autoseed_h

#include "ControlNet.h"
#include "Pvl.h"
#include "UserInterface.h"

namespace Isis {
  enum SeedDomain {
    XY,
    SampleLine
  };

  extern void autoseed(UserInterface &ui,
                       Pvl *log = nullptr);
  extern void autoseed(UserInterface &ui, SerialNumberList &serialNumbers, ControlNet *precnet = nullptr, Pvl *log=nullptr);
}

#endif

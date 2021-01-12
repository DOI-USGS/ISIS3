#ifndef findimageoverlaps_h
#define findimageoverlaps_h

#include "FileList.h"
#include "Pvl.h"
#include "UserInterface.h"

namespace Isis {
  extern void findimageoverlaps(UserInterface &ui, Pvl *log=nullptr);

  extern void findimageoverlaps(FileList &images, UserInterface &ui, Pvl *log=nullptr);
}

#endif

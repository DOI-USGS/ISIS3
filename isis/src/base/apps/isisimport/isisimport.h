#ifndef isisimport_h
#define isisimport_h

#include "Pvl.h"
#include "UserInterface.h"
#include "ProcessImport.h"

namespace Isis {
  bool setFileIfExists(ProcessImport& fileImporter, FileName& fileName, const QString &ext);
  extern void isisimport(UserInterface &ui, Pvl *log=nullptr);
}

#endif

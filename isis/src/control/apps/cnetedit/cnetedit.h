/** This is free and unencumbered software released into the public domain.

The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */
#ifndef cnetedit_h
#define cnetedit_h

#include "UserInterface.h"

namespace Isis{
  class ControlNet;
  class ControlPointList;
  class SerialNumberList;

  extern Pvl cnetedit(UserInterface &ui);
  extern Pvl cnetedit(ControlNet &cnet, UserInterface &ui,
                       ControlPointList *cpList=nullptr,
                       SerialNumberList *cubeSnl=nullptr,
                       QMap< QString, QSet<QString> * > *editMeasuresList=nullptr,
                       Pvl *defFile=nullptr,
                       SerialNumberList *validationSnl=nullptr);
}

#endif

#ifndef SmtkStack_h
#define SmtkStack_h
/** This is free and unencumbered software released into the public domain.
The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */                                                                    

#include "SmtkPoint.h"
#include <QHash>
#include <QPair>

namespace Isis {

  typedef QPair<int, int>              SmtkQPair;
  typedef QHash<SmtkQPair, SmtkPoint>  SmtkQStack;

  typedef SmtkQStack::iterator         SmtkQStackIter;  
  typedef SmtkQStack::const_iterator   SmtkQStackConstIter;

};

#endif

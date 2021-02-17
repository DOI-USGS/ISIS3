#ifndef Isis_GuiParameterFactory_h
#define Isis_GuiParameterFactory_h
/** This is free and unencumbered software released into the public domain.
The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */

#include <QGridLayout>
#include "GuiParameter.h"

namespace Isis {
  /**
   * @author ????-??-?? Unknown
   *
   * @internal
   */
  class GuiParameterFactory {
    public:
      static GuiParameter *Create(QGridLayout *grid,
                                  Isis::UserInterface &ui,
                                  int group, int param);

    private:
      GuiParameterFactory() {};
      virtual ~GuiParameterFactory() {};
  };
};



#endif

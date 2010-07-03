#ifndef Isis_GuiParameterFactory_h
#define Isis_GuiParameterFactory_h

#include <QGridLayout>
#include "GuiParameter.h"

namespace Isis {
  class GuiParameterFactory {
    public:
      static GuiParameter* Create (QGridLayout *grid,
                                   Isis::UserInterface &ui,
                                   int group, int param);

    private:
      GuiParameterFactory () {};
      virtual ~GuiParameterFactory () {};
  };
};



#endif

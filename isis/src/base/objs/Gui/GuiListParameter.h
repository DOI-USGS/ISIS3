#ifndef Isis_GuiListParameter_h
#define Isis_GuiListParameter_h

#include <QButtonGroup>

#include "GuiParameter.h"


namespace Isis {

  class GuiListParameter : public GuiParameter {

    Q_OBJECT

    public:

      GuiListParameter (QGridLayout *grid, UserInterface &ui,
                        int group, int param);
      ~GuiListParameter ();

      iString Value ();

      void Set (iString newValue);

      virtual std::vector<std::string> Exclusions();

    private:
      QButtonGroup *p_buttonGroup;
  };
};



#endif



#ifndef Isis_GuiBooleanParameter_h
#define Isis_GuiBooleanParameter_h

#include <QCheckBox>

#include "GuiParameter.h"


namespace Isis {

  class GuiBooleanParameter : public GuiParameter {

    Q_OBJECT

    public:

      GuiBooleanParameter (QGridLayout *grid, UserInterface &ui,
                           int group, int param);
      ~GuiBooleanParameter ();

      iString Value ();

      void Set (iString newValue);

      virtual std::vector<std::string> Exclusions();

      bool IsModified();

    private:
      QCheckBox *p_checkBox;
  };
};



#endif


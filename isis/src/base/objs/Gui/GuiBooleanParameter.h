
#ifndef Isis_GuiBooleanParameter_h
#define Isis_GuiBooleanParameter_h

#include <QCheckBox>

#include "GuiParameter.h"


namespace Isis {
  /**
   * @author ????-??-?? Unknown
   *
   * @internal
   */
  class GuiBooleanParameter : public GuiParameter {

      Q_OBJECT

    public:

      GuiBooleanParameter(QGridLayout *grid, UserInterface &ui,
                          int group, int param);
      ~GuiBooleanParameter();

      QString Value();

      void Set(QString newValue);

      virtual std::vector<QString> Exclusions();

      bool IsModified();

    private:
      QCheckBox *p_checkBox;
  };
};



#endif


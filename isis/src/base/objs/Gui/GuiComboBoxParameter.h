#ifndef Isis_GuiComboBoxParameter_h
#define Isis_GuiComboBoxParameter_h

#include <QComboBox>

#include "GuiParameter.h"


namespace Isis {
  /**
   * @author ????-??-?? Unknown
   *
   * @internal
   */
  class GuiComboBoxParameter : public GuiParameter {

      Q_OBJECT

    public:

      GuiComboBoxParameter(QGridLayout *grid, UserInterface &ui,
                       int group, int param);
      ~GuiComboBoxParameter();

      IString Value();

      void Set(IString newValue);

      virtual std::vector<std::string> Exclusions();

    private:
      QComboBox *p_combo;

    private slots:
      void setOption(int option);
  };
};



#endif


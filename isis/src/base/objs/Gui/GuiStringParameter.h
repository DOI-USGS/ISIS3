#ifndef Isis_GuIStringParameter_h
#define Isis_GuIStringParameter_h

#include <QLineEdit>

#include "GuiParameter.h"


namespace Isis {

//  class QTextEdit;
  /**
   * @author ????-??-?? Unknown
   *
   * @internal
   */
  class GuiStringParameter : public GuiParameter {

      Q_OBJECT

    public:

      GuiStringParameter(QGridLayout *grid, UserInterface &ui,
                         int group, int param);
      ~GuiStringParameter();

      IString Value();

      void Set(IString newValue);

    private:
      QLineEdit *p_lineEdit;

  };
};



#endif


#ifndef Isis_GuiStringParameter_h
#define Isis_GuiStringParameter_h

#include <QLineEdit>

#include "GuiParameter.h"


namespace Isis {

//  class QTextEdit;

  class GuiStringParameter : public GuiParameter {

    Q_OBJECT

    public:

      GuiStringParameter (QGridLayout *grid, UserInterface &ui, 
                        int group, int param);
      ~GuiStringParameter ();

      iString Value ();

      void Set (iString newValue);

    private:
      QLineEdit *p_lineEdit;

  };
};



#endif


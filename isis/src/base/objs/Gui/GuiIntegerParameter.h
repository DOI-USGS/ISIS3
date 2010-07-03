
#ifndef Isis_GuiIntegerParameter_h
#define Isis_GuiIntegerParameter_h

#include <QLineEdit>

#include "GuiParameter.h"


namespace Isis {

//  class QTextEdit;

  class GuiIntegerParameter : public GuiParameter {

    Q_OBJECT

    public:

      GuiIntegerParameter (QGridLayout *grid, UserInterface &ui, 
                        int group, int param);
      ~GuiIntegerParameter ();

      iString Value ();

      void Set (iString newValue);

    private:
      QLineEdit *p_lineEdit;

  };
};



#endif


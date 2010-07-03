#ifndef Isis_GuiDoubleParameter_h
#define Isis_GuiDoubleParameter_h

#include <QLineEdit>

#include "GuiParameter.h"


namespace Isis {

//  class QTextEdit;

  class GuiDoubleParameter : public GuiParameter {

    Q_OBJECT

    public:

      GuiDoubleParameter (QGridLayout *grid, UserInterface &ui, 
                        int group, int param);
      ~GuiDoubleParameter ();

      iString Value ();

      void Set (iString newValue);

    private:
      QLineEdit *p_lineEdit;

  };
};



#endif


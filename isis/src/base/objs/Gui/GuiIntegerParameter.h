
#ifndef Isis_GuiIntegerParameter_h
#define Isis_GuiIntegerParameter_h

#include <QLineEdit>

#include "GuiParameter.h"


namespace Isis {

//  class QTextEdit;
  /**
   * @author ????-??-?? Unknown
   *
   * @internal
   */
  class GuiIntegerParameter : public GuiParameter {

      Q_OBJECT

    public:

      GuiIntegerParameter(QGridLayout *grid, UserInterface &ui,
                          int group, int param);
      ~GuiIntegerParameter();

      QString Value();

      void Set(QString newValue);

    private:
      QLineEdit *p_lineEdit;

  };
};



#endif


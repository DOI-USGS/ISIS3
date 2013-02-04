#ifndef Isis_GuQStringParameter_h
#define Isis_GuQStringParameter_h

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

      QString Value();

      void Set(QString newValue);

    private:
      QLineEdit *p_lineEdit;

  };
};



#endif


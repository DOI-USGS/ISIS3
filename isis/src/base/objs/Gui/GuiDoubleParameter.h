#ifndef Isis_GuiDoubleParameter_h
#define Isis_GuiDoubleParameter_h
/** This is free and unencumbered software released into the public domain.
The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */

#include <QLineEdit>

#include "GuiParameter.h"


namespace Isis {

//  class QTextEdit;
  /**
   * @author ????-??-?? Unknown
   *
   * @internal
   */
  class GuiDoubleParameter : public GuiParameter {

      Q_OBJECT

    public:

      GuiDoubleParameter(QGridLayout *grid, UserInterface &ui,
                         int group, int param);
      ~GuiDoubleParameter();

      QString Value();

      void Set(QString newValue);

    private:
      QLineEdit *p_lineEdit;

  };
};



#endif


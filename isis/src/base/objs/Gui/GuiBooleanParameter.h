#ifndef Isis_GuiBooleanParameter_h
#define Isis_GuiBooleanParameter_h
/** This is free and unencumbered software released into the public domain.
The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */

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


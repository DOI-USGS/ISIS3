#ifndef Isis_GuiListParameter_h
#define Isis_GuiListParameter_h
/** This is free and unencumbered software released into the public domain.
The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */

#include <QButtonGroup>

#include "GuiParameter.h"


namespace Isis {
  /**
   * @author ????-??-?? Unknown
   *
   * @internal
   *
   * @history 2012-11-20 Janet Barrett - Fixed the Set method so that it doesn't
   *                                     stop searching for a matched string until an
   *                                     exact match is found or it has gone through the
   *                                     entire list. Fixes #554.
   */
  class GuiListParameter : public GuiParameter {

      Q_OBJECT

    public:

      GuiListParameter(QGridLayout *grid, UserInterface &ui,
                       int group, int param);
      ~GuiListParameter();

      QString Value();

      void Set(QString newValue);

      virtual std::vector<QString> Exclusions();

    private:
      QButtonGroup *p_buttonGroup;
  };
};



#endif


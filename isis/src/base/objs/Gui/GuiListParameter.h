#ifndef Isis_GuiListParameter_h
#define Isis_GuiListParameter_h

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

      IString Value();

      void Set(IString newValue);

      virtual std::vector<std::string> Exclusions();

    private:
      QButtonGroup *p_buttonGroup;
  };
};



#endif


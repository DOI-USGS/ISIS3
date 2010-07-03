#include "GuiListParameter.h"
#include "GuiCubeParameter.h"
#include "GuiFilenameParameter.h"
#include "GuiStringParameter.h"
#include "GuiIntegerParameter.h"
#include "GuiDoubleParameter.h"
#include "GuiBooleanParameter.h"
#include "GuiParameterFactory.h"
#include "UserInterface.h"
#include "Application.h"

namespace Isis {
  GuiParameter* GuiParameterFactory::Create (QGridLayout *grid,
                                             Isis::UserInterface &ui,
                                             int group, int param) {
    std::string paramType = ui.ParamType(group, param);

    GuiParameter *p = NULL;
    if (ui.ParamListSize(group, param) > 0) {
      p = new GuiListParameter (grid, ui, group, param);
    }
    else if (paramType == "cube") {
      p = new GuiCubeParameter (grid, ui, group, param);
    }
    else if (paramType == "filename") {
      p = new GuiFilenameParameter (grid, ui, group, param);
    }
    else if (paramType == "string") {
      p = new GuiStringParameter (grid, ui, group, param);
    }
    else if (paramType == "integer") {
      p = new GuiIntegerParameter (grid, ui, group, param);
    }
    else if (paramType == "double") {
      p = new GuiDoubleParameter (grid, ui, group, param);
    }
    else if (paramType == "boolean") {
      p = new GuiBooleanParameter (grid, ui, group, param);
    }
    else {
      std::string msg = "Invalid parameter type in XML [" + paramType + "]";
      throw iException::Message(iException::Programmer,msg,_FILEINFO_);
    }

    return p;
  }
}




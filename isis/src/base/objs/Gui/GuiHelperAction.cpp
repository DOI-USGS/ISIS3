#include "GuiHelperAction.h"
#include <iostream>

namespace Isis {
  GuiHelperAction::GuiHelperAction(QObject *parent, const QString &funct) : QAction(parent) {
    p_funct = funct;
    connect(this,SIGNAL(triggered(bool)),this,SLOT(retrigger()));
  }

  GuiHelperAction::~GuiHelperAction() {};

  void GuiHelperAction::retrigger() {
    emit trigger(p_funct);
  }

}



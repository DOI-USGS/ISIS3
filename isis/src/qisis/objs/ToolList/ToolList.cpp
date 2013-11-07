#include "ToolList.h"

#include <QStackedWidget>
#include <QToolBar>

#include "RubberBandTool.h"
#include "Tool.h"

namespace Isis {
  ToolList::ToolList() {
    m_activeToolBarStack = NULL;
    m_rubberBandTool = NULL;
  }


  ToolList::~ToolList() {
    delete m_activeToolBarStack;
  }


  void ToolList::append(Tool *tool) {
    m_tools.append(tool);

    if (tool) {
      tool->setList(this);

      RubberBandTool *rubberBandTool = dynamic_cast<RubberBandTool *>(tool);

      if (rubberBandTool) {
        m_rubberBandTool = rubberBandTool;
      }
    }
  }


  int ToolList::count() const {
    return m_tools.count();
  }


  RubberBandTool *ToolList::rubberBandTool() {
    return m_rubberBandTool;
  }


  QStackedWidget *ToolList::toolBarStack() {
    return m_activeToolBarStack;
  }


  QStackedWidget *ToolList::toolBarStack(QToolBar *toolBar) {
    if(!m_activeToolBarStack) {
      m_activeToolBarStack = new QStackedWidget;
      toolBar->addWidget(m_activeToolBarStack);
    }

    return m_activeToolBarStack;
  }


  Tool *ToolList::operator[](int index) {
    return m_tools[index];
  }
}

#ifndef ToolList_h
#define ToolList_h

/** This is free and unencumbered software released into the public domain.

The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */

#include <QList>

#include <QPointer>

class QStackedWidget;
class QToolBar;

namespace Isis {
  class RubberBandTool;
  class Tool;

  /**
  * @brief Allows tools to share data between each other
  *
  * @ingroup Visualization Tools
  *
  * @author 2012-09-18 Steven Lambright
  *
  * @internal
  */
  class ToolList {
    public:
      ToolList();
      ~ToolList();

      void append(Tool *tool);
      int count() const;
      RubberBandTool *rubberBandTool();
      QStackedWidget *toolBarStack();
      QStackedWidget *toolBarStack(QToolBar *toolBar);

      Tool *operator[](int index);

    private:
      Q_DISABLE_COPY(ToolList);

      QList<Tool *> m_tools;

      QPointer<QStackedWidget> m_activeToolBarStack;
      QPointer<RubberBandTool> m_rubberBandTool;
  };
}

#endif

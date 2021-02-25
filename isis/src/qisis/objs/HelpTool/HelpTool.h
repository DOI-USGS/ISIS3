#ifndef HelpTool_h
#define HelpTool_h

/** This is free and unencumbered software released into the public domain.

The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */

#include <QAction>
#include "Tool.h"

namespace Isis {
  /**
  * @brief Qisis Help Tool
  *
  * @ingroup Visualization Tools
  *
  * @author ????-??-?? Jeff Anderson
  *
  * @internal @history 2007-06-12 Tracie Sucharski - Added aboutProgram method
  *
  */
  class HelpTool : public Tool {
      Q_OBJECT

    public:
      HelpTool(QWidget *parent);
      void addTo(QMenu *menu);
      void addToPermanent(QToolBar *perm);

      QString menuName() const {
        return "&Help";
      }

    public slots:
      void whatsThis();

    private:
      QAction *p_whatsThis;
      QAction *p_aboutProgram;

    private slots:
      void aboutProgram();
  };
};

#endif

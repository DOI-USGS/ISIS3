#ifndef Isis_GuiFileNameParameter_h
#define Isis_GuiFileNameParameter_h

/** This is free and unencumbered software released into the public domain.
The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */

#include "GuiParameter.h" // parent

namespace Isis {

  /**
   * @author 2006-10-31 ???
   *
   * @internal
   *   @history 2009-11-10 Mackenzie Boyd - Moved SelectFile method up to parent
   *                          class GuiParameter, also moved member pointers to
   *                          QToolButton and QLineEdit up.
   *   @history 2009-12-15 Travis Addair - Fixed bug with button for opening file
   *                          dialog, made this class a parent for
   *                          GuiCubeParameter, and moved SelectFile method back
   *                          to this class, no longer prompting users to confirm
   *                          overwriting a file.
   *   @history  2010-07-19 Jeannie Walldren - Modified SelectFile() method to
   *                            allow users to view files in the directory.
   *                            Updated documentation.
   */

  class GuiFileNameParameter : public GuiParameter {

      Q_OBJECT

    public:

      GuiFileNameParameter(QGridLayout *grid, UserInterface &ui,
                           int group, int param);
      ~GuiFileNameParameter();

      QString Value();

      void Set(QString newValue);

    protected slots:
      virtual void SelectFile();
  };
};



#endif


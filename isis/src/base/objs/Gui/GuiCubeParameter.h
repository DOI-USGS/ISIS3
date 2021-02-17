#ifndef Isis_GuiCubeParameter_h
#define Isis_GuiCubeParameter_h

/** This is free and unencumbered software released into the public domain.
The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */

#include "GuiFilenameParameter.h" //parent

namespace Isis {

  /**
   * @author 2006-10-31 ???
   *
   * @internal
   *   @history 2009-11-10 Mackenzie Boyd - Moved SelectFile method up to parent
   *                          class GuiParameter, also moved member pointers to
   *                          QToolButton and QLineEdit up.
   *   @history 2009-12-15 Travis Addair - Fixed bug with button for opening file
   *                          dialog, made this class a child of
   *                          GuiFileNameParameter, and moved SelectFile method
   *                          back to this class, no longer prompting users to
   *                          confirm overwriting a file.
   *   @history  2010-07-19 Jeannie Walldren - Removed SelectFile() method since
   *                           it was identical to parent's method after output
   *                           file options were modified.  Updated
   *                           documentation. Removed unnecessary #includes.
   */

  class GuiCubeParameter : public GuiFileNameParameter {

      Q_OBJECT

    public:

      GuiCubeParameter(QGridLayout *grid, UserInterface &ui,
                       int group, int param);
      ~GuiCubeParameter();

    protected slots:
      // Method identical to parent method GuiFileNameParameter::SelectFile()
      // Removed from this class 2010-07-15
      // Previous documentation:
      //    * @internal
      //    *   @history  2007-05-16 Tracie Sucharski - For cubes located in CWD, do
      //    *                             not include path in the lineEdit.
      //    *   @history  2007-06-05 Steven Koechle - Corrected problem where
      //    *                             output cube was being opened not
      //    *                             saved.
      // virtual void SelectFile();

    private:
      QMenu *p_menu;

    private slots:
      void SelectAttribute();
      void ViewCube();
      void ViewLabel();
  };
};



#endif


#ifndef Isis_GuiCubeParameter_h
#define Isis_GuiCubeParameter_h

/**
 * @file
 * $Revision: 1.5 $
 * $Date: 2009/12/15 20:44:57 $
 *
 *   Unless noted otherwise, the portions of Isis written by the USGS are
 *   public domain. See individual third-party library and package descriptions
 *   for intellectual property information, user agreements, and related
 *   information.
 *
 *   Although Isis has been used by the USGS, no warranty, expressed or
 *   implied, is made by the USGS as to the accuracy and functioning of such
 *   software and related material nor shall the fact of distribution
 *   constitute any such warranty, and no responsibility is assumed by the
 *   USGS in connection therewith.
 *
 *   For additional information, launch
 *   $ISISROOT/doc//documents/Disclaimers/Disclaimers.html
 *   in a browser or see the Privacy &amp; Disclaimers page on the Isis website,
 *   http://isis.astrogeology.usgs.gov, and the USGS privacy and disclaimers on
 *   http://www.usgs.gov/privacy.html.
 */

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


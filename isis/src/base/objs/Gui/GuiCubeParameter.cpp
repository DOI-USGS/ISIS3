/**
 * @file
 * $Revision: 1.12 $
 * $Date: 2010/01/06 19:40:17 $
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

#include <sstream>

#include <QDialog>
#include <QDir>
#include <QFileDialog>
#include <QHBoxLayout>
#include <QLineEdit>
#include <QMenu>
#include <QString>
#include <QTextEdit>
#include <QToolButton>


#include "Application.h"
#include "Cube.h"
#include "FileName.h"
#include "GuiCubeParameter.h"
#include "GuiInputAttribute.h"
#include "GuiOutputAttribute.h"
#include "IException.h"
#include "ProgramLauncher.h"
#include "Pvl.h"
#include "UserInterface.h"


namespace Isis {

  /**
   * @brief Constructs GuiCubeParameter object
   *
   * @param grid Pointer to QGridLayout
   * @param ui User interface object
   * @param group Index of group
   * @param param Index of parameter
   */
  GuiCubeParameter::GuiCubeParameter(QGridLayout *grid, UserInterface &ui,
                                     int group, int param) :
    GuiFileNameParameter(grid, ui, group, param) {
    p_menu = new QMenu();

    QAction *fileAction = new QAction(this);
    fileAction->setText("Select Cube");
    connect(fileAction, SIGNAL(triggered(bool)), this, SLOT(SelectFile()));
    p_menu->addAction(fileAction);

    QAction *attAction = new QAction(this);
    attAction->setText("Change Attributes ...");
    connect(attAction, SIGNAL(triggered(bool)), this, SLOT(SelectAttribute()));
    p_menu->addAction(attAction);

    QAction *viewAction = new QAction(this);
    viewAction->setText("View cube");
    connect(viewAction, SIGNAL(triggered(bool)), this, SLOT(ViewCube()));
    p_menu->addAction(viewAction);

    QAction *labAction = new QAction(this);
    labAction->setText("View labels");
    connect(labAction, SIGNAL(triggered(bool)), this, SLOT(ViewLabel()));
    p_menu->addAction(labAction);

    p_fileButton->setMenu(p_menu);
    p_fileButton->setPopupMode(QToolButton::MenuButtonPopup);
    QString optButtonWhatsThisText = "<p><b>Function:</b> \
            Opens a file chooser window to select a file from</p> <p>\
            <b>Hint: </b> Click the arrow for more cube parameter options</p>";
    p_fileButton->setWhatsThis(optButtonWhatsThisText);

    p_type = CubeWidget;
  }


  /**
   *  Destructor of GuiCubeParameter object.
   */
  GuiCubeParameter::~GuiCubeParameter() {
    delete p_menu;
  }


  /**
   * Select cube attributes.
   */
  void GuiCubeParameter::SelectAttribute() {
    if(p_ui->ParamFileMode(p_group, p_param) == "input") {
      Isis::CubeAttributeInput att(p_lineEdit->text().toStdString());
      std::string curAtt = att.toString();
      std::string newAtt;
      int status = GuiInputAttribute::GetAttributes(curAtt, newAtt,
                   p_ui->ParamName(p_group, p_param),
                   p_fileButton);
      if((status == 1) && (curAtt != newAtt)) {
        Isis::FileName f(p_lineEdit->text().toStdString());
        p_lineEdit->setText((iString)(f.expanded() + newAtt));
      }
    }
    else {
      Isis::CubeAttributeOutput att("+" + p_ui->PixelType(p_group, p_param));
      bool allowProp = att.propagatePixelType();
      att.addAttributes(FileName(p_lineEdit->text()));

      std::string curAtt = att.toString();
      std::string newAtt;
      int status = GuiOutputAttribute::GetAttributes(curAtt, newAtt,
                   p_ui->ParamName(p_group, p_param),
                   allowProp,
                   p_fileButton);
      if((status == 1) && (curAtt != newAtt)) {
        Isis::FileName f(p_lineEdit->text().toStdString());
        p_lineEdit->setText((iString)(f.expanded() + newAtt));
      }
    }

    return;
  }

  /**
   * Opens cube in qview.
   * @throws Isis::Exception::User "You must enter a cube name to open"
   */
  void GuiCubeParameter::ViewCube() {
    try {
      // Make sure the user entered a value
      if(IsModified()) {
        std::string cubeName = Value();

        // Check to make sure the cube can be opened
        Isis::Cube temp;
        temp.open(cubeName);
        temp.close();

        // Open the cube in Qview
        iString command = "$ISISROOT/bin/qview " + cubeName + " &";
        ProgramLauncher::RunSystemCommand(command);
      }
      // Throw an error if no cube name was entered
      else {
        std::string msg = "You must enter a cube name to open";
        throw IException(IException::User, msg, _FILEINFO_);
      }
    }
    catch(IException &e) {
      Isis::iApp->GuiReportError(e);
    }
  }

  /**
   * Displays cube label in the GUI log.
   * @throws Isis::Exception::User "You must enter a cube name to open"
   */
  void GuiCubeParameter::ViewLabel() {
    try {
      // Make sure the user entered a value
      if(IsModified()) {
        std::string cubeName = Value();

        // Check to make sure the cube can be opened
        Isis::Cube temp;
        temp.open(cubeName);

        // Get the label and write it out to the log
        Isis::Pvl *label = temp.getLabel();
        Isis::Application::GuiLog(*label);

        // Close the cube
        temp.close();

      }
      else {
        std::string msg = "You must enter a cube name to open";
        throw IException(IException::User, msg, _FILEINFO_);
      }
    }
    catch(IException &e) {
      Isis::iApp->GuiReportError(e);
    }

  }
}


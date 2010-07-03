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

#include "GuiCubeParameter.h"

#include "Application.h"
#include "Cube.h"
#include "Filename.h"
#include "Preference.h"
#include "GuiInputAttribute.h"
#include "GuiOutputAttribute.h"
#include "iException.h"
#include "Pvl.h"
#include "System.h"
#include "UserInterface.h"


namespace Isis {

  GuiCubeParameter::GuiCubeParameter(QGridLayout *grid, UserInterface &ui,
                                     int group, int param) :
  GuiFilenameParameter(grid, ui, group, param) {
    p_menu = new QMenu ();

    QAction *fileAction = new QAction(this);
    fileAction->setText("Select Cube");
    connect(fileAction,SIGNAL(triggered(bool)),this,SLOT(SelectFile()));
    p_menu->addAction(fileAction);

    QAction *attAction = new QAction(this);
    attAction->setText("Change Attributes ...");
    connect(attAction,SIGNAL(triggered(bool)),this,SLOT(SelectAttribute()));
    p_menu->addAction(attAction);

    QAction *viewAction = new QAction(this);
    viewAction->setText("View cube");
    connect(viewAction,SIGNAL(triggered(bool)),this,SLOT(ViewCube()));
    p_menu->addAction(viewAction);

    QAction *labAction = new QAction(this);
    labAction->setText("View labels");
    connect(labAction,SIGNAL(triggered(bool)),this,SLOT(ViewLabel()));
    p_menu->addAction(labAction);

    p_fileButton->setMenu(p_menu);
    p_fileButton->setPopupMode(QToolButton::MenuButtonPopup);
    QString optButtonWhatsThisText = "<p><b>Function:</b> \
            Opens a file chooser window to select a file from</p> <p>\
            <b>Hint: </b> Click the arrow for more cube parameter options</p>"; 
    p_fileButton->setWhatsThis(optButtonWhatsThisText); 

    p_type = CubeWidget;
  }


  GuiCubeParameter::~GuiCubeParameter() {
    delete p_menu;
  }


  /**
   * Gets an input/output file from a GUI filechooser or typed in
   * filename.
   * 
   * @internal
   * @history  2007-05-16 Tracie Sucharski - For cubes located in CWD, do
   *                           not include path in the lineEdit.
   * @history  2007-06-05 Steven Koechle - Corrected problem where
   *                           output cube was being opened not
   *                           saved.
   */
  void GuiCubeParameter::SelectFile() {
    // What directory do we look in?
    QString dir;
    if ((p_lineEdit->text().length() > 0) &&
        (p_lineEdit->text().toStdString() != p_ui->ParamInternalDefault(p_group,p_param))) {
      Isis::Filename f(p_lineEdit->text().toStdString());
      dir = (QString)(iString)f.Expanded();
    } else if (p_ui->ParamPath(p_group,p_param).length() > 0) {
      Isis::Filename f(p_ui->ParamPath(p_group,p_param));
      dir = (QString)(iString)Filename(p_ui->ParamPath(p_group,p_param)).Expanded();
    }

    // Set up the filter
    QString filter = (iString)p_ui->ParamFilter(p_group,p_param);
    if (filter.isEmpty()) {
      filter = "Any(*)";
    } 
    else {
      filter += ";;Any(*)";
    }


    // Get the filename
    QString s;
    if (p_ui->ParamFileMode(p_group,p_param) == "input") {
      s = QFileDialog::getOpenFileName(p_fileButton,"Select file",dir,filter);
    } 
    else {
      // if/else statements are functionally identical, but left in case 
      // different file selection capabilities are desired for different
      // preferences later
      if (Preference::Preferences().FindGroup("CubeCustomization").FindKeyword("Overwrite")[0] == "Allow") {
        QFlags<QFileDialog::Option> options (QFileDialog::DontConfirmOverwrite);
        s = QFileDialog::getSaveFileName(p_fileButton,"Select file",dir,filter,0,options);
      }
      else {
        QFlags<QFileDialog::Option> options (QFileDialog::DontConfirmOverwrite);
        s = QFileDialog::getSaveFileName(p_fileButton,"Select file",dir,filter,0,options);
      }
    }

    if (s != "") {
      Isis::Filename f(s.toStdString());
      if (f.absoluteDir() == QDir::currentPath()) {
        s = (QString)(iString)f.Name();
      }
      Set(s.toStdString());
    }
  } 

  void GuiCubeParameter::SelectAttribute() {
    if (p_ui->ParamFileMode(p_group,p_param) == "input") {
      Isis::CubeAttributeInput att(p_lineEdit->text().toStdString());
      std::string curAtt = att.BandsStr();
      std::string newAtt;
      int status = GuiInputAttribute::GetAttributes (curAtt,newAtt,
                                                     p_ui->ParamName(p_group,p_param),
                                                     p_fileButton);
      if ((status == 1) && (curAtt != newAtt)) {
        Isis::Filename f(p_lineEdit->text().toStdString());
        p_lineEdit->setText((iString)(f.Expanded() + newAtt));
      }
    } else {
      Isis::CubeAttributeOutput att("+" + p_ui->PixelType(p_group,p_param));
      bool allowProp = att.PropagatePixelType();
      att.Set(p_lineEdit->text().toStdString());

      std::string curAtt;
      att.Write(curAtt);
      std::string newAtt;
      int status = GuiOutputAttribute::GetAttributes (curAtt,newAtt,
                                                      p_ui->ParamName(p_group,p_param),
                                                      allowProp,
                                                      p_fileButton);
      if ((status == 1) && (curAtt != newAtt)) {
        Isis::Filename f(p_lineEdit->text().toStdString());
        p_lineEdit->setText((iString)(f.Expanded() + newAtt));
      }
    }

    return;
  }

  void GuiCubeParameter::ViewCube() {
    try {
      // Make sure the user entered a value
      if (IsModified()) {
        std::string cubeName = Value();

        // Check to make sure the cube can be opened
        Isis::Cube temp;
        temp.Open(cubeName);
        temp.Close();

        // Open the cube in Qview
        std::string command = "qview " + cubeName + " &";
        Isis::System(command);
      }
      // Throw an error if no cube name was entered
      else {
        std::string msg = "You must enter a cube name to open";
        throw Isis::iException::Message(Isis::iException::User,msg,_FILEINFO_);
      }
    } catch (Isis::iException &e) {
      Isis::iApp->GuiReportError(e);
    }
  }

  void GuiCubeParameter::ViewLabel() {
    try {
      // Make sure the user entered a value
      if (IsModified()) {
        std::string cubeName = Value();

        // Check to make sure the cube can be opened
        Isis::Cube temp;
        temp.Open(cubeName);

        // Get the label and write it out to the log
        Isis::Pvl *label = temp.Label();
        Isis::Application::GuiLog(*label);

        // Close the cube
        temp.Close();

      } else {
        std::string msg = "You must enter a cube name to open";
        throw Isis::iException::Message(Isis::iException::User,msg,_FILEINFO_);
      }
    } catch (Isis::iException &e) {
      Isis::iApp->GuiReportError(e);
    }

  }
}


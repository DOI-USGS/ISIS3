/**                                                                       
 * @file                                                                  
 * $Revision: 1.11 $ 
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

#include <QFileDialog>
#include <QFontMetrics>
#include <QHBoxLayout>

#include "GuiFilenameParameter.h"

#include "iString.h"
#include "UserInterface.h"


namespace Isis {

  GuiFilenameParameter::GuiFilenameParameter(QGridLayout *grid, UserInterface &ui,
                                             int group, int param) :
  GuiParameter(grid, ui, group, param) {
    connect(p_lineEdit,SIGNAL(textChanged(const QString &)),this,SIGNAL(ValueChanged()));
    grid->addWidget(p_lineEdit,param,2);

    grid->addWidget(p_fileButton,param,3);  

    QAction *action = new QAction(this);
    std::string file = Filename("$ISIS3DATA/base/icons/view_tree.png").Expanded();
    action->setIcon(QPixmap((iString)file));
    connect(action,SIGNAL(triggered(bool)),this,SLOT(SelectFile()));

    p_fileButton->setIconSize(QSize(22,22));
    p_fileButton->setIcon(QPixmap((iString)file));
    p_fileButton->setDefaultAction(action);
    p_fileButton->setToolTip("Select file");
    QString fileButtonWhatsThisText = "<p><b>Function:</b> \
            Opens a file chooser window to select a file from</p>"; 
    p_fileButton->setWhatsThis(fileButtonWhatsThisText); 

    if (p_ui->HelpersSize(group,param) != 0) {
      grid->addWidget(AddHelpers(p_lineEdit),param,4);
    }

    RememberWidget(p_lineEdit);
    RememberWidget(p_fileButton);

    p_type = FilenameWidget;
  }


  GuiFilenameParameter::~GuiFilenameParameter() {}


  void GuiFilenameParameter::Set (iString newValue) {
    p_lineEdit->setText (newValue.ToQt());
  }


  iString GuiFilenameParameter::Value () {
    return p_lineEdit->text().toStdString();
  }

  /**
   *
   * @internal
   * @history  2007-05-16 Tracie Sucharski - For files located in CWD, do
   *                           not include path in the lineEdit.
   * @history  2007-06-05 Steven Koechle - Corrected problem where
   *                           output Filename was being opened
   *                           not saved. Defaulted to output mode
   *                           if not specified.
   * @history  2009-11-02 Mackenzie Boyd - Corrected building of 
   *                           filter string to match
   *                           GuiCubeParameter.cpp and, if no
   *                           filters specified, to specify
   *                           "Any(*)" not ";;Any(*)" as it had
   *                           been doing.
   */
  void GuiFilenameParameter::SelectFile() {
    // What directory do we look in?
    QString dir;
    if ((p_lineEdit->text().length() > 0) &&
        (p_lineEdit->text().toStdString() != p_ui->ParamInternalDefault(p_group,p_param))) {
      Isis::Filename f(p_lineEdit->text().toStdString());
      dir = (QString)(iString)f.Expanded();
    } else if (p_ui->ParamPath(p_group,p_param).length() > 0) {
      Isis::Filename f(p_ui->ParamPath(p_group,p_param));
      dir = (QString)(iString)f.Expanded();
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
      QFlags<QFileDialog::Option> options (QFileDialog::ShowDirsOnly);
      s = QFileDialog::getSaveFileName(p_fileButton,"Select file",dir,filter,0,options);
    }

    if (s != "") {
      Isis::Filename f(s.toStdString());
      if (f.absoluteDir() == QDir::currentPath()) {
        s = (QString)(iString)f.Name();
      }
      Set(s.toStdString());
    }
  }
}


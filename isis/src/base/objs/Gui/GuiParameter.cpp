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

#include <QFileDialog>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QToolButton>

#include "GuiParameter.h"

#include "Application.h"
#include "Filename.h"
#include "GuiHelperAction.h"
#include "UserInterface.h"

#include "GuiFilenameParameter.h"
#include "GuiCubeParameter.h"

namespace Isis {
  //! Constructor
  GuiParameter::GuiParameter(QGridLayout *grid, UserInterface &ui,
                             int group, int param) : QObject() {
    p_ui = &ui;
    p_group = group;
    p_param = param;

    p_name = ui.ParamName(group, param);

    p_fileButton = new QToolButton();
    p_lineEdit = new QLineEdit();

    p_label = new QLabel((iString)p_ui->ParamName(group, param));
    p_label->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
    p_label->setToolTip((iString)p_ui->ParamBrief(group,param));
    grid->addWidget(p_label, param, 0, Qt::AlignCenter);

    QString whatsThis;
    whatsThis  = (QString)(iString)("<b>Parameter:</b> " + p_ui->ParamName(group,param));
    whatsThis += (QString)(iString)("<p><b>Type:</b> " + p_ui->ParamType(group,param) + "</p>");
    whatsThis += (QString)(iString)("<p><b>Brief:</b> " + p_ui->ParamBrief(group,param) + "</p>");
    iString def = p_ui->ParamDefault(group,param);
    if (def == "") def = "None";
    whatsThis += "<p><b>Default: </b>" + QString(def) + "</p>";
    iString intdef = p_ui->ParamInternalDefault(group,param);
    if (intdef != "") {
      whatsThis += "<p><b>Internal Default: </b> " + QString(intdef) + "</p>";
    }
    iString pixtype = p_ui->PixelType(group,param);
    if (pixtype != "") {
      whatsThis += "<p><b>Pixel Type: </b> " + QString(pixtype) + "</p>";
    }
    iString pmin = p_ui->ParamMinimum(group,param);
    if (pmin != "") {
      if (p_ui->ParamMinimumInclusive(group,param) =="YES") {
        whatsThis += "<p><b>Greater Than Or Equal To: </b>" +
          QString(pmin) + "</p>";
      }
      else {
        whatsThis += "<p><b>Greater Than: </b>" + QString(pmin) + "</p>";
      }
    }
    iString pmax = p_ui->ParamMaximum(group,param);
    if (pmax != "") {
      if (p_ui->ParamMaximumInclusive(group,param) =="YES") {
        whatsThis += "<p><b>Less Than Or Equal To: </b>" +
          QString(pmax) + "</p>";
      }
      else {
        whatsThis += "<p><b>Less Than: </b>" + QString(pmax) + "</p>";
      }
    }
    if (p_ui->ParamLessThanSize(group,param) > 0) {
      whatsThis += "<p><b>Less Than: </b>" + 
        QString((iString)p_ui->ParamLessThan(group,param,0));
      for (int l=1; l<p_ui->ParamLessThanSize(group,param); l++) {
        whatsThis += ", " + QString((iString)p_ui->ParamLessThan(group,param,l));
      }
      whatsThis += "</p>";
    }
    if (p_ui->ParamLessThanOrEqualSize(group,param) > 0) {
      whatsThis += "<p><b>Less Than Or Equal: </b>" + 
        QString((iString)p_ui->ParamLessThanOrEqual(group,param,0));
      for (int l=1; l<p_ui->ParamLessThanOrEqualSize(group,param); l++) {
        whatsThis += ", " + 
          QString((iString)p_ui->ParamLessThanOrEqual(group,param,l));
      }
      whatsThis += "</p>";
    }
    if (p_ui->ParamNotEqualSize(group,param) > 0) {
      whatsThis += "<p><b>Not Equal: </b>" + 
        QString((iString)p_ui->ParamNotEqual(group,param,0));
      for (int l=1; l<p_ui->ParamNotEqualSize(group,param); l++) {
        whatsThis += ", " + QString((iString)p_ui->ParamNotEqual(group,param,l));
      }
      whatsThis += "</p>";
    }
    if (p_ui->ParamGreaterThanSize(group,param) > 0) {
      whatsThis += "<p><b>Greater Than: </b>" + 
        QString((iString)p_ui->ParamGreaterThan(group,param,0));
      for (int l=1; l<p_ui->ParamGreaterThanSize(group,param); l++) {
        whatsThis += ", " + 
          QString((iString)p_ui->ParamGreaterThan(group,param,l));
      }
      whatsThis += "</p>";
    }
    if (p_ui->ParamGreaterThanOrEqualSize(group,param) > 0) {
      whatsThis += "<p><b>Greater Than Or Equal: </b>" + 
        QString((iString)p_ui->ParamGreaterThanOrEqual(group,param,0));
      for (int l=1; l<p_ui->ParamGreaterThanOrEqualSize(group,param); l++) {
        whatsThis += ", " + 
          QString((iString)p_ui->ParamGreaterThanOrEqual(group,param,l));
      }
      whatsThis += "</p>";
    }
    if (p_ui->ParamIncludeSize(group,param) >0) {
      whatsThis += "<p><b>Inclusions: </b>" + 
        QString((iString)p_ui->ParamInclude(group,param,0));
      for (int l=1; l<p_ui->ParamIncludeSize(group,param); l++) {
        whatsThis += ", " + 
          QString((iString)p_ui->ParamInclude(group,param,l));
      }
      whatsThis += "</p>";
    }
    if (p_ui->ParamExcludeSize(group,param) >0) {
      whatsThis += "<p><b>Exclusions: </b>" + 
        QString((iString)p_ui->ParamExclude(group,param,0));
      for (int l=1; l<p_ui->ParamExcludeSize(group,param); l++) {
        whatsThis += ", " + 
          QString((iString)p_ui->ParamExclude(group,param,l));
      }
      whatsThis += "</p>";
    }                
    if (p_ui->ParamOdd(group,param) != "") {
      whatsThis += "<p><b>Odd: </b>" + 
        QString((iString)p_ui->ParamOdd(group,param)) + "</p>";
    }
    p_label->setWhatsThis(whatsThis);

    p_helperMenu = NULL;
  }


  //! Destructor
  GuiParameter::~GuiParameter() {
    if(p_helperMenu) {
      delete p_helperMenu;
      p_helperMenu = NULL;
    }

    p_widgetList.clear();
  }

  //! Change the parameter to the default value
  void GuiParameter::SetToDefault () {
    if (p_ui->ParamDefault (p_group, p_param).size() > 0) {
      Set(p_ui->ParamDefault (p_group, p_param));
    }
    else if (p_ui->ParamInternalDefault (p_group, p_param).size () > 0) {
      Set(p_ui->ParamInternalDefault (p_group, p_param));
    }
    else {
      Set ("");
    }
  }

  //! Change the parameter to the current user interface value
  void GuiParameter::SetToCurrent () {
    if (p_ui->WasEntered(p_name)) {
      Set (p_ui->GetAsString(p_name));
    }
    else {
      SetToDefault();
    }
  }

  //! Return if the parameter value is different from the default value
  bool GuiParameter::IsModified() {
    if (!IsEnabled()) return false;
    if (p_ui->ParamDefault (p_group, p_param).size() > 0) {
      if (Value() == p_ui->ParamDefault (p_group, p_param)) return false;
    }
    else if (p_ui->ParamInternalDefault (p_group, p_param).size () > 0) {
      if (Value() == p_ui->ParamInternalDefault (p_group, p_param)) return false;
    }
    else {
      if (Value() == "") return false;
    }
    return true;
  }

  //! Update the value on the GUI with the value in the UI
  void GuiParameter::Update() {
    if (p_ui->WasEntered(p_name)) {
      Set (p_ui->GetAsString(p_name));
    }
    else if (p_ui->ParamDefault (p_group, p_param).size() > 0) {
      Set (p_ui->ParamDefault (p_group, p_param));
    }
    else if (p_ui->ParamInternalDefault (p_group, p_param).size () > 0) {
      Set (p_ui->ParamInternalDefault (p_group, p_param));
    }
    else {
      Set ("");
    }
  }

  //! Add widgets to a list for enabling/disabling
  void GuiParameter::RememberWidget(QWidget *w) {
    p_widgetList.push_back(w);
  }

  //! Enable or disable the parameter
  void GuiParameter::SetEnabled(bool enabled) {
    p_label->setEnabled(enabled);
    for (int i=0; i<p_widgetList.size(); i++) {
      p_widgetList[i]->setEnabled(enabled);
    }
  }

  //! Return list of current exclusions
  std::vector<std::string> GuiParameter::Exclusions() {
    std::vector<std::string> list;
    return list;
  }

  //! Sets up helper button
  QWidget *GuiParameter::AddHelpers(QObject *lo) {
    // Just make one single helper button
    if (p_ui->HelpersSize(p_group,p_param) == 1) {
      GuiHelperAction *action = 
        new GuiHelperAction(lo,(iString)p_ui->HelperFunction(p_group,p_param,0));
      if (p_ui->HelperIcon(p_group,p_param,0) != "") {
        iString file = Filename(
          p_ui->HelperIcon(p_group,p_param,0)).Expanded();
        action->setIcon(QIcon(QPixmap(file)));
      }
      else {
        action->setText((iString)p_ui->HelperButtonName(p_group, p_param, 0));
      }
      action->setToolTip((iString)p_ui->HelperBrief(p_group,p_param,0));
      QString helperText = "<p><b>Function:</b> " + 
        QString((iString)p_ui->HelperDescription(p_group,p_param,0)) + "</p>"; 
      action->setWhatsThis(helperText);
      connect (action,SIGNAL(trigger(const QString &)),this,
               SIGNAL(HelperTrigger(const QString &)));

      QToolButton *helper = new QToolButton();
      helper->setText((iString)p_ui->HelperButtonName(p_group, p_param, 0));
      helper->setDefaultAction(action);

      if (p_ui->HelperIcon(p_group,p_param,0) != "") {
        helper->setText("");
        iString file = Filename(
          p_ui->HelperIcon(p_group,p_param,0)).Expanded();
        helper->setIconSize(QSize(22,22));
        helper->setIcon(QIcon(QPixmap(file)));
      }
      else {
        helper->setFixedWidth(helper->fontMetrics().width(
          " " + helper->text() + " "));
      }
      RememberWidget(helper);
      return helper;
    }

    // Make a drop down menu of helper buttons
    else {
      if(p_helperMenu) {
        throw iException::Message(iException::Programmer, "Can not call GuiParameter::AddHelpers twice", _FILEINFO_);
      }

      p_helperMenu = new QMenu ();

      // Create default action item
      GuiHelperAction *action = 
        new GuiHelperAction(lo,(iString)p_ui->HelperFunction(p_group,p_param,0));
      if (p_ui->HelperIcon(p_group,p_param,0) != "") {
        iString file = Filename(
          p_ui->HelperIcon(p_group,p_param,0)).Expanded();
        action->setIcon(QIcon(QPixmap(file)));
      }
      else {
        action->setText((iString)p_ui->HelperButtonName(p_group, p_param, 0));
      }
      connect (action,SIGNAL(trigger(const QString &)),this,
               SIGNAL(HelperTrigger(const QString &)));

      // Set up helper button
      QToolButton *helper = new QToolButton();
      helper->setText((iString)p_ui->HelperButtonName(p_group, p_param, 0));

      helper->setMenu(p_helperMenu);
      helper->setPopupMode(QToolButton::MenuButtonPopup);
      helper->setDefaultAction(action);
      helper->setToolTip((iString)p_ui->HelperBrief(p_group,p_param,0));
      QString text = "<p><b>Function:</b> " + 
        QString((iString)p_ui->HelperDescription(p_group,p_param,0)) + "</p>" +
        "<p><b>Hint: </b> Click on the arrow to see more helper functions</p>"; 
      helper->setWhatsThis(text);

      if (p_ui->HelperIcon(p_group,p_param,0) != "") {
        helper->setText("");
        iString file = Filename(
          p_ui->HelperIcon(p_group,p_param,0)).Expanded();
        helper->setIconSize(QSize(22,22));
        helper->setIcon(QIcon(QPixmap(file)));
      }
      else {
        helper->setFixedWidth(helper->fontMetrics().width(
          "  " + helper->text() + "  "));
      } 

      // Set up default action item in menu list
      GuiHelperAction *action2 = 
        new GuiHelperAction(lo,(iString)p_ui->HelperFunction(p_group,p_param,0));
      action2->setText((iString)p_ui->HelperBrief(p_group, p_param, 0));
      action2->setToolTip((iString)p_ui->HelperBrief(p_group,p_param,0));
      QString helperText = "<p><b>Function:</b> " + 
        QString((iString)p_ui->HelperDescription(p_group,p_param,0)) + "</p>"; 
      action2->setWhatsThis(helperText);
      connect (action2,SIGNAL(trigger(const QString &)),this,
               SIGNAL(HelperTrigger(const QString &)));
      p_helperMenu->addAction(action2);


      // Add each additional helper button to the menu list
      for (int i=1; i<p_ui->HelpersSize(p_group,p_param); i++) {
        GuiHelperAction *helperAction = 
          new GuiHelperAction(lo,(iString)p_ui->HelperFunction(p_group,p_param,i));
        helperAction->setText((iString)p_ui->HelperBrief(p_group, p_param, i));
        helperAction->setToolTip((iString)p_ui->HelperBrief(p_group,p_param,i));
        QString helperText = "<p><b>Function:</b> " + 
          QString((iString)p_ui->HelperDescription(p_group,p_param,i)) + "</p>"; 
        helperAction->setWhatsThis(helperText);
        connect (helperAction,SIGNAL(trigger(const QString &)),this,
                 SIGNAL(HelperTrigger(const QString &)));
        p_helperMenu->addAction(helperAction);
      } 
      RememberWidget(helper);
      return helper;
    }
  }
}

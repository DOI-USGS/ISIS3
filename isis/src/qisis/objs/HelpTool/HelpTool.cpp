#include <QWhatsThis>
#include <QMenu>
#include <QString>
#include <QApplication>

#include "HelpTool.h"
#include "MainWindow.h"
#include "Workspace.h"
#include "Application.h"
#include "Preference.h"

namespace Qisis {
  // Constructor
  HelpTool::HelpTool (QWidget *parent) : Qisis::Tool(parent) {
    p_whatsThis = new QAction(parent);
    p_whatsThis->setShortcut(Qt::SHIFT+Qt::Key_F1);
    p_whatsThis->setText("&What's This?");
    p_whatsThis->setIcon(QPixmap(toolIconDir()+"/contexthelp.png"));
    p_whatsThis->setToolTip("What's This");
    QString whatsThis =
      "<b>Function:</b> Use this to get longer descriptions of button \
      functions \
      <p><b>Shortcut:</b> Shift+F1</p>";
    p_whatsThis->setWhatsThis(whatsThis);
    connect(p_whatsThis,SIGNAL(activated()),this,SLOT(whatsThis()));

    p_aboutProgram = new QAction(parent);
    p_aboutProgram->setShortcut(Qt::CTRL+Qt::Key_H);
    QString s = "About ";
    s += QApplication::applicationName();
    p_aboutProgram->setText(s);
    connect(p_aboutProgram,SIGNAL(activated()),this,SLOT(aboutProgram()));

  }

  // Add actions to the Help menu
  void HelpTool::addTo (QMenu *menu) {
    menu->addAction(p_whatsThis);
    menu->addAction(p_aboutProgram);
  }

  //! Add actions to the permanent toolbar
  void HelpTool::addToPermanent (QToolBar *perm) {
    perm->addAction(p_whatsThis);
  }

  // The user pressed the whatsThis button so do something
  void HelpTool::whatsThis () {
    QWhatsThis::enterWhatsThisMode();
  }


  /**
   * Slot to print program information
   * 
   * @author  2007-06-12 Tracie Sucharski
   */
  void HelpTool::aboutProgram () {
    Isis::Filename file((std::string)
                        "$ISISROOT/doc/Application/presentation/PrinterFriendly/" +
                        QApplication::applicationName().toStdString() + "/" +
                        QApplication::applicationName().toStdString() + ".html");

    Isis::PvlGroup &uig = Isis::Preference::Preferences().FindGroup("UserInterface");
    std::string command = (std::string) uig["GuiHelpBrowser"] +
                          (std::string)" file:" + file.Expanded() + " &";
    system (command.c_str());
 
  }
}

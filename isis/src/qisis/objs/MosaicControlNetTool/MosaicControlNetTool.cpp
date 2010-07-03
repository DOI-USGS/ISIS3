#include "FileDialog.h"
#include "MosaicControlNetTool.h"

#include <QMenu>

namespace Qisis {

  /**
   * MosaicControlNetTool constructor
   * 
   * @param parent 
   */
  MosaicControlNetTool::MosaicControlNetTool (MosaicWidget *parent) : Qisis::MosaicTool(parent) {
    connect(this,SIGNAL(activated(bool)),this,SLOT(updateTool()));
    p_parent = parent;
//setParent(parent); //The way it SHOULD be done

    p_connectivity = new QAction(parent);
    p_connectivity->setShortcut(Qt::Key_I);
    p_connectivity->setText("Show Islands (I)");
    p_connectivity->setIcon(QPixmap(toolIconDir()+"/viewmag+.png"));
    connect(p_connectivity,SIGNAL(activated()),this,SLOT(displayConnectivity()));

    createDialog(parent);
  }


   /**
   * Creates the dialog used by this tool
   *
   * @param parent
   */
  void MosaicControlNetTool::createDialog(QWidget *parent) {

    p_dialog = new QDialog(parent);
    p_dialog->setWindowTitle("Control Net");


    // Put the buttons and text field in a gridlayout
    //QGridLayout *gridLayout = new QGridLayout ();
    /*gridLayout->addWidget(latLabel,0,0);
    gridLayout->addWidget(p_latLineEdit,0,1);
    gridLayout->addWidget(lonLabel,1,0);
    gridLayout->addWidget(p_lonLineEdit, 1, 1);*/

    // Create the action buttons
    p_loadControlNetButton = new QPushButton ();
    p_loadControlNetButton->setIcon(QPixmap((QString::fromStdString(Isis::Filename("$base/icons").Expanded().c_str()) + "/HILLBLU_molecola.png")));
    connect(p_loadControlNetButton, SIGNAL(clicked()), this, SLOT(loadControlNet()));

    p_displayControlNetButton = new QPushButton ("Display");
    p_displayControlNetButton->setCheckable(true);
    connect(p_displayControlNetButton, SIGNAL(clicked()), this, SLOT(displayControlNet()));

    p_displayConnectivity = new QPushButton ("Connectivity");
    p_displayConnectivity->setCheckable(true);
    connect(p_displayConnectivity, SIGNAL(clicked()), this, SLOT(displayConnectivity()));

    QPushButton* cancelButton = new QPushButton ("Done");
    connect(cancelButton, SIGNAL(clicked()), this, SLOT(hideTool()));

    // Make |X| button close properly
    connect(p_dialog, SIGNAL(finished(int)), this, SLOT(hideTool()));

    // Put the buttons in a horizontal orientation
    QHBoxLayout *actionLayout = new QHBoxLayout();
    actionLayout->addWidget(p_loadControlNetButton);
    actionLayout->addWidget(p_displayControlNetButton);
    actionLayout->addWidget(p_displayConnectivity);
    actionLayout->addWidget(cancelButton);

    QVBoxLayout *dialogLayout = new QVBoxLayout;
    //dialogLayout->addLayout(gridLayout);
    dialogLayout->addLayout(actionLayout);
    p_dialog->setLayout(dialogLayout);
  }


  /**
   * Adds the control net action to the given menu.
   * 
   * @param menu 
   */
  void MosaicControlNetTool::addToMenu(QMenu *menu) {
    menu->addAction(p_connectivity);
  }


  /**
   * Adds the action to the toolpad.
   * 
   * @param toolpad 
   * 
   * @return QAction* 
   */
  QAction *MosaicControlNetTool::toolPadAction(ToolPad *toolpad) {
    p_action = new QAction(toolpad);
    //p_action->setIcon(QPixmap(toolIconDir()+"/find.png"));
    p_action->setIcon(QPixmap((QString::fromStdString(Isis::Filename("$base/icons").Expanded().c_str()) + "/HILLBLU_molecola.png")));
    p_action->setToolTip("Control Net (C)");
    p_action->setShortcut(Qt::Key_C);
    QString text  =
      "<b>Function:</b>  Utilize a Control Network \
      <p><b>Shortcut:</b>  C</p> ";
    p_action->setWhatsThis(text);
    return p_action;
  }


  /**
   * Creates the widget to add to the tool bar.
   * 
   * @param parent 
   * 
   * @return QWidget* 
   */
  QWidget *MosaicControlNetTool::createToolBarWidget (QStackedWidget *parent) {
    QWidget *hbox = new QWidget(parent);
    return hbox;
  }


  /** 
   * This slot opens and reopens this tool properly
   */
  void MosaicControlNetTool::updateTool() {
    if(isActive()) {
      p_dialog->show();

      p_displayControlNetButton->setCheckable(true);
      if(p_parent->controlNet() != NULL) {
        p_displayControlNetButton->setChecked(true);
        displayControlNet();
      }

      p_displayConnectivity->setCheckable(true);
      p_displayConnectivity->setChecked(false);

      p_action->setChecked(false);
    }
  }


  /**
   * This slot closes this tool properly
   */
  void MosaicControlNetTool::hideTool() {
    // Hide the control net
    if( p_displayControlNetButton->isChecked() ) {
      p_displayControlNetButton->setCheckable(false);
      displayControlNet();
    }

    // Revert to individual colors
    if( p_displayConnectivity->isChecked() ) {
      p_displayConnectivity->setCheckable(false);
      displayConnectivity();
    }

    p_dialog->hide();
    //p_action->setChecked(false);
  }


  /**
   * Displays the connectivity of Control Points
   * 
   */
  void MosaicControlNetTool::displayConnectivity(){
    p_parent->displayConnectivity(p_displayConnectivity->isChecked());
  }


  /**
   * Loads a control net from a file
   * 
   */
  void MosaicControlNetTool::loadControlNet() {

    // Bring up a file dialog for user to select their cnet file.
    QString netFile = FileDialog::getOpenFileName(p_parent, 
                                                  "Select Control Net. File", 
                                                  QDir::current().dirName(), 
                                                  "*.net");

    //--------------------------------------------------------------
    // if the file is not empty attempt to load in the control points
    // for each mosaic item
    //---------------------------------------------------------------
    if(!netFile.isEmpty()) {

      try {
        Isis::Filename controlNetFile(netFile.toStdString());
        p_parent->setControlNet( controlNetFile );
      }
      catch (Isis::iException &e) {
        QString message = "Invalid control network.  \n";
        std::string errors = e.Errors();
        message += errors.c_str();
        e.Clear ();
        QMessageBox::information(p_parent,"Error",message);
        QApplication::restoreOverrideCursor();
        p_loadControlNetButton->setChecked(false);
        return;
      }

      //--------------------------------------------------------------
      // for each mosaic item, display its control points
      //--------------------------------------------------------------
      QList<MosaicItem *> mosaicItems = p_parent->mosaicItems();
      for(int i = 0; i < mosaicItems.size(); i++) {
        mosaicItems[i]->displayControlPoints(p_parent->controlNet());
      }

      p_displayControlNetButton->setChecked(true);
       
    } else {
      //---------------------------------------------------
      // this means the user canceled out of the dialog box
      //---------------------------------------------------
      p_loadControlNetButton->setChecked(false);
    }

  }


  void MosaicControlNetTool::displayControlNet() {
    if(p_parent->controlNet() != NULL) {
      QList<MosaicItem *> mosaicItems = p_parent->mosaicItems();

      for(int i = 0; i < mosaicItems.size(); i++) {
        mosaicItems[i]->setControlPointsVisible(p_displayControlNetButton->isChecked());
      }
    }
  }



}

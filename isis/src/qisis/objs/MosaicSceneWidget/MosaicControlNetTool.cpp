#include "MosaicControlNetTool.h"

#include <iostream>

#include <QMenu>

#include "ControlNet.h"
#include "ControlNetGraphicsItem.h"
#include "CubeDisplayProperties.h"
#include "FileDialog.h"
#include "iException.h"
#include "iString.h"
#include "MosaicSceneWidget.h"
#include "PvlObject.h"

namespace Isis {

  /**
   * MosaicControlNetTool constructor
   *
   * @param parent
   */
  MosaicControlNetTool::MosaicControlNetTool(MosaicSceneWidget *scene) :
      MosaicTool(scene) {
    p_controlNet = NULL;
    p_controlNetGraphics = NULL;
    p_loadControlNetButton = NULL;
    p_displayControlNetButton = NULL;
    p_displayConnectivity = NULL;
    p_closeNetwork = NULL;

    // Create the action buttons
    p_loadControlNetButton = new QPushButton();
    p_loadControlNetButton->setIcon(getIcon("HILLBLU_molecola.png"));
    connect(p_loadControlNetButton, SIGNAL(clicked()), this, SLOT(openControlNet()));
    connect(p_loadControlNetButton, SIGNAL(destroyed(QObject *)),
            this, SLOT(objectDestroyed(QObject *)));

    p_displayControlNetButton = new QPushButton("Display");
    p_displayControlNetButton->setCheckable(true);
    p_displayControlNetButton->setEnabled(false);
    connect(p_displayControlNetButton, SIGNAL(clicked()), this, SLOT(displayControlNet()));
    connect(p_displayControlNetButton, SIGNAL(destroyed(QObject *)),
            this, SLOT(objectDestroyed(QObject *)));

    p_displayConnectivity = new QPushButton("Color Connectivity");
    connect(p_displayConnectivity, SIGNAL(clicked()), this, SLOT(displayConnectivity()));
    connect(p_displayConnectivity, SIGNAL(destroyed(QObject *)),
            this, SLOT(objectDestroyed(QObject *)));
    p_displayConnectivity->setEnabled(false);

    p_closeNetwork = new QPushButton("Close Network");
    p_closeNetwork->setEnabled(false);
    connect(p_closeNetwork, SIGNAL(clicked()), this, SLOT(closeNetwork()));
    connect(p_closeNetwork, SIGNAL(destroyed(QObject *)),
            this, SLOT(objectDestroyed(QObject *)));

    p_connectivity = new QAction(this);
    p_connectivity->setShortcut(Qt::Key_I);
    p_connectivity->setText("Show Islands (I)");
    p_connectivity->setIcon(getIcon("viewmag+.png"));
    connect(p_connectivity, SIGNAL(activated()), this, SLOT(displayConnectivity()));
    connect(p_connectivity, SIGNAL(destroyed(QObject *)),
            this, SLOT(objectDestroyed(QObject *)));
  }


  MosaicControlNetTool::~MosaicControlNetTool() {
    p_controlNetGraphics = NULL; // the scene cleans/cleaned this up

    if(p_loadControlNetButton)
      delete p_loadControlNetButton;

    if(p_displayControlNetButton)
      delete p_displayControlNetButton;

    if(p_displayConnectivity)
      delete p_displayConnectivity;

    if(p_closeNetwork)
      delete p_closeNetwork;

    if(p_connectivity)
      delete p_connectivity;

    closeNetwork();
  }


  CubeDisplayProperties *MosaicControlNetTool::takeDisplay(
      QString sn, QList<CubeDisplayProperties *> &displays) {
    if(p_controlNet && p_controlNetGraphics) {
      QString filename = p_controlNetGraphics->snToFilename(sn);

      for(int i = 0; i < displays.size(); i++) {
        CubeDisplayProperties *display = displays[i];

        if(display->fileName() == filename) {
          return displays.takeAt(i);
        }
      }
    }

    return NULL;
  }


  /**
   * Adds the control net action to the given menu.
   *
   * @param menu
   */
  void MosaicControlNetTool::addToMenu(QMenu *menu) {
    menu->addAction(p_connectivity);
  }


  PvlObject MosaicControlNetTool::toPvl() const {
    PvlObject obj(projectPvlObjectName());

    obj += PvlKeyword("Filename", p_controlNetFile);
    obj += PvlKeyword("Visible",
        p_controlNetGraphics && p_controlNetGraphics->isVisible());

    return obj;
  }


  void MosaicControlNetTool::fromPvl(PvlObject &obj) {
    p_controlNetFile = QString::fromStdString(obj["Filename"][0]);
    if(p_controlNetFile == "Null")
      p_controlNetFile = "";

    loadNetwork();

    if(p_controlNetGraphics && p_displayControlNetButton) {
      p_displayControlNetButton->setChecked( (int)obj["Visible"][0] );
      displayControlNet();
    }
  }


  iString MosaicControlNetTool::projectPvlObjectName() const {
    return "MosaicControlNetTool";
  }


  /**
   * Adds the action to the toolpad.
   *
   * @param toolpad
   *
   * @return QAction*
   */
  QAction *MosaicControlNetTool::getPrimaryAction() {
    QAction *action = new QAction(this);
    action->setIcon(getIcon("HILLBLU_molecola.png"));
    action->setToolTip("Control Net (C)");
    action->setShortcut(Qt::Key_C);
    QString text  =
      "<b>Function:</b>  Utilize a Control Network \
      <p><b>Shortcut:</b>  C</p> ";
    action->setWhatsThis(text);
    return action;
  }


  QWidget *MosaicControlNetTool::getToolBarWidget() {
    // Put the buttons in a horizontal orientation
    QHBoxLayout *actionLayout = new QHBoxLayout();
    if(p_loadControlNetButton)
      actionLayout->addWidget(p_loadControlNetButton);

    if(p_displayControlNetButton)
      actionLayout->addWidget(p_displayControlNetButton);

    if(p_displayConnectivity)
      actionLayout->addWidget(p_displayConnectivity);

    if(p_closeNetwork)
      actionLayout->addWidget(p_closeNetwork);

    actionLayout->setMargin(0);

    QWidget *toolBarWidget = new QWidget;
    toolBarWidget->setLayout(actionLayout);

    return toolBarWidget;
  }


  /**
   * This slot opens and reopens this tool properly
   */
  void MosaicControlNetTool::updateTool() {
    if(isActive() && p_controlNetFile == "")
      openControlNet();
  }


  /**
   * Displays the connectivity of Control Points
   *
   */
  void MosaicControlNetTool::displayConnectivity() {
    if(p_controlNet) {
      QList<CubeDisplayProperties *> displays = getWidget()->cubeDisplays();

      QList<QColor> colorsUsed;

      QList< QList<QString> > serialConns =
          p_controlNet->GetSerialConnections();

      QList<QString> island;
      foreach(island, serialConns) {
        QColor color;

        QString cubeSn;
        foreach(cubeSn, island) {
          CubeDisplayProperties *display = takeDisplay(cubeSn, displays);

          while(!color.isValid()) {
            QColor displayColor =
                display->getValue(CubeDisplayProperties::Color).value<QColor>();

            if(colorsUsed.indexOf(displayColor) == -1) {
              colorsUsed.append(displayColor);
              color = displayColor;
            }
            else {
              QColor ranColor = CubeDisplayProperties::randomColor();

              if(colorsUsed.indexOf(ranColor) == -1) {
                colorsUsed.append(ranColor);
                color = ranColor;
              }
            }
          }

          display->setColor(color);
        }
      }
    }
  }


  void MosaicControlNetTool::closeNetwork() {
    if(p_controlNetGraphics) {
      getWidget()->getScene()->removeItem(p_controlNetGraphics);

      delete p_controlNetGraphics;
      p_controlNetGraphics = NULL;
    }

    if(p_controlNet) {
      delete p_controlNet;
      p_controlNet = NULL;
    }

    if(p_displayControlNetButton)
      p_displayControlNetButton->setChecked(false);

    if(p_loadControlNetButton)
      p_loadControlNetButton->setEnabled(true);

    if(p_displayControlNetButton)
      p_displayControlNetButton->setEnabled(false);

    if(p_displayConnectivity)
      p_displayConnectivity->setEnabled(false);

    if(p_closeNetwork)
      p_closeNetwork->setEnabled(false);

    p_controlNetFile = "";
  }


  void MosaicControlNetTool::objectDestroyed(QObject *obj) {
    if(obj == p_loadControlNetButton)
      p_loadControlNetButton = NULL;
    else if(obj == p_displayControlNetButton)
      p_displayControlNetButton = NULL;
    else if(obj == p_displayConnectivity)
      p_displayConnectivity = NULL;
    else if(obj == p_closeNetwork)
      p_closeNetwork = NULL;
    else if(obj == p_connectivity)
      p_connectivity = NULL;
    else if(obj == p_controlNetGraphics)
      p_controlNetGraphics = NULL;
  }


  /**
   * Loads a control net from a file
   *
   */
  void MosaicControlNetTool::openControlNet() {

    // Bring up a file dialog for user to select their cnet file.
    QString netFile = Qisis::FileDialog::getOpenFileName(getWidget(),
                      "Select Control Net. File",
                      QDir::current().dirName(),
                      "Control Networks (*.net);;All Files (*.*)");

    //--------------------------------------------------------------
    // if the file is not empty attempt to load in the control points
    // for each mosaic item
    //---------------------------------------------------------------
    if(!netFile.isEmpty()) {
      Filename controlNetFile(netFile.toStdString());
      p_controlNetFile = QString::fromStdString(controlNetFile.Expanded());
      loadNetwork();
    }
    else {
      //---------------------------------------------------
      // this means the user canceled out of the dialog box
      //---------------------------------------------------
      p_loadControlNetButton->setChecked(false);
    }
  }


  void MosaicControlNetTool::loadNetwork() {
    QString netFile = p_controlNetFile;
    closeNetwork();
    p_controlNetFile = netFile;

    if(p_controlNetFile.size() > 0) {
      try {
        p_controlNet = new ControlNet(p_controlNetFile.toStdString());
        p_controlNetGraphics = new ControlNetGraphicsItem(p_controlNet,
            getWidget());
        connect(p_controlNetGraphics, SIGNAL(destroyed(QObject *)),
                this, SLOT(objectDestroyed(QObject *)));
      }
      catch(iException &e) {
        QString message = "Invalid control network.  \n";
        std::string errors = e.Errors();
        message += errors.c_str();
        e.Clear();
        QMessageBox::information(getWidget(), "Error", message);
        p_loadControlNetButton->setChecked(false);
        return;
      }

      if(p_displayControlNetButton)
        p_displayControlNetButton->setChecked(true);

      if(p_loadControlNetButton)
        p_loadControlNetButton->setEnabled(false);

      if(p_displayControlNetButton)
        p_displayControlNetButton->setEnabled(true);

      if(p_displayConnectivity)
        p_displayConnectivity->setEnabled(true);

      if(p_closeNetwork)
        p_closeNetwork->setEnabled(true);
    }
  }


  void MosaicControlNetTool::displayControlNet() {
    if(p_controlNetGraphics && p_displayControlNetButton)
      p_controlNetGraphics->setVisible(p_displayControlNetButton->isChecked());
  }
}


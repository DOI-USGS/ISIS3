#include "MosaicControlNetTool.h"

#include <iostream>

#include <QMenu>

#include "ControlNet.h"
#include "ControlNetGraphicsItem.h"
#include "CubeDisplayProperties.h"
#include "FileDialog.h"
#include "IException.h"
#include "IString.h"
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
    m_controlNet = NULL;
    m_controlNetGraphics = NULL;
    m_loadControlNetButton = NULL;
    m_displayControlNetButton = NULL;
    m_displayConnectivity = NULL;
    m_closeNetwork = NULL;
    m_controlNetFileLabel = NULL;
    m_randomizeColors = NULL;

    // Create the action buttons

    m_displayControlNetButton = new QPushButton("Display");
    m_displayControlNetButton->setCheckable(true);
    m_displayControlNetButton->setEnabled(false);
    m_displayControlNetButton->setToolTip("Toggle the visibility of the "
        "control points and movement arrows in the network.");
    connect(m_displayControlNetButton, SIGNAL(clicked()), this, SLOT(displayControlNet()));
    connect(m_displayControlNetButton, SIGNAL(destroyed(QObject *)),
            this, SLOT(objectDestroyed(QObject *)));

    m_displayConnectivity = new QPushButton("Color Islands");
    m_displayConnectivity->setToolTip("Color cubes the same if the control "
        "network has a connection between them");
    connect(m_displayConnectivity, SIGNAL(clicked()), this, SLOT(displayConnectivity()));
    connect(m_displayConnectivity, SIGNAL(destroyed(QObject *)),
            this, SLOT(objectDestroyed(QObject *)));
    m_displayConnectivity->setEnabled(false);

    m_randomizeColors = new QPushButton("Color Images");
    m_randomizeColors->setToolTip("Color all cubes differently");
    connect(m_randomizeColors, SIGNAL(clicked()), this, SLOT(randomizeColors()));
    connect(m_randomizeColors, SIGNAL(destroyed(QObject *)),
            this, SLOT(objectDestroyed(QObject *)));

    m_displayArrows = new QPushButton("Show Movement");
    m_displayArrows->setCheckable(true);
    m_displayArrows->setChecked(false);
    m_displayArrows->setToolTip("Show arrow from the apriori surface point to "
        "the adjusted surface point for each control point with this "
        "information");
    connect(m_displayArrows, SIGNAL(clicked()), this, SLOT(displayArrows()));
    connect(m_displayArrows, SIGNAL(destroyed(QObject *)),
            this, SLOT(objectDestroyed(QObject *)));
    m_displayArrows->setEnabled(false);

    m_closeNetwork = new QPushButton("Close Network");
    m_closeNetwork->setEnabled(false);
    m_closeNetwork->setVisible(false);
    m_closeNetwork->setToolTip("Close the currently open control network");
    connect(m_closeNetwork, SIGNAL(clicked()), this, SLOT(closeNetwork()));
    connect(m_closeNetwork, SIGNAL(destroyed(QObject *)),
            this, SLOT(objectDestroyed(QObject *)));

    m_loadControlNetButton = new QPushButton("Open Network");
    m_loadControlNetButton->setToolTip("Open and load a control network");
    connect(m_loadControlNetButton, SIGNAL(clicked()),
            this, SLOT(openControlNet()));
    connect(m_loadControlNetButton, SIGNAL(destroyed(QObject *)),
            this, SLOT(objectDestroyed(QObject *)));

    m_controlNetFileLabel = new QLabel;
    m_controlNetFileLabel->setToolTip("The filename of the currently open "
        "control network");
    connect(m_controlNetFileLabel, SIGNAL(destroyed(QObject *)),
            this, SLOT(objectDestroyed(QObject *)));
  }


  MosaicControlNetTool::~MosaicControlNetTool() {
    m_controlNetGraphics = NULL; // the scene cleans/cleaned this up

    delete m_loadControlNetButton;
    delete m_displayControlNetButton;
    delete m_displayConnectivity;
    delete m_displayArrows;
    delete m_closeNetwork;
    delete m_controlNetFileLabel;
    delete m_randomizeColors;

    closeNetwork();
  }


  CubeDisplayProperties *MosaicControlNetTool::takeDisplay(
      QString sn, QList<CubeDisplayProperties *> &displays) {
    if (m_controlNet && m_controlNetGraphics) {
      QString filename = m_controlNetGraphics->snToFileName(sn);

      for(int i = 0; i < displays.size(); i++) {
        CubeDisplayProperties *display = displays[i];

        if (display->fileName() == filename) {
          return displays.takeAt(i);
        }
      }
    }

    return NULL;
  }


  PvlObject MosaicControlNetTool::toPvl() const {
    PvlObject obj(projectPvlObjectName());

    obj += PvlKeyword("FileName", m_controlNetFile);
    obj += PvlKeyword("Visible",
        m_controlNetGraphics && m_controlNetGraphics->isVisible());

    return obj;
  }


  void MosaicControlNetTool::fromPvl(const PvlObject &obj) {
    m_controlNetFile = QString::fromStdString(obj["FileName"][0]);
    if (m_controlNetFile == "Null")
      m_controlNetFile = "";

    loadNetwork();

    if (m_controlNetGraphics && m_displayControlNetButton) {
      m_displayControlNetButton->setChecked( (int)obj["Visible"][0] );
      displayControlNet();
    }
  }


  IString MosaicControlNetTool::projectPvlObjectName() const {
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
    action->setToolTip("Control Net (c)");
    action->setShortcut(Qt::Key_C);
    QString text  =
      "<b>Function:</b>  Display and analyze a control network<br><br>"
      "This tool shows you all of the control points in your network for "
      "which a latitude/longitude can be calculated. The control points are "
      "shown as color-coded '+' marks. The control points have a right-click "
      "menu and information about them can be seen just by hovering over them."
      "<p><b>Shortcut:</b>  c</p> ";
    action->setWhatsThis(text);
    return action;
  }


  QWidget *MosaicControlNetTool::getToolBarWidget() {
    // Put the buttons in a horizontal orientation
    QHBoxLayout *actionLayout = new QHBoxLayout();

    if (m_displayControlNetButton)
      actionLayout->addWidget(m_displayControlNetButton);

    if (m_displayConnectivity)
      actionLayout->addWidget(m_displayConnectivity);

    if (m_randomizeColors)
      actionLayout->addWidget(m_randomizeColors);

    if (m_displayArrows)
      actionLayout->addWidget(m_displayArrows);

    if (m_closeNetwork)
      actionLayout->addWidget(m_closeNetwork);

    if (m_loadControlNetButton)
      actionLayout->addWidget(m_loadControlNetButton);

    if (m_controlNetFileLabel)
      actionLayout->addWidget(m_controlNetFileLabel);

    actionLayout->setMargin(0);

    QWidget *toolBarWidget = new QWidget;
    toolBarWidget->setLayout(actionLayout);

    return toolBarWidget;
  }


  /**
   * This slot opens and reopens this tool properly
   */
  void MosaicControlNetTool::updateTool() {
    if (isActive() && m_controlNetFile == "") {
      openControlNet();
    }
  }


  /**
   * The user toggled the cnet visibility - re-sync the graphics item visibility
   *   with the action.
   */
  void MosaicControlNetTool::displayArrows() {
    if (m_controlNetGraphics && m_displayArrows)
      m_controlNetGraphics->setArrowsVisible(m_displayArrows->isChecked());
  }


  /**
   * Displays the connectivity of Control Points
   *
   */
  void MosaicControlNetTool::displayConnectivity() {
    if (m_controlNet) {
      QList<CubeDisplayProperties *> displays = getWidget()->cubeDisplays();

      QList<QColor> colorsUsed;

      QList< QList<QString> > serialConns =
          m_controlNet->GetSerialConnections();

      QList<QString> island;
      foreach(island, serialConns) {
        QColor color;

        QString cubeSn;
        foreach(cubeSn, island) {
          CubeDisplayProperties *display = takeDisplay(cubeSn, displays);

          if (display) {
            while(!color.isValid()) {
              QColor displayColor = display->getValue(
                  CubeDisplayProperties::Color).value<QColor>();

              if (colorsUsed.indexOf(displayColor) == -1) {
                colorsUsed.append(displayColor);
                color = displayColor;
              }
              else {
                QColor ranColor = CubeDisplayProperties::randomColor();

                if (colorsUsed.indexOf(ranColor) == -1) {
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
  }


  /**
   * Close the open network, if one is open. m_controlNetFile is set to an
   *   empty string.
   */
  void MosaicControlNetTool::closeNetwork() {
    if (m_controlNetGraphics) {
      getWidget()->getScene()->removeItem(m_controlNetGraphics);

      delete m_controlNetGraphics;
    }

    if (m_controlNet) {
      delete m_controlNet;
      m_controlNet = NULL;
    }

    if (m_displayControlNetButton)
      m_displayControlNetButton->setChecked(false);

    if (m_displayControlNetButton)
      m_displayControlNetButton->setEnabled(false);

    if (m_displayConnectivity)
      m_displayConnectivity->setEnabled(false);

    if (m_displayArrows)
      m_displayArrows->setChecked(false);

    if (m_displayArrows)
      m_displayArrows->setEnabled(false);

    if (m_closeNetwork) {
      m_closeNetwork->setEnabled(false);
      m_closeNetwork->setVisible(false);
    }

    if (m_loadControlNetButton) {
      m_loadControlNetButton->setEnabled(true);
      m_loadControlNetButton->setVisible(true);
    }

    if (m_controlNetFileLabel)
      m_controlNetFileLabel->setText("");

    m_controlNetFile = "";
  }


  /**
   * An object was destroyed, NULL it out.
   */
  void MosaicControlNetTool::objectDestroyed(QObject *obj) {
    if (obj == m_loadControlNetButton)
      m_loadControlNetButton = NULL;
    else if (obj == m_displayControlNetButton)
      m_displayControlNetButton = NULL;
    else if (obj == m_displayConnectivity)
      m_displayConnectivity = NULL;
    else if (obj == m_closeNetwork)
      m_closeNetwork = NULL;
    else if (obj == m_controlNetGraphics)
      m_controlNetGraphics = NULL;
    else if (obj == m_displayArrows)
      m_displayArrows = NULL;
    else if (obj == m_controlNetFileLabel)
      m_controlNetFileLabel = NULL;
    else if (obj == m_randomizeColors)
      m_randomizeColors = NULL;
  }


  /**
   * Loads a control net from a file
   *
   */
  void MosaicControlNetTool::openControlNet() {

    // Bring up a file dialog for user to select their cnet file.
    QString netFile = FileDialog::getOpenFileName(getWidget(),
                      "Select Control Net. File",
                      QDir::current().dirName() + "/",
                      "Control Networks (*.net);;All Files (*.*)");

    //--------------------------------------------------------------
    // if the file is not empty attempt to load in the control points
    // for each mosaic item
    //---------------------------------------------------------------
    if (!netFile.isEmpty()) {
      FileName controlNetFile(netFile.toStdString());
      m_controlNetFile = QString::fromStdString(controlNetFile.expanded());
      loadNetwork();

      if (m_displayControlNetButton)
        m_displayControlNetButton->setChecked(true);
    }
  }


  /**
   * Load m_controlNetFile into memory - this will re-load the network if it's
   *   already open.
   */
  void MosaicControlNetTool::loadNetwork() {
    QString netFile = m_controlNetFile;
    closeNetwork();
    m_controlNetFile = netFile;
    m_controlNetFileLabel->setText( QFileInfo(netFile).fileName() );

    if (m_controlNetFile.size() > 0) {
      try {
        m_controlNet = new ControlNet(
            m_controlNetFile.toStdString());
        m_controlNetGraphics = new ControlNetGraphicsItem(m_controlNet,
            getWidget());
        connect(m_controlNetGraphics, SIGNAL(destroyed(QObject *)),
                this, SLOT(objectDestroyed(QObject *)));
      }
      catch(IException &e) {
        QString message = "Invalid control network.\n";
        message += e.toString().ToQt();
        QMessageBox::information(getWidget(), "Error", message);
        return;
      }

      if (m_displayControlNetButton)
        m_displayControlNetButton->setEnabled(true);

      if (m_displayConnectivity)
        m_displayConnectivity->setEnabled(true);

      if (m_displayArrows)
        m_displayArrows->setEnabled(true);

      if (m_closeNetwork) {
        m_closeNetwork->setEnabled(true);
        m_closeNetwork->setVisible(true);
      }

      if (m_loadControlNetButton) {
        m_loadControlNetButton->setEnabled(false);
        m_loadControlNetButton->setVisible(false);
      }
    }
  }


  void MosaicControlNetTool::randomizeColors() {
    foreach (CubeDisplayProperties * display, getWidget()->cubeDisplays()) {
      display->setColor(CubeDisplayProperties::randomColor());
    }
  }


  /**
   * The user toggled the cnet visibility - re-sync the graphics item visibility
   *   with the action.
   */
  void MosaicControlNetTool::displayControlNet() {
    if (m_controlNetGraphics && m_displayControlNetButton)
      m_controlNetGraphics->setVisible(m_displayControlNetButton->isChecked());
  }
}

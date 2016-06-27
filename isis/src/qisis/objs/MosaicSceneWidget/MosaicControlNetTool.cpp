#include "MosaicControlNetTool.h"

#include <iostream>

#include <QMenu>

#include "Control.h"
#include "ControlList.h"
#include "ControlNet.h"
#include "ControlNetGraphicsItem.h"
#include "Directory.h"
#include "FileDialog.h"
#include "FileName.h"
#include "IException.h"
#include "Image.h"
#include "ImageDisplayProperties.h"
#include "ImageList.h"
#include "IString.h"
#include "MosaicControlNetToolMovementConfigDialog.h"
#include "MosaicSceneWidget.h"
#include "Project.h"
#include "PvlObject.h"
#include "SpecialPixel.h"

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

    m_movementArrowColorSource = NoMovement;
    m_measureCount = 10;
    m_residualMagnitude = 5.0;

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

    m_configMovement = new QPushButton(tr("Configure Movement Display"));
    m_configMovement->setToolTip(tr("Show arrow from the apriori surface point to "
        "the adjusted surface point for each control point with this "
        "information"));
    connect(m_configMovement, SIGNAL(clicked()), this, SLOT(configMovement()));
    connect(m_configMovement, SIGNAL(destroyed(QObject *)),
            this, SLOT(objectDestroyed(QObject *)));

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
    delete m_configMovement;
    delete m_closeNetwork;
    delete m_controlNetFileLabel;
    delete m_randomizeColors;

    closeNetwork();
  }


  Image *MosaicControlNetTool::takeImage(
      QString sn, ImageList &images) {
    if (m_controlNet && m_controlNetGraphics) {
      QString filename = m_controlNetGraphics->snToFileName(sn);

      for(int i = 0; i < images.size(); i++) {
        Image *image = images[i];

        if (image->fileName() == filename) {
          return images.takeAt(i);
        }
      }
    }

    return NULL;
  }


  PvlObject MosaicControlNetTool::toPvl() const {
    PvlObject obj(projectPvlObjectName());

    obj += PvlKeyword("FileName", m_controlNetFile);
    obj += PvlKeyword("Visible",
        Isis::toString((int)(m_controlNetGraphics && m_controlNetGraphics->isVisible())));
    obj += PvlKeyword("Movement", toString(m_movementArrowColorSource));

    if (maxMovementColorMeasureCount() != -1) {
      obj += PvlKeyword("MovementColorMaxMeasureCount", Isis::toString(m_measureCount));
    }

    if (maxMovementColorResidualMagnitude() != Null) {
      obj += PvlKeyword("MovementColorMaxResidualMagnitude",
                        Isis::toString(m_residualMagnitude));
    }

    return obj;
  }


  void MosaicControlNetTool::fromPvl(const PvlObject &obj) {
    m_controlNetFile = obj["FileName"][0];
    if (m_controlNetFile == "Null")
      m_controlNetFile = "";

    if (obj.hasKeyword("Movement")) {
      m_movementArrowColorSource = fromMovementColorSourceString(obj["Movement"]);
    }

    if (obj.hasKeyword("MovementColorMaxMeasureCount")) {
      m_measureCount = toInt(obj["MovementColorMaxMeasureCount"][0]);
    }

    if (obj.hasKeyword("MovementColorMaxResidualMagnitude")) {
      m_residualMagnitude = toDouble(obj["MovementColorMaxResidualMagnitude"][0]);
    }

    loadNetwork();

    if (m_controlNetGraphics && m_displayControlNetButton) {
      m_displayControlNetButton->setChecked( toBool(obj["Visible"][0]) );
      displayControlNet();
    }
  }


  QString MosaicControlNetTool::projectPvlObjectName() const {
    return "MosaicControlNetTool";
  }


  /**
   * Define how the movement arrows should be drawn. This includes if movement arrows should be
   *   drawn, what criteria should be used, and how to stretch (where to transition colors) the
   *   arrows.
   *
   * NOTE: This is just a quick implementation and is probably not designed correctly. This is
   *       subject to change.
   *
   * @param colorSource If and how to draw and color the arrows
   * @param maxMeasureCount The measure count to become fully colored
   * @param maxResidualMagnitude The max residual magnitude to become fully colored
   */
  void MosaicControlNetTool::setMovementArrowColorSource(MovementColorSource colorSource,
      int maxMeasureCount, double maxResidualMagnitude) {
    m_movementArrowColorSource = colorSource;
    m_measureCount = maxMeasureCount;
    m_residualMagnitude = maxResidualMagnitude;

    if (m_controlNetGraphics) {
      m_controlNetGraphics->setArrowsVisible(m_movementArrowColorSource != NoMovement,
          m_movementArrowColorSource == MeasureCount, m_measureCount,
          m_movementArrowColorSource == ResidualMagnitude, m_residualMagnitude);
    }
  }


  /**
   * Get the current setting for the movement arrows.
   */
  MosaicControlNetTool::MovementColorSource MosaicControlNetTool::movementArrowColorSource() const {
    return m_movementArrowColorSource;
  }


  /**
   * Get the current measure count to become fully colored. This will return -1 if it's undefined.
   */
  int MosaicControlNetTool::maxMovementColorMeasureCount() const {
    int result = -1;

    if (m_measureCount > 0)
      result = m_measureCount;

    return result;
  }


  /**
   * Get the current max. residual magnitude to become fully colored. This will return Null if it's
   *   undefined.
   */
  double MosaicControlNetTool::maxMovementColorResidualMagnitude() const {
    double result = Null;

    if (!IsSpecial(m_residualMagnitude))
      result = m_residualMagnitude;

    return result;
  }


  /**
   * Convert a MovementColorSource to a string for serialization purposes.
   */
  QString MosaicControlNetTool::toString(MovementColorSource source) {
    QString result;

    switch (source) {
      case NoMovement:
        result = "No movement arrows";
        break;

      case NoColor:
        result = "Black movement arrows";
        break;

      case MeasureCount:
        result = "Movement arrows colored by measure count";
        break;

      case ResidualMagnitude:
        result = "Movement arrows colored by residual magnitude";
        break;
    }

    return result;
  }


  /**
   * Convert a string back to a MovementColorSource (for serialization purposes).
   */
  MosaicControlNetTool::MovementColorSource MosaicControlNetTool::fromMovementColorSourceString(
      QString string) {
    MovementColorSource result = NoMovement;

    for (int i = 0; i < NUM_MOVEMENT_COLOR_SOURCE_VALUES; i++) {
      if (string.toLower() == toString((MovementColorSource)i).toLower()) {
        result = (MovementColorSource)i;
      }
    }

    return result;
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

    if (m_configMovement)
      actionLayout->addWidget(m_configMovement);

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
   * Bring up a movement arrow configuration dialog
   */
  void MosaicControlNetTool::configMovement() {
    MosaicControlNetToolMovementConfigDialog *configDialog =
        new MosaicControlNetToolMovementConfigDialog(this, qobject_cast<QWidget *>(parent()));
    configDialog->setAttribute(Qt::WA_DeleteOnClose);
    configDialog->show();
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
   * Displays the connectivity of Control Points
   *
   */
  void MosaicControlNetTool::displayConnectivity() {
    if (m_controlNet) {
      ImageList images = getWidget()->images();

      QList<QColor> colorsUsed;

      QList< QList<QString> > serialConns =
          m_controlNet->GetSerialConnections();

      QList<QString> island;
      foreach(island, serialConns) {
        QColor color;

        QString cubeSn;
        foreach(cubeSn, island) {
          Image *image = takeImage(cubeSn, images);

          if (image) {
            while(!color.isValid()) {
              QColor displayColor = image->displayProperties()->getValue(
                  ImageDisplayProperties::Color).value<QColor>();

              if (colorsUsed.indexOf(displayColor) == -1) {
                colorsUsed.append(displayColor);
                color = displayColor;
              }
              else {
                QColor ranColor = ImageDisplayProperties::randomColor();

                if (colorsUsed.indexOf(ranColor) == -1) {
                  colorsUsed.append(ranColor);
                  color = ranColor;
                }
              }
            }

            image->displayProperties()->setColor(color);
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
    else if (obj == m_configMovement)
      m_configMovement = NULL;
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
    /*  //tjw
    if (!getWidget()->directory()) {
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
        FileName controlNetFile(netFile);
        m_controlNetFile = controlNetFile.expanded();
        loadNetwork();

        if (m_displayControlNetButton)
          m_displayControlNetButton->setChecked(true);
      }
    }
    else {
      QList<ControlList *> controls = getWidget()->directory()->project()->controls();
      bool ok = false;
      if (controls.count() > 0) {
        QMap<Control *, QString> cnetChoices;
        foreach (ControlList *list, controls) {
          foreach (Control *control, *list) {
            cnetChoices[control] = tr("%1/%2").arg(list->name())
                .arg(control->displayProperties()->displayName());
          }
        }

        QStringList cnetNames = cnetChoices.values();
        qSort(cnetNames);

        QString selected = QInputDialog::getItem(NULL, tr("Select control network"),
                                                 tr("Select control network"), cnetNames, 0, false,
                                                 &ok);
        if (ok && !selected.isEmpty()) {
          m_controlNetFile = cnetChoices.key(selected)->fileName();
          loadNetwork();
          if (m_displayControlNetButton)
            m_displayControlNetButton->setChecked(true);
        }
      }
      else if (controls.count() == 1 && controls.at(0)->count() == 1) {
        m_controlNetFile = controls.first()->first()->fileName();
        loadNetwork();
        if (m_displayControlNetButton)
          m_displayControlNetButton->setChecked(true);
      }
    }
    */
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
            m_controlNetFile);
        m_controlNetGraphics = new ControlNetGraphicsItem(m_controlNet,
            getWidget());

        setMovementArrowColorSource(m_movementArrowColorSource,
                                    m_measureCount, m_residualMagnitude);

        connect(m_controlNetGraphics, SIGNAL(destroyed(QObject *)),
                this, SLOT(objectDestroyed(QObject *)));
      }
      catch(IException &e) {
        QString message = "Invalid control network.\n";
        message += e.toString();
        QMessageBox::information(getWidget(), "Error", message);
        return;
      }

      if (m_displayControlNetButton)
        m_displayControlNetButton->setEnabled(true);

      if (m_displayConnectivity)
        m_displayConnectivity->setEnabled(true);

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
    foreach (Image *image, getWidget()->images()) {
      image->displayProperties()->setColor(ImageDisplayProperties::randomColor());
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


  void MosaicControlNetTool::mouseButtonRelease(QPointF point, Qt::MouseButton s) {
    if (!isActive() || !m_controlNet) return;

    //  Find closes point

  }
}

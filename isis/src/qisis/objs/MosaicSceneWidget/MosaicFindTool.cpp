#include "MosaicFindTool.h"

#include <QDialog>
#include <QDoubleValidator>
#include <QGraphicsEllipseItem>
#include <QGraphicsScene>
#include <QGridLayout>
#include <QLabel>
#include <QLineEdit>
#include <QMenu>
#include <QMessageBox>
#include <QPushButton>

#include "iString.h"
#include "FindSpotGraphicsItem.h"
#include "MosaicGraphicsView.h"
#include "MosaicSceneWidget.h"
#include "Projection.h"

namespace Isis {
  /**
   * MosaicFindTool constructor
   *
   *
   * @param parent
   */
  MosaicFindTool::MosaicFindTool(MosaicSceneWidget *scene) :
      MosaicTool(scene) {
    p_findSpot = NULL;
  }


  /**
   *
   *
   */
  void MosaicFindTool::getUserGroundPoint() {

    //Validate latitude value
    QString latitude = p_latLineEdit->text();
    int cursorPos = 0;
    QValidator::State validLat =
      p_latLineEdit->validator()->validate(latitude, cursorPos);
    if(validLat != QValidator::Acceptable) {
      QMessageBox::warning(getWidget(), "Error",
                           "Latitude value must be in the range -90 to 90",
                           QMessageBox::Ok, QMessageBox::NoButton, QMessageBox::NoButton);
      return;
    }

    //Validate longitude value
    QString longitude = p_lonLineEdit->text();
    QValidator::State validLon =
      p_lonLineEdit->validator()->validate(longitude, cursorPos);
    if(validLon != QValidator::Acceptable) {
      QMessageBox::warning(getWidget(), "Error",
                           "Longitude value must be a double",
                           QMessageBox::Ok, QMessageBox::NoButton, QMessageBox::NoButton);
      return;
    }

    double lat = Isis::iString(latitude.toStdString()).ToDouble();
    double lon = Isis::iString(longitude.toStdString()).ToDouble();

    Isis::Projection *projection = getWidget()->getProjection();

    if(projection && projection->SetGround(lat, lon)) {
      QPointF scenePos(projection->XCoord(), -1 * projection->YCoord());
      QRectF sceneRect(getWidget()->getView()->sceneRect());

      if(sceneRect.contains(scenePos)) {
        if(p_findSpot != NULL) {
          clearPoint();
        }

        p_findSpot = new FindSpotGraphicsItem(scenePos, getWidget());

        getWidget()->getScene()->addItem(p_findSpot);
        getWidget()->getView()->centerOn(scenePos);
      }
      else {
        QString message = "Lat/Lon not within this view.";
        QMessageBox::information(getWidget(), "Point Not Found",
                                 message, QMessageBox::Ok);
      }
    }
  }


  /**
   * Adds the action to the toolpad.
   *
   *
   * @param toolpad
   *
   * @return QAction*
   */
  QAction *MosaicFindTool::getPrimaryAction() {
    p_action = new QAction(this);
    p_action->setIcon(getIcon("find.png"));
    p_action->setToolTip("Find (F)");
    p_action->setShortcut(Qt::Key_F);
    QString text  =
      "<b>Function:</b>  Find the specified lat/lon. \
      <p><b>Shortcut:</b>  F</p> ";
    p_action->setWhatsThis(text);
    return p_action;
  }


  QWidget *MosaicFindTool::getToolBarWidget() {
    p_latLineEdit = new QLineEdit();
    p_latLineEdit->setValidator(new QDoubleValidator(-90.0, 90.0, 99, this));

    p_lonLineEdit = new QLineEdit();
    p_lonLineEdit->setValidator(new QDoubleValidator(this));

    QLabel *latLabel = new QLabel("Latitude");
    QLabel *lonLabel = new QLabel("Longitude");

    // Create the action buttons
    QPushButton *okButton = new QPushButton("Go to Point");
    connect(okButton, SIGNAL(clicked()), this, SLOT(getUserGroundPoint()));

    QPushButton *clearButton = new QPushButton("Clear Point");
    connect(clearButton, SIGNAL(clicked()), this, SLOT(clearPoint()));

    // Put the buttons in a horizontal orientation
    QHBoxLayout *actionLayout = new QHBoxLayout();
    actionLayout->addWidget(latLabel);
    actionLayout->addWidget(p_latLineEdit);
    actionLayout->addWidget(lonLabel);
    actionLayout->addWidget(p_lonLineEdit);
    actionLayout->addWidget(okButton);
    actionLayout->addWidget(clearButton);
    actionLayout->addStretch(1);
    actionLayout->setMargin(0);

    QWidget *toolBarWidget = new QWidget;
    toolBarWidget->setLayout(actionLayout);

    return toolBarWidget;
  }


  /**
   * Adds the pan action to the given menu.
   *
   *
   * @param menu
   */
  void MosaicFindTool::addToMenu(QMenu *menu) {

  }


  PvlObject MosaicFindTool::toPvl() const {
    PvlObject obj(projectPvlObjectName());

    obj += PvlKeyword("Latitude", p_latLineEdit->text());
    obj += PvlKeyword("Longitude", p_lonLineEdit->text());
    obj += PvlKeyword("Visible", (p_findSpot != NULL));

    return obj;
  }


  void MosaicFindTool::fromPvl(PvlObject &obj) {
    p_latLineEdit->setText(obj["Latitude"][0]);
    p_lonLineEdit->setText(obj["Longitude"][0]);
    if((int)obj["Visible"][0] != 0) {
      getUserGroundPoint();
    }
  }


  iString MosaicFindTool::projectPvlObjectName() const {
    return "MosaicFindTool";
  }


  /**
   * Creates the widget to add to the tool bar.
   *
   *
   * @param parent
   *
   * @return QWidget*
   */
  QWidget *MosaicFindTool::createToolBarWidget() {
    QWidget *widget = new QWidget();
    return widget;
  }


  void MosaicFindTool::mouseButtonRelease(QPointF mouseLoc, Qt::MouseButton s) {
    if(!isActive())
      return;

    if(s == Qt::LeftButton) {
      Isis::Projection *proj = getWidget()->getProjection();

      if(proj && getWidget()->getView()->sceneRect().contains(mouseLoc)) {
        if(proj->SetCoordinate(mouseLoc.x(), -1 * mouseLoc.y())) {
          if(p_findSpot != NULL) {
            clearPoint();
          }

          p_findSpot = new FindSpotGraphicsItem(mouseLoc, getWidget());

          getWidget()->getScene()->addItem(p_findSpot);

          p_latLineEdit->setText(QString::number(proj->Latitude(), 'g', 10));
          p_lonLineEdit->setText(QString::number(proj->Longitude(), 'g', 10));
        }
      }
    }
  }


  /**
   *
   *
   */
  void MosaicFindTool::clearPoint() {
    if(p_findSpot != NULL) {
      getWidget()->getScene()->removeItem(p_findSpot);

      delete p_findSpot;
      p_findSpot = NULL;
    }
  }


  /**
   * This method sets the QGraphicsView to allow the user to select
   * mosaic items by dragging a rubber band.
   *
   */
  void MosaicFindTool::updateTool() {
    if(isActive()) {
    }
  }

}


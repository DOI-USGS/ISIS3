#include "MosaicFindTool.h"

#include <QMenu>

namespace Qisis {
  /**
   * MosaicFindTool constructor
   * 
   * 
   * @param parent 
   */
  MosaicFindTool::MosaicFindTool (QWidget *parent) : Qisis::MosaicTool(parent) {
    connect(this,SIGNAL(activated(bool)),this,SLOT(updateTool()));
    p_parent = (MosaicWidget *) parent;
    p_findSpot = NULL;
    createDialog(parent);
  }


   /**
   * Creates the dialog used by this tool
   * 
   * @param parent 
   */
  void MosaicFindTool::createDialog(QWidget *parent) {
    
    p_dialog = new QDialog(parent);
    p_dialog->setWindowTitle("Find Latitude/Longitude Coordinate");

    p_latLineEdit = new QLineEdit();
    p_latLineEdit->setText("0");
    p_latLineEdit->setValidator(new QDoubleValidator(-90.0,90.0,99,parent));
    p_lonLineEdit = new QLineEdit();
    p_lonLineEdit->setText("0");
    p_lonLineEdit->setValidator(new QDoubleValidator(parent));
    QLabel *latLabel = new QLabel("Latitude");
    QLabel *lonLabel = new QLabel("Longitude");

    // Put the buttons and text field in a gridlayout
    QGridLayout *gridLayout = new QGridLayout ();
    gridLayout->addWidget(latLabel,0,0);
    gridLayout->addWidget(p_latLineEdit,0,1);
    gridLayout->addWidget(lonLabel,1,0);
    gridLayout->addWidget(p_lonLineEdit, 1, 1);

    // Create the action buttons
    QPushButton *okButton = new QPushButton ("Ok");
    connect(okButton, SIGNAL(clicked()), this, SLOT(getUserGroundPoint()));

    QPushButton *clearButton = new QPushButton("Clear Dot");
    connect(clearButton, SIGNAL(clicked()), this, SLOT(clearPoint()));

    QPushButton* cancelButton = new QPushButton ("Cancel");
    connect(cancelButton, SIGNAL(clicked()), this, SLOT(clearPoint()));
    connect(cancelButton, SIGNAL(clicked()), p_dialog, SLOT(hide()));

    // Put the buttons in a horizontal orientation
    QHBoxLayout *actionLayout = new QHBoxLayout();
    actionLayout->addWidget(okButton);
    actionLayout->addWidget(clearButton);
    actionLayout->addWidget(cancelButton);

    QVBoxLayout *dialogLayout = new QVBoxLayout;
    dialogLayout->addLayout(gridLayout);
    dialogLayout->addLayout(actionLayout); 
    p_dialog->setLayout(dialogLayout);
       
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
      p_latLineEdit->validator()->validate(latitude,cursorPos);
    if (validLat != QValidator::Acceptable) {
      QMessageBox::warning(p_dialog, "Error",
                "Latitude value must be in the range -90 to 90",
                QMessageBox::Ok,QMessageBox::NoButton,QMessageBox::NoButton);
      return;
    }

    //Validate longitude value
    QString longitude = p_lonLineEdit->text();
    QValidator::State validLon = 
      p_lonLineEdit->validator()->validate(longitude,cursorPos);
    if (validLon != QValidator::Acceptable) {
      QMessageBox::warning(p_dialog, "Error",
                "Longitude value must be a double",
                QMessageBox::Ok,QMessageBox::NoButton,QMessageBox::NoButton);
      return;
    }

    double lat = Isis::iString(latitude.toStdString()).ToDouble();
    double lon = Isis::iString(longitude.toStdString()).ToDouble();

    Isis::Projection *projection = p_parent->projection();

    if(projection->Has180Domain()){ 
      lon = projection->To180Domain(lon);
      if(projection->IsPositiveWest()) lon = projection->ToPositiveWest(lon, 180);
    }else if(projection->IsPositiveWest()){
      lon = projection->ToPositiveWest(lon, 360);
    }
    // convert lat. if necessary
    if(projection->IsPlanetographic()) {
      lat = projection->ToPlanetographic(lat, projection->EquatorialRadius(), projection->PolarRadius());
    }

    if (projection->SetGround(lat, lon)) {
      double x = projection->XCoord();
      double y = -1 *(projection->YCoord());
      
      //get the graphics scene
      QGraphicsScene *graphicsScene = p_parent->scene();
    
      if(graphicsScene->sceneRect().contains(QPointF(x,y))){
        QTransform m = graphicsScene->views().last()->transform();
        double size = (8.0 / m.m11());
        //double size = (p_parent->screenResolution()) * 2;
        QRectF rect = QRectF(x-(size/2),y-(size/2), size, size);
        if(p_findSpot != NULL) {
          clearPoint();
        }
        p_findSpot = graphicsScene->addEllipse(rect, QPen(Qt::red), QBrush(Qt::red, Qt::SolidPattern));
        graphicsScene->views().last()->centerOn(x,y);
      } else {
        QString message = "Lat/Lon not within this view.";
        QMessageBox::information(p_parent, "Point Not Found", message, QMessageBox::Ok);
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
  QAction *MosaicFindTool::toolPadAction(ToolPad *toolpad) {
    p_action = new QAction(toolpad);
    p_action->setIcon(QPixmap(toolIconDir()+"/find.png"));
    p_action->setToolTip("Find (F)");
    p_action->setShortcut(Qt::Key_F);
    QString text  =
      "<b>Function:</b>  Find the specified lat/lon. \
      <p><b>Shortcut:</b>  F</p> ";
    p_action->setWhatsThis(text);
    return p_action;
  }


  /**
   * Adds the pan action to the given menu.
   * 
   * 
   * @param menu 
   */
  void MosaicFindTool::addToMenu(QMenu *menu) {
    
  }


  /**
   * Creates the widget to add to the tool bar.
   * 
   * 
   * @param parent 
   * 
   * @return QWidget* 
   */
  QWidget *MosaicFindTool::createToolBarWidget (QStackedWidget *parent) {
    QWidget *hbox = new QWidget(parent);
    return hbox;
  }


  /**
   * 
   * 
   */
  void MosaicFindTool::clearPoint() {
     if(p_findSpot != NULL) {
       p_parent->scene()->removeItem(p_findSpot);
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
      p_dialog->show();
      p_action->setChecked(false);    
    }
  }

}

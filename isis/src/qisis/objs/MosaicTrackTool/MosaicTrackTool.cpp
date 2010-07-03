#include "MosaicTrackTool.h"

namespace Qisis {
  /**
   * MosaicTrackTool constructor
   * 
   * 
   * @param parent 
   */
  MosaicTrackTool::MosaicTrackTool (QStatusBar *parent) : Qisis::MosaicTool(parent) {
    p_sbar = parent;

    p_latLabel = new QLabel(p_sbar);
    p_latLabel->setText("W 999999");
    p_latLabel->setMinimumSize(p_latLabel->sizeHint());
    p_latLabel->setToolTip("Latitude");
    p_sbar->addPermanentWidget(p_latLabel);

    p_lonLabel = new QLabel(p_sbar);
    p_lonLabel->setText("W 999999");
    p_lonLabel->setMinimumSize(p_lonLabel->sizeHint());
    p_lonLabel->setToolTip("Longitude");
    p_sbar->addPermanentWidget(p_lonLabel);

    clearLabels();

    activate(true);
  }


  /**
   * Updates the labels anytime the mouse moves.
   * SLA - does nothing right now.  it's not even called 
   * when the mosue moves. 
   * 
   * @param p 
   */
  void MosaicTrackTool::mouseMove(QPoint p) {
  }


  /**
   * Clears the labels if the mouse leaves the application.
   * 
   */
  void MosaicTrackTool::mouseLeave() {
  }


  /**
   * Updates the tracking labels. 
   * Displays the Planetocentric Latitude and 360 Positive East 
   * Longitude in the lower right corner of the window. 
   * 
   * 
   * @param p 
   */
  void MosaicTrackTool::updateLabels (QPointF p) {
    bool inChildItem = false;
    //----------------------------------------------------------------------
    // we need to find out if the point given is over an item, if not, call 
    // clearLables() if so, then we need to get the item and figure out the 
    // lat/lon that corresponds with the given point.
    //----------------------------------------------------------------------
    QList<MosaicItem *> items = getWidget()->mosaicItems();
    for(int i = 0; i < items.size(); i++) {
      //if(inChildItem) break;
      if(items[i]->contains(p)) {
        // now we need to get the translation from screen coords. to lat/lon.
        QPointF groundPoints = items[i]->screenToGround(p);
        p_lonLabel->setText("LON " + QString::number(groundPoints.y()));
        p_latLabel->setText("LAT " + QString::number(groundPoints.x()));
        break;
      } else {
        QList<QGraphicsItem *>children = items[i]->children();
        for(int j= 0; j < children.size(); j++) {
          if(children[j]->contains(p)){
            QPointF groundPoints = items[i]->screenToGround(p);
            p_lonLabel->setText("LON " + QString::number(groundPoints.y()));
            p_latLabel->setText("LAT " + QString::number(groundPoints.x()));
            inChildItem = true;
            break; 
          }
        }
      }
      if(inChildItem) break;
      clearLabels();
    }
  }


  /**
   * Clears the labels.
   * 
   */
  void MosaicTrackTool::clearLabels () {
    p_lonLabel->setText("LON n/a");
    p_latLabel->setText("LAT n/a");
    
  }

}



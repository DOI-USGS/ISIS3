#ifndef MosaicZoomTool_h
#define MosaicZoomTool_h

#include "MosaicTool.h"

class QAction;
class QLineEdit;

namespace Qisis {
  /**
   * @brief Handles zoom operations for Isis qt apps
   *
   * @ingroup Visualization Tools
   *
   * @author  Stacy Alley
   *
   * @internal
   *   @history
   */
  class MosaicZoomTool : public Qisis::MosaicTool {
    Q_OBJECT

    public:
      MosaicZoomTool (MosaicWidget *parent);
      void addToMenu(QMenu *menu);
      void rubberBandComplete(QRect r, QGraphicsSceneMouseEvent *mouseEvent);
      void updateResolutionBox();

    protected slots:
      void updateTool();
      
    protected:
      QAction *toolPadAction(ToolPad *toolpad);
      //! Returns the name of the menu.
      QString menuName() const { return "&View"; };
      QWidget *createToolBarWidget(QStackedWidget *parent);

    public slots:
      void zoomIn2X();
      void zoomOut2X();
    
      void zoomActual ();
      void zoomFit ();
      void zoomFitWidth ();
      void zoomFitHeight ();
      void zoomManual ();

    private:
      void zoomBy (double factor);
      QAction *p_zoomIn2X; //!< Zoom in 2 times.
      QAction *p_zoomOut2X; //!< Zoom out 2 times.
      QAction *p_zoomActual;  //!< Zoom to actual size action.
      QAction *p_zoomFit; //!< Fit the cube in the viewport action.
      QLineEdit *p_zoomLineEdit; //!< Line edit for manual zoom factor.
      QDoubleSpinBox *p_scaleBox;
      double p_screenResolution;
      MosaicWidget *p_parent;
  };
};

#endif


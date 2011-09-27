#ifndef MosaicZoomTool_h
#define MosaicZoomTool_h

#include "MosaicTool.h"

class QAction;
class QDoubleSpinBox;
class QGraphicsSceneMouseEvent;
class QLineEdit;
class QRect;

namespace Isis {
  /**
   * @brief Handles zoom operations for Isis qt apps
   *
   * @ingroup Visualization Tools
   *
   * @author  Stacy Alley
   *
   * @internal
   *   @history 2011-09-27 Steven Lambright - Improved user documentation
   */
  class MosaicZoomTool : public MosaicTool {
      Q_OBJECT

    public:
      MosaicZoomTool(MosaicSceneWidget *);
      void addToMenu(QMenu *menu);
      void updateResolutionBox();

    protected slots:
      void updateTool();

    protected:
      QAction *getPrimaryAction();
      QWidget *getToolBarWidget();
      void mouseButtonRelease(QPointF, Qt::MouseButton s);
      void mouseWheel(QPointF, int);
      void rubberBandComplete(QRectF r, Qt::MouseButton s);

    public slots:
      void zoomIn2X(QPointF center = QPointF());
      void zoomOut2X(QPointF center = QPointF());

      void zoomActual(QPointF center = QPointF());
      void zoomFit();
      void zoomFitWidth();
      void zoomFitHeight();
      void zoomManual();

    private:
      double limitZoomBy(double factor);
      void zoomBy(double factor, QPointF center = QPointF());
      QAction *p_zoomIn2X; //!< Zoom in 2 times.
      QAction *p_zoomOut2X; //!< Zoom out 2 times.
      QAction *p_zoomActual;  //!< Zoom to actual size action.
      QAction *p_zoomFit; //!< Fit the cube in the viewport action.
      QLineEdit *p_zoomLineEdit; //!< Line edit for manual zoom factor.
      QDoubleSpinBox *p_scaleBox;
      double p_screenResolution;
  };
};

#endif


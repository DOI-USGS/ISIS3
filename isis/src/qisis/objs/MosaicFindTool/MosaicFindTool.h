#ifndef MosaicFindTool_h
#define MosaicFindTool_h

#include "MosaicTool.h"
#include <QAction>

namespace Qisis {
  /**
   * @brief Handles selection operations for Isis qt apps
   *
   * @ingroup Visualization Tools
   *
   * @author  Stacy Alley
   *
   * @internal
   *   @history
   */
  class MosaicFindTool : public Qisis::MosaicTool {
    Q_OBJECT

    public:
      MosaicFindTool (QWidget *parent);
      void addToMenu(QMenu *menu);

    protected slots:
      void updateTool();
      
    protected:
      QAction *toolPadAction(ToolPad *toolpad);
      //! Returns the name of the menu.
      QString menuName() const { return "&View"; };
      QWidget *createToolBarWidget(QStackedWidget *parent);

    public slots:
      void getUserGroundPoint();
      void clearPoint();
      
    private:
      void createDialog(QWidget *parent);
      QDialog *p_dialog;
      QAction *p_findPoint;
      QToolButton *p_findPtButton;
      QLineEdit *p_status;
      bool p_released;
      bool p_pressed;
      QPoint p_point;
      QPen p_pen;
      bool p_paint;
      QLineEdit *p_lonLineEdit; //!< Input for longitude
      QLineEdit *p_latLineEdit; //!< Input for latitude
      MosaicWidget *p_parent;
      QGraphicsEllipseItem *p_findSpot;
      QAction *p_action;
      
  };
};

#endif


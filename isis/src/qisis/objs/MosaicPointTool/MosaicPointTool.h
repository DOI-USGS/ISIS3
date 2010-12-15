#ifndef Qisis_MosaicPointTool_h
#define Qisis_MosaicPointTool_h

#include "MosaicTool.h"

class QLabel;
namespace Isis {
  class ControlNet;
}

namespace Qisis {

  /**
   * @brief
   *
   * @ingroup Visualization Tools
   *
   * @author Stacy Alley
   *
   * @internal
   *
   *  @history 2010-10-26 Tracie Sucharski Added missing includes to cpp after
   *                                removing includes from ControlNet.h.
   */

  class MosaicPointTool : public Qisis::MosaicTool {
      Q_OBJECT

    public:
      MosaicPointTool(QWidget *parent);
      void addToMenu(QMenu *menu);

    public slots:
      void findPoint(QPointF p, Isis::ControlNet *cn);

    protected:
      QAction *toolPadAction(ToolPad *toolpad);
      //! Returns the name of the menu.
      QString menuName() const {
        return "&View";
      };
      QWidget *createToolBarWidget(QStackedWidget *parent);

    private:

      MosaicWidget *p_parent;
      QMessageBox *p_pointInfoDialog;
      QVBoxLayout *p_vLayout;
      QLabel *p_pointIdLabel;
      QLabel *p_numMeasuresLabel;
      QLabel *p_pointTypeLabel;
  };
};

#endif

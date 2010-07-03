#ifndef MosaicPanTool_h
#define MosaicPanTool_h

#include "MosaicTool.h"

#include <QAction>

namespace Qisis {
  /**
   * @brief Handles panning operations for Isis qt apps
   *
   * @ingroup Visualization Tools
   *
   * @author  Stacy Alley
   *
   * @internal
   *   @history
   */
  class MosaicPanTool : public Qisis::MosaicTool {
    Q_OBJECT

    public:
      MosaicPanTool (QWidget *parent);
      void addToMenu(QMenu *menu);

    protected slots:
      void updateTool();

    protected:
      QAction *toolPadAction(ToolPad *toolpad);
      //! Returns the name of the menu.
      QString menuName() const { return "&View"; };
      QWidget *createToolBarWidget(QStackedWidget *parent);

    public slots:
      
    private:
      MosaicWidget *p_parent;
  };
};

#endif


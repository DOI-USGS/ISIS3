#ifndef MosaicControlNetTool_h
#define MosaicControlNetTool_h

#include "MosaicTool.h"
#include <QAction>

namespace Qisis {
  /**
   * @brief Handles Control Net displays
   *
   * @ingroup Visualization Tools
   *
   * @author  Christopher Austin
   *
   * @internal
   *   @history 2010-06-24 Christopher Austin - Added |x| functionality and fixed
   *    	control net loading
   */
  class MosaicControlNetTool : public Qisis::MosaicTool {
      Q_OBJECT

    public:
      MosaicControlNetTool(MosaicWidget *parent);
      void addToMenu(QMenu *menu);

    public slots:

    protected:
      QAction *toolPadAction(ToolPad *toolpad);
      //! Returns the name of the menu.
      QString menuName() const {
        return "&View";
      };
      QWidget *createToolBarWidget(QStackedWidget *parent);

    protected slots:
      void updateTool();
      void hideTool();
      void loadControlNet();
      void displayControlNet();
      void displayConnectivity();

    private:
      void createDialog(QWidget *parent);
      QDialog *p_dialog;
      MosaicWidget *p_parent;
      QAction *p_action;

      QPushButton *p_loadControlNetButton;
      QPushButton *p_displayControlNetButton;
      QPushButton *p_displayConnectivity;
      QAction *p_connectivity;
  };
};

#endif


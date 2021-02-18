#ifndef MosaicPanTool_h
#define MosaicPanTool_h

#include "MosaicTool.h"

#include <QAction>

namespace Isis {
  /**
   * @brief Handles panning operations for Isis qt apps
   *
   * @ingroup Visualization Tools
   *
   * @author ????-??-?? Stacy Alley
   *
   * @internal
   *   @history 2011-05-07 Steven Lambright - Refactored along with all of
   *                       'qmos.' A lot of work still needs to be done for
   *                       these tools.
   *   @history 2011-09-27 Steven Lambright - Improved user documentation
   */
  class MosaicPanTool : public MosaicTool {
      Q_OBJECT

    public:
      MosaicPanTool(MosaicSceneWidget *);

    protected slots:
      void updateTool();

    protected:
      QAction *getPrimaryAction();
      QWidget *createToolBarWidget();

    public slots:

    private:
  };
};

#endif


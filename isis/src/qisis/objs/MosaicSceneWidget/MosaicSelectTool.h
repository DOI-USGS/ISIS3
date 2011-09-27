#ifndef MosaicSelectTool_h
#define MosaicSelectTool_h

#include "MosaicTool.h"

#include <QAction>

namespace Isis {
  /**
   * @brief Handles selection operations for Isis qt apps
   *
   * @ingroup Visualization Tools
   *
   * @author  Stacy Alley
   *
   * @internal
   *   @history 2011-09-27 Steven Lambright - Improved user documentation
   */
  class MosaicSelectTool : public MosaicTool {
      Q_OBJECT

    public:
      MosaicSelectTool(MosaicSceneWidget *);

    protected slots:
      void updateTool();

    protected:
      virtual QAction *getPrimaryAction();

    public slots:

    private:

  };
};

#endif


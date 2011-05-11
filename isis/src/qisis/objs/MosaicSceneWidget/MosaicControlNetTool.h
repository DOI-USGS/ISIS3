#ifndef MosaicControlNetTool_h
#define MosaicControlNetTool_h

#include "MosaicTool.h"

class QDialog;
class QPushButton;

namespace Isis {
  class ControlNet;
  class ControlNetGraphicsItem;
  class CubeDisplayProperties;

  /**
   * @brief Handles Control Net displays
   *
   * @ingroup Visualization Tools
   *
   * @author  Christopher Austin
   *
   * @internal
   *   @history 2010-06-24 Christopher Austin - Added |x| functionality and fixed
   *            control net loading
   *   @history 2010-05-07 Steven Lambright - Refactored.
   *   @history 2010-05-10 Steven Lambright - Reduced useless code, open cnet
   *            button is now always available.
   */
  class MosaicControlNetTool : public MosaicTool {
      Q_OBJECT

    public:
      MosaicControlNetTool(MosaicSceneWidget *);
      ~MosaicControlNetTool();

      void addToMenu(QMenu *menu);

      PvlObject toPvl() const;
      void fromPvl(PvlObject &obj);
      iString projectPvlObjectName() const;

    public slots:

    protected:
      QAction *getPrimaryAction();
      QWidget *getToolBarWidget();

    protected slots:
      void updateTool();
      void openControlNet();
      void displayControlNet();
      void displayConnectivity();
      void closeNetwork();

    private slots:
      void loadNetwork();
      void objectDestroyed(QObject *);

    private:
      void createDialog();

      CubeDisplayProperties *
          takeDisplay(QString sn, QList< CubeDisplayProperties *> &displays);

      QPushButton *p_loadControlNetButton;
      QPushButton *p_displayControlNetButton;
      QPushButton *p_displayConnectivity;
      QPushButton *p_closeNetwork;
      QAction *p_connectivity;
      ControlNet *p_controlNet;
      ControlNetGraphicsItem *p_controlNetGraphics;
      QString p_controlNetFile;
  };
};

#endif


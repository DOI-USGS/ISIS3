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
      void objectDestroyed(QObject *);

    private:
      void loadNetwork();
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


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
   *            button is now always enabled.
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
      void displayArrows();
      void displayConnectivity();
      void displayControlNet();
      void closeNetwork();

    private slots:
      void loadNetwork();
      void objectDestroyed(QObject *);

    private:
      void createDialog();

      CubeDisplayProperties *
          takeDisplay(QString sn, QList< CubeDisplayProperties *> &displays);

      QPushButton *m_loadControlNetButton;
      QPushButton *m_displayControlNetButton;
      QPushButton *m_displayConnectivity;
      QPushButton *m_displayArrows;
      QPushButton *m_closeNetwork;
      QAction *m_connectivity;
      ControlNet *m_controlNet;
      ControlNetGraphicsItem *m_controlNetGraphics;
      QString m_controlNetFile;
  };
};

#endif


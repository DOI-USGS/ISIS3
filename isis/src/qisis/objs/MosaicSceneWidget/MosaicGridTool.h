#ifndef MosaicGridTool_h
#define MosaicGridTool_h

#include "MosaicTool.h"

#include <QPointer>

class QAction;
class QDialog;
class QGraphicsItem;
class QLineEdit;
class QToolButton;

namespace Isis {
  /**
   * @brief This controls the 'Grid' abilities in the MosaicSceneWidget 
   *
   * @ingroup Visualization Tools
   *
   * @author  Stacy Alley
   *
   * @internal
   *   @history 2011-05-07 Steven Lambright - Refactored along with all of
   *                       'qmos'
   *   @history 2011-05-11 Steven Lambright - Added project settings
   */
  class MosaicGridTool : public MosaicTool {
      Q_OBJECT

    public:
      MosaicGridTool(MosaicSceneWidget *);
      void addToMenu(QMenu *menu);

      PvlObject toPvl() const;
      void fromPvl(const PvlObject &obj);
      iString projectPvlObjectName() const;

    public slots:
      void getUserGrid();
      void clearGrid();

    protected:
      QAction *getPrimaryAction();
      QWidget *getToolBarWidget();
      QWidget *createToolBarWidget();

    private:
      QPointer<QLineEdit> m_baseLatLineEdit; //!< Input for base latitude
      QPointer<QLineEdit> m_baseLonLineEdit; //!< Input for base longitude
      QPointer<QLineEdit> m_latIncLineEdit; //!< Input for latitude increment
      QPointer<QLineEdit> m_lonIncLineEdit; //!< Input for longitude increment
      QPointer<QLineEdit> m_densityEdit; //!< Input for grid density
      QGraphicsItem *m_gridItem;
      QAction *m_action;
  };
};

#endif


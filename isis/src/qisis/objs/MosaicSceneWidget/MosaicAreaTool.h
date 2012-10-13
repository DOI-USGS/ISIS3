#ifndef MosaicAreaTool_h
#define MosaicAreaTool_h

#include "MosaicTool.h"

class QAction;
class QGraphicsPolygonItem;
class QLineEdit;

namespace Isis {
  class Distance;
  class PvlObject;

  /**
   * @brief This displays a box with a given distance from a point
   *
   * @ingroup Visualization Tools
   *
   * @author ????-??-?? Steven Lambright
   *
   * @internal
   *   @history 2011-09-27 Steven Lambright - Improved user documentation
   *   @history 2012-04-16 Jeannie Backer - Added forward declaration for
   *                           PvlObject. Added #include for PvlKeyword and
   *                           PvlObject classes in implementation file.
   */
  class MosaicAreaTool : public MosaicTool {
      Q_OBJECT

    public:
      MosaicAreaTool(MosaicSceneWidget *);
      void addToMenu(QMenu *menu);

      PvlObject toPvl() const;
      void fromPvl(const PvlObject &obj);
      IString projectPvlObjectName() const;

    protected:
      QAction *getPrimaryAction();
      QWidget *getToolBarWidget();
      QWidget *createToolBarWidget();
      void mouseButtonRelease(QPointF, Qt::MouseButton);

    public slots:
      void userChangedBox();
      void clearBox();

    private:
      QRectF calcLatLonRange(QPointF centerLatLon, Distance size);

      QAction *m_drawBox;
      QLineEdit *m_lonLineEdit; //!< Input for longitude
      QLineEdit *m_latLineEdit; //!< Input for latitude
      QLineEdit *m_areaLineEdit; //!< Input for latitude
      QGraphicsPolygonItem *m_box;
      QAction *m_action;
  };
};

#endif


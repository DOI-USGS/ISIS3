#ifndef MosaicAreaTool_h
#define MosaicAreaTool_h

#include "MosaicTool.h"

class QAction;
class QGraphicsPolygonItem;
class QLineEdit;

namespace Isis {
  class Distance;

  /**
   * @brief This displays a box with a given distance from a point
   *
   * @ingroup Visualization Tools
   *
   * @author  Steven Lambright
   *
   * @internal
   */
  class MosaicAreaTool : public MosaicTool {
      Q_OBJECT

    public:
      MosaicAreaTool(MosaicSceneWidget *);
      void addToMenu(QMenu *menu);

      PvlObject toPvl() const;
      void fromPvl(PvlObject &obj);
      iString projectPvlObjectName() const;

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


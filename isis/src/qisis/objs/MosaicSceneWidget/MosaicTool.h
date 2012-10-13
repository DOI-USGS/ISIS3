#ifndef MosaicTool_h
#define MosaicTool_h

#include <QObject>

// required since it's in a slot
#include <QPointF>
#include <QRectF>

class QAction;
class QGraphicsView;
class QMenu;
class QPixmap;
class QPoint;
class QToolBar;

namespace Isis {
  class ToolPad;
}

namespace Isis {
  class IString;
  class MosaicSceneWidget;
  class PvlObject;

  /**
   * @brief Base class for the MosaicTools
   *
   * @ingroup Visualization Tools
   *
   * @author ????-??-?? Stacy Alley
   *
   * @internal
   *   @history 2011-04-14 Steven Lambright Refactored to use the new
   *                           MosaicSceneWidget
   *   @history 2011-09-27 Steven Lambright - No longer produces errors when
   *                           given a NULL MosaicSceneWidget.
   *   @history 2011-11-04 Steven Lambright - Added getViewActions().
   */
  class MosaicTool : public QObject {
      Q_OBJECT

    public:
      MosaicTool(MosaicSceneWidget *);
      ~MosaicTool();

      /**
       * Returns the activeness of this toool
       *
       *
       * @return bool
       */
      bool isActive() const {
        return p_active;
      };


      /**
       * returns the path to the icon directory.
       *
       *
       * @return QString
       */
      QPixmap getIcon(IString iconName) const;

      virtual void addTo(QMenu *menu) {}
      virtual void addTo(ToolPad *toolPad);
      virtual void addTo(QToolBar *toolBar);

      virtual QList<QAction *> getViewActions();

      virtual PvlObject toPvl() const;
      virtual void fromPvl(const PvlObject &obj);
      virtual IString projectPvlObjectName() const;

    signals:
      void activated(bool);

    public slots:
      void activate(bool);

    protected slots:
      virtual void updateTool() {};
      virtual void mouseEnter() {};
      virtual void mouseMove(QPointF);
      virtual void mouseLeave() {};
      virtual void mouseDoubleClick(QPointF);
      virtual void mouseButtonPress(QPointF, Qt::MouseButton s);
      virtual void mouseButtonRelease(QPointF, Qt::MouseButton s);
      virtual void mouseWheel(QPointF, int delta);
      virtual void rubberBandComplete(QRectF r, Qt::MouseButton s) {};

      void toolBarDestroyed(QObject *obj);

    signals:


    protected:
      MosaicSceneWidget *getWidget() {
        return p_widget;
      }

      /**
       * This method returns an action that is used to activate this tool.
       *
       * This method will only be called once so it can new the action without
       *   a problem.
       */
      virtual QAction *getPrimaryAction() = 0;

      /**
       * This method returns a widget that will be put in a tool bar when the
       *   tool is activated.
       *
       * This method will only be called once so it can new the widget without
       *   a problem.
       */
      virtual QWidget *getToolBarWidget();

    private:
      void enableToolBar();
      void disableToolBar();
      bool p_active; //!< Is the tool active?

      MosaicSceneWidget *p_widget;

      QAction *p_primaryAction;
      QAction *p_toolBarAction;
  };
};

#endif


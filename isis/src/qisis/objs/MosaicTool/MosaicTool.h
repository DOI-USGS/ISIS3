#ifndef Qisis_MosaicTool_h
#define Qisis_MosaicTool_h

#include <QObject>
#include <QToolBar>
#include <QMenu>
#include <QStackedWidget>
#include <QGraphicsView>

#include "ToolPad.h"
#include "MosaicMainWindow.h"

namespace Qisis {
  /**
  * @brief Base class for the MosaicTools
  *
  * @ingroup Visualization Tools
  *
  * @author Stacy Alley
  *
  * @internal
  */

  class MosaicTool : public QObject {
    Q_OBJECT

    public:
      MosaicTool (QWidget *parent);
      void addTo (Qisis::ToolPad *toolpad);
      virtual void rubberBandComplete(QRect r){};


      /**
       * Returns the activeness of this toool
       * 
       * 
       * @return bool 
       */
      bool isActive() const { return p_active; };
      
      /** 
       * requires the programmer to have this member 
       * 
       * @param menu
       */
      virtual void addTo (QMenu *menu) {};


      /** 
       * requires the programmer to have this member 
       * 
       * @param toolbar
       */
      virtual void addToPermanent (QToolBar *toolbar) {};

      void addToActive (QToolBar *toolbar);

      /** 
       * requires the programmer to have this member 
       * 
       * @param mmw
       */
      virtual void addTo (MosaicMainWindow *mmw);


      /** 
       * returns the path to the icon directory.
       * 
       * 
       * @return QString
       */
      QString toolIconDir () const { return p_toolIconDir; };

      void setGraphicsView(QGraphicsView *view) {p_graphicsView = view; };
      QGraphicsView *getGraphicsView() { return p_graphicsView; };

      void setWidget(MosaicWidget *widget);
      MosaicWidget *getWidget() { return p_widget; };

      virtual void updateTool(){};

  signals:
      void activated(bool);

    public slots:
      void activate (bool);

    protected slots:
      /**
       * requires the programmer to have this member
       * 
       */
      virtual void mouseEnter() {};


      /** 
       * requires the programmer to have this member
       * 
       * @param p
       */
      virtual void mouseMove(QPoint p) {};


      /** 
       * requires the programmer to have this member
       * 
       */
      virtual void mouseLeave() {};


      /** 
       * requires the programmer to have this member
       * 
       * @param p
       */
      virtual void mouseDoubleClick(QPoint p) {};


      /** 
       * requires the programmer to have this member
       * 
       * @param p
       * @param s
       */
      virtual void mouseButtonPress(QPoint p, Qt::MouseButton s) {};


      /** 
       * requires the programmer to have this member
       * 
       * @param p
       * @param s
       */
      virtual void mouseButtonRelease(QPoint p, Qt::MouseButton s) {};
     
    signals:
     

    protected:

      /** 
       * Anytime a tool is created, you must setup a tool pad action with it.
       * 
       * @param toolpad
       * 
       * @return QAction*
       */
      virtual QAction *toolPadAction (ToolPad *toolpad) { return NULL; };


      /** 
       *  Anytime a tool is created, you must give it a name for the
       *  menu.
       *  
       * @return QString
       */
      virtual QString menuName () const { return ""; };


      /** 
       * Anytime a tool is created, you must add it to the tool bar.
       * 
       * @param parent
       * 
       * @return QWidget*
       */
      virtual QWidget *createToolBarWidget (QStackedWidget *parent) { return NULL; };

    private:
      void enableToolBar ();
      void disableToolBar();

      bool p_active; //!< Is the tool acitve?
      QWidget *p_toolBarWidget; //!< The tool bar on which this tool resides.
      QAction *p_toolPadAction; //!< The tool pad on which this tool resides.

      QString p_toolIconDir; //!< The pathway to the icon directory.

      static QStackedWidget *p_activeToolBarStack; //!< Active tool bars
      QGraphicsView *p_graphicsView;
      MosaicWidget *p_widget;
  };
};

#endif

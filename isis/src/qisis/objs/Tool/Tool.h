#ifndef Tool_h
#define Tool_h

/** This is free and unencumbered software released into the public domain.

The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */


#include <QObject>

#include <QDebug>
#include <QPoint>

class QAction;
class QMenu;
class QPainter;
//class QPoint;
class QStackedWidget;
class QToolBar;

template< class T > class QVector;


namespace Isis {
  class CubeViewport;
  class MdiCubeViewport;
  class RubberBandTool;
  class ToolList;
  class ToolPad;
  class ViewportMainWindow;
  class Workspace;

  /**
  * @brief Base class for the Qisis tools
  *
  * @ingroup Visualization Tools
  *
  * @author ????-??-?? Jeff Anderson
  *
  * @internal
  *  @history 2007-04-30  Tracie Sucharski,  Added Qpainter to parameters
  *                           for the paintViewport method.  This will allow
  *                           the tools to use the CubeViewport painter rather
  *                           than creating a new one which causes warnings
  *                           in Qt4.
  *  @history 2007-09-11  Steven Lambright - Added code to handle the rubber band tool
  *  @history 2007-11-19  Stacy Alley - Added code for the new ViewportMainWindow
  *                       class which is used to keep track of the size and
  *                       location of the qisis windows.
  *  @history 2009-03-27  Noah Hilt, Steven Lambright - Removed old rubber band
  *      and changed cubeViewportList to call the Workspace viewport list.
  *  @history 2010-03-18  Sharmila Prasad - Reset warning status when a different tool is chosen, done
  *                                    on mouse double click, press or release
  *  @history 2010-05-24  Eric Hyer - setCubeViewport is now public
  *  @history 2010-06-26  Eric Hyer - Now uses MdiCubeViewport
  *  @history 2010-11-08  Eric Hyer - Added connects and disconnects for
  *               cubeViewport's new mouseMove signal
  *  @history 2012-02-27  Tracie Sucharski - Added the signal, toolActivated,
  *               to enable a tool to connect and respond when user selects
  *               a given tool using the toolPad button.
  */

  class Tool : public QObject {
      Q_OBJECT

    public:
      Tool(QWidget *parent);

      void addTo(ViewportMainWindow *mw);

      void addTo(ToolPad *toolpad);

      /**
       *  Anytime a tool is created, you must give it a name for the
       *  menu.
       *
       * @return QString
       */
      virtual QString menuName() const {
        return "";
      }


      /**
       * @param menu
       */
      virtual void addTo(QMenu *menu) {};


      /**
       * @param toolbar
       */
      virtual void addToPermanent(QToolBar *toolbar) {};

      void addToActive(QToolBar *toolbar);

      /**
       * @param ws
       */
      virtual void addTo(Workspace *ws);


      /**
       * returns the path to the icon directory.
       *
       *
       * @return QString
       */
      QString toolIconDir() const {
        return m_toolIconDir;
      };


      /**
       * @param vp
       * @param painter
       */
      virtual void paintViewport(MdiCubeViewport *vp, QPainter *painter) {}

      RubberBandTool *rubberBandTool();
      void setList(ToolList *currentList);

    public slots:
      void activate(bool);
      virtual void updateTool();
      void setCubeViewport(MdiCubeViewport *cvp);


    signals:
      void clearWarningSignal();
      void toolActivated();

    protected slots:

      /**
       */
      virtual void rubberBandComplete() {  }


      /**
       * This is called when actions change which pixels from the cube
       *   are displayed.
       */
      virtual void screenPixelsChanged() {}

      /**
       */
      virtual void mouseEnter() {}

      virtual void mouseMove(QPoint p);
      virtual void mouseMove(QPoint p, Qt::MouseButton) {}

      /**
       */
      virtual void mouseLeave() {};

      virtual void mouseDoubleClick(QPoint p);
      virtual void mouseButtonPress(QPoint p, Qt::MouseButton s);
      virtual void mouseButtonRelease(QPoint p, Qt::MouseButton s);

      /**
       */
      virtual void updateMeasure() {}


      /**
       */
      virtual void scaleChanged() {}


      /**
       */
      virtual void stretchRequested(MdiCubeViewport *, int) {}


      /**
       * @param viewport
       */
      void registerTool(MdiCubeViewport *viewport);


    signals:
      /**
       */
      void viewportChanged();

    protected:
      /**
       * Return the current cubeviewport
       *
       * @return CubeViewport*
       */
      inline MdiCubeViewport *cubeViewport() const {
        return m_cvp;
      }


      /**
       * A list of cubeviewports.
       */
      typedef QVector< MdiCubeViewport * > CubeViewportList;

      CubeViewportList *cubeViewportList() const;


      /**
       * Anytime a tool is created, you must setup a tool pad action with it.
       *
       * @param toolpad
       *
       * @return QAction*
       */
      virtual QAction *toolPadAction(ToolPad *toolpad) {
        return NULL;
      }


      /**
       * Anytime a tool is created, you must add it to the tool bar.
       *
       * @param parent
       *
       * @return QWidget*
       */
      virtual QWidget *createToolBarWidget(QStackedWidget *parent) {
        return NULL;
      }


      /**
       * Anytime a tool is created, you must add the connections for it.
       *
       * @param cvp
       */
      virtual void addConnections(MdiCubeViewport *cvp) {}


      /**
       * Anytime a tool is created, you must be able to remove it's connections.
       *
       * @param cvp
       */
      virtual void removeConnections(MdiCubeViewport *cvp) {}


      //! Anytime a tool is created, you may use the rubber band tool.
      virtual void enableRubberBandTool();

      Workspace *workspace();

    private:
      void addViewportConnections();
      void removeViewportConnections();
      void enableToolBar();
      void disableToolBar();

      MdiCubeViewport *m_cvp;  //!< current cubeviewport
      Workspace *m_workspace;

      bool m_active;            //!< Is the tool acitve?
      QWidget *m_toolBarWidget;  //!< The tool bar on which this tool resides.
      QAction *m_toolPadAction;  //!< The tool pad on which this tool resides.
      QString m_toolIconDir;    //!< The pathway to the icon directory.
      ToolList *m_toolList;
  };
}

#endif

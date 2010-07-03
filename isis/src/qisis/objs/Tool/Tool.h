#ifndef Qisis_Tool_h
#define Qisis_Tool_h

/**
 * @file
 * $Date: 2010/07/01 22:52:23 $
 * $Revision: 1.21 $
 *
 *   Unless noted otherwise, the portions of Isis written by the USGS are public
 *   domain. See individual third-party library and package descriptions for
 *   intellectual property information,user agreements, and related information.
 *
 *   Although Isis has been used by the USGS, no warranty, expressed or implied,
 *   is made by the USGS as to the accuracy and functioning of such software
 *   and related material nor shall the fact of distribution constitute any such
 *   warranty, and no responsibility is assumed by the USGS in connection
 *   therewith.
 *
 *   For additional information, launch
 *   $ISISROOT/doc//documents/Disclaimers/Disclaimers.html in a browser or see
 *   the Privacy &amp; Disclaimers page on the Isis website,
 *   http://isis.astrogeology.usgs.gov, and the USGS privacy and disclaimers on
 *   http://www.usgs.gov/privacy.html.
 */


#include <QObject>

// FIXME: remove this include!
#include <QPoint>


class QAction;
class QMenu;
class QPainter;
//class QPoint;
class QStackedWidget;
class QToolBar;

template< class T > class QVector;


namespace Qisis {
  class CubeViewport;
  class MdiCubeViewport;
  class ToolPad;
  class ViewportMainWindow;
  class Workspace;

  /**
  * @brief Base class for the Qisis tools
  *
  * @ingroup Visualization Tools
  *
  * @author ??? Jeff Anderson
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
  */

  class Tool : public QObject {
      Q_OBJECT

    public:
      Tool(QWidget *parent);

      void addTo(Qisis::ViewportMainWindow *mw);

      void addTo(Qisis::ToolPad *toolpad);

      /**
       * requires the programmer to have this member
       *
       * @param menu
       */
      virtual void addTo(QMenu *menu) {};


      /**
       * requires the programmer to have this member
       *
       * @param toolbar
       */
      virtual void addToPermanent(QToolBar *toolbar) {};

      void addToActive(QToolBar *toolbar);

      /**
       * requires the programmer to have this member
       *
       * @param ws
       */
      virtual void addTo(Qisis::Workspace *ws);


      /**
       * returns the path to the icon directory.
       *
       *
       * @return QString
       */
      QString toolIconDir() const {
        return p_toolIconDir;
      };


      /**
       * requires the programmer to have this member
       *
       * @param vp
       * @param painter
       */
      virtual void paintViewport(MdiCubeViewport *vp, QPainter *painter) {};


    public slots:
      void activate(bool);
      virtual void updateTool();
      void setCubeViewport(Qisis::MdiCubeViewport *cvp);


    signals:
      void clearWarningSignal();

    protected slots:

      /**
       */
      virtual void rubberBandComplete() {};


      /**
       * This is called when actions change which pixels from the cube
       *   are displayed.
       */
      virtual void screenPixelsChanged() {};

      /**
       */
      virtual void mouseEnter() {};

      virtual void mouseMove(QPoint p);

      /**
       */
      virtual void mouseLeave() {};

      virtual void mouseDoubleClick(QPoint p);
      virtual void mouseButtonPress(QPoint p, Qt::MouseButton s);
      virtual void mouseButtonRelease(QPoint p, Qt::MouseButton s);

      /**
       */
      virtual void updateMeasure() {};


      /**
       */
      virtual void scaleChanged() {};


      /**
       */
      virtual void stretchRequested(MdiCubeViewport *, int) {};


      /**
       * @param viewport
       */
      void registerTool(Qisis::MdiCubeViewport *viewport);


    signals:
      /**
       */
      void viewportChanged();

    protected:
      /**
       * Return the current cubeviewport
       *
       * @return Qisis::CubeViewport*
       */
      inline Qisis::MdiCubeViewport *cubeViewport() const {
        return p_cvp;
      };


      /**
       * A list of cubeviewports.
       */
      typedef QVector< Qisis::MdiCubeViewport * > CubeViewportList;

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
      };


      /**
       *  Anytime a tool is created, you must give it a name for the
       *  menu.
       *
       * @return QString
       */
      virtual QString menuName() const {
        return "";
      };


      /**
       * Anytime a tool is created, you must add it to the tool bar.
       *
       * @param parent
       *
       * @return QWidget*
       */
      virtual QWidget *createToolBarWidget(QStackedWidget *parent) {
        return NULL;
      };


      /**
       * Anytime a tool is created, you must add the connections for it.
       *
       * @param cvp
       */
      virtual void addConnections(MdiCubeViewport *cvp) {};


      /**
       * Anytime a tool is created, you must be able to remove it's connections.
       *
       * @param cvp
       */
      virtual void removeConnections(MdiCubeViewport *cvp) {};


      //! Anytime a tool is created, you may use the rubber band tool.
      virtual void enableRubberBandTool();

      Qisis::MdiCubeViewport *p_cvp;  //!< current cubeviewport
      Workspace *p_workspace;

    private:
      void addViewportConnections();
      void removeViewportConnections();
      void enableToolBar();
      void disableToolBar();

      bool p_active;            //!< Is the tool acitve?
      QWidget *p_toolBarWidget;  //!< The tool bar on which this tool resides.
      QAction *p_toolPadAction;  //!< The tool pad on which this tool resides.
      QString p_toolIconDir;    //!< The pathway to the icon directory.
      static QStackedWidget *p_activeToolBarStack;  //!< Active tool bars
  };
};

#endif

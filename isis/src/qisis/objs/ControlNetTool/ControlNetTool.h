#ifndef ControlNetTool_h
#define ControlNetTool_h

#include "Tool.h"
#include "ControlPoint.h"

#include <QAction>
#include <QCloseEvent>
#include <QPalette>
#include <QPointer>
#include <QString>
#include <QStringList>


namespace Isis {
  class ControlMeasure;
  class ControlNet;
  class ControlPointEdit;
  class Cube;
  class CubeDnView;
  class CubeViewport;
  class Directory;
  class MainWindow;
  class MdiCubeViewport;
  class QnetHoldPointDialog;
  class SerialNumberList;
  class Stretch;
  class ToolPad;
  class UniversalGroundMap;

  /**
   * @brief ControlNetTool Handles mouse events on CubeDnViews for control point editing for the 
   *        ipce app.
   *
   * @ingroup Visualization Tools
   *
   * @author 2016-09-01 Tracie Sucharski
   *
   * @internal
   *   @history 2016-09-30 Tracie Sucharski - Pass in directory to constructor, so that we can
   *                           query for shapes and other data from the project.
   *   @history 2016-10-25 Tracie Sucharski - Check for existence of Control net in the
   *                           paintViewport method.
   *   @history 2017-05-18 Tracie Sucharski - Added serialNumber to the modifyControlPoint signal.
   *   @history 2017-07-27 Tyler Wilson - Added the ability for the Ipce tool to check and see
   *                           if control nets exist within the current project within
   *                           the toolPadAction function.  If no control nets exist within the
   *                           project, the Ipce tool is disabled on start-up.  Fixes #4994.
   *   @history 2017-08-02 Tracie Sucharski - Draw the current edit Control Point as a circle with
   *                           center crosshair in red.  Removed refresh method; it was not being
   *                           used.  Fixes #5007, #5008.
   *   @history 2017-08-08 Cole Neubauer - Renamed from IpceTool.  Fixes #5090. 
   *   @history 2017-08-09 Cole Neubauer - Added loadNetwork() for changing inbetween active
   *                           networks Fixes #4567
   *   @history 2018-03-12 Tracie Sucharski - Fixed some documentation leftover from renaming from
   *                           IpceTool.  References #5090.
   *   @history 2018-03-27 Tracie Sucharski - Redraw cube viewports when a new control net is
   *                           loaded.
   *   @history 2018-04-13 Tracie Sucharski - In mouseButtonRelease method return if a control net
   *                           has not been set.
   */
  class ControlNetTool : public Tool {
    Q_OBJECT

    public:
      ControlNetTool (Directory *directory, QWidget *parent);
      virtual ~ControlNetTool ();

      void setControlNet(ControlNet *controlNet);
      void paintViewport (MdiCubeViewport *cvp, QPainter *painter);

    signals:
      void modifyControlPoint(ControlPoint *controlPoint, QString serialNumber);
      void deleteControlPoint(ControlPoint *controlPoint);
      void createControlPoint(double latitude, double longitude, Cube *cube,
                              bool isGroundSource = false);

    public slots:
      void loadNetwork();

    protected:
      QAction *toolPadAction(ToolPad *pad);

    protected slots:
      void mouseButtonRelease(QPoint p, Qt::MouseButton s);

    private slots:
      void paintAllViewports();

    private:
      void createActions();
      void createMenus();
      void drawAllMeasurements(MdiCubeViewport *vp, QPainter *painter);

      QPointer<MainWindow> m_ControlNetTool;
      Directory *m_directory;
      CubeDnView *m_view;

      QPointer<ControlNet> m_controlNet;

      QPointer<Workspace> m_workspace;
  };
};

#endif

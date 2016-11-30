#ifndef IpceTool_h
#define IpceTool_h

#include "Tool.h"
#include "ControlPoint.h"

#include <QAction>
#include <QCloseEvent>
#include <QPalette>
#include <QPointer>
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
   * @brief Ipce (Qnet) tool operations
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
   *                           
   */
  class IpceTool : public Tool {
    Q_OBJECT

    public:
      IpceTool (Directory *directory, QWidget *parent);
      virtual ~IpceTool ();

      void setControlNet(ControlNet *controlNet);
      void paintViewport (MdiCubeViewport *cvp, QPainter *painter);

    signals:
      void modifyControlPoint(ControlPoint *controlPoint);
      void deleteControlPoint(ControlPoint *controlPoint);
      void createControlPoint(double latitude, double longitude, Cube *cube,
                              bool isGroundSource = false);

    public slots:
      void refresh();

    protected:
      QAction *toolPadAction(ToolPad *pad);

    protected slots:
      void mouseButtonRelease(QPoint p, Qt::MouseButton s);

    private slots:
      void paintAllViewports (QString pointId );

    private:
      void createActions();
      void createMenus();
      void drawAllMeasurements(MdiCubeViewport *vp, QPainter *painter);

      QPointer<MainWindow> m_ipceTool;
      Directory *m_directory;
      CubeDnView *m_view;

      QPointer<ControlNet> m_controlNet;

      QPointer<Workspace> m_workspace;
  };
};

#endif

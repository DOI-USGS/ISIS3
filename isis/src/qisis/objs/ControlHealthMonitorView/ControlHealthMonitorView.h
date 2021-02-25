#ifndef ControlHealthMonitorView_h
#define ControlHealthMonitorView_h

/** This is free and unencumbered software released into the public domain.

The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */

#include <QMap>
#include <QPointer>
#include <QToolBar>
#include <QWidgetAction>

#include "AbstractProjectItemView.h"

namespace Isis {
  class Control;
  class Directory;
  class ProjectItem;
  class ControlHealthMonitorWidget;
  class ControlNet;
  class ControlPoint;
  class ToolPad;

  /**
   * View for the Control Net Health Monitor
   *
   * @author 2018-06-07 Adam Goins
   *
   * @internal
   *   @history 2018-06-07 Adam Goins - Initial Version.
   *   @history 2018-06-26 Adam Goins - Made the view dockable with setCentralWidget().
   *   @history 2018-07-10 Tracie Sucharski - Remove sizePolicy and sizeHint. These are set in the
   *                           parent class, AbstracProjectItemView.
   *   @history 2018-07-25 Tracie Sucharski - Put sizeHint back since it was decided to put this
   *                           view split with the project view, so we don't want this as large
   *                           as the other views such as Footprint2DView or CubeDnView.
   */

class ControlHealthMonitorView : public AbstractProjectItemView {

  Q_OBJECT

  public:
    ControlHealthMonitorView(Directory *directory, QWidget *parent = 0);
    ~ControlHealthMonitorView();

    virtual QSize sizeHint() const;

    virtual QList<QAction *> permToolBarActions();
    virtual QList<QAction *> activeToolBarActions();
    virtual QList<QAction *> toolPadActions();

    ControlHealthMonitorWidget *controlHealthMonitorWidget();

  public slots:

  private slots:
    void openPointEditor(ControlPoint *point);
    void openImageEditor(QList<QString> serials);

  private:
    Directory *m_directory;

    QPointer<ControlHealthMonitorWidget> m_controlHealthMonitorWidget;

    ToolPad *m_toolPad;        //!< The tool pad
    QToolBar *m_permToolBar;   //!< The permanent tool bar
    QToolBar *m_activeToolBar; //!< The active tool bar

    QWidgetAction *m_activeToolBarAction; //!< Stores the active tool bar
  };
}

#endif // ControlHealthMonitorVIEW_H

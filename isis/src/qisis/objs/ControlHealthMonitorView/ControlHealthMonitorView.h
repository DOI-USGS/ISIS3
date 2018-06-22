#ifndef ControlHealthMonitorView_h
#define ControlHealthMonitorView_h
/**
 * @file
 * $Date$
 * $Revision$
 *
 *   Unless noted otherwise, the portions of Isis written by the USGS are
 *   public domain. See individual third-party library and package descriptions
 *   for intellectual property information, user agreements, and related
 *   information.
 *
 *   Although Isis has been used by the USGS, no warranty, expressed or
 *   implied, is made by the USGS as to the accuracy and functioning of such
 *   software and related material nor shall the fact of distribution
 *   constitute any such warranty, and no responsibility is assumed by the
 *   USGS in connection therewith.
 *
 *   For additional information, launch
 *   $ISISROOT/doc//documents/Disclaimers/Disclaimers.html
 *   in a browser or see the Privacy &amp; Disclaimers page on the Isis website,
 *   http://isis.astrogeology.usgs.gov, and the USGS privacy and disclaimers on
 *   http://www.usgs.gov/privacy.html.
 */
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
   */

class ControlHealthMonitorView : public AbstractProjectItemView {

  Q_OBJECT

  public:
    ControlHealthMonitorView(Directory *directory, QWidget *parent = 0);
    ~ControlHealthMonitorView();

    virtual QList<QAction *> permToolBarActions();
    virtual QList<QAction *> activeToolBarActions();
    virtual QList<QAction *> toolPadActions();

    ControlHealthMonitorWidget *controlHealthMonitorWidget();

    QSize sizeHint() const;

  public slots:

  private slots:
    void openPointEditor(ControlPoint *point);
    void openImageEditor();

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

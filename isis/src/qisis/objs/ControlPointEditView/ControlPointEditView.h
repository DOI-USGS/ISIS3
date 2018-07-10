#ifndef ControlPointEditView_h
#define ControlPointEditView_h
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
#include <QPushButton>

#include "AbstractProjectItemView.h"

namespace Isis {
  class Control;
  class ControlPointEditWidget;
  class Directory;
  class ProjectItem;
  class ToolPad;

  /**
   * View for editing a single ControlPoint
   *
   * @author 2016-04-06 Tracie Sucharski
   *
   * @internal
   *   @history 2016-09-30 Tracie Sucharski - Pass in directory to constructor, so that we can
   *                           query for shapes and other data from the project.
   *   @history 2018-05-28 Kaitlyn Lee - Since AbstractProjectItemView now inherits
   *                           from QMainWindow, I added a dummy central widget
   *                           and set its layout to QVBoxLayout. We used to set
   *                           the whole CnetEditorView widget's layout, now we only
   *                           set the central widget's layout.
   *   @history 2018-06-13 Kaitlyn Lee - Removed toolbars, since they are not needed.
   *   @history 2018-06-28 Kaitlyn Lee - Removed toolbars. When multiple views are open,
   *                           there is a possibility of getting ambiguous shortcut errors.
   *                           To counter this, we enable/disable actions. On default, a
   *                           view's actions are disabled. To enable the actions, move the
   *                           cursor over the view. When a user moves the cursor outside of
   *                           the view, the actions are disabled. Because this view uses
   *                           buttons instead of actions, overrode enableActions() and
   *                           disableActions() and added m_buttons to enable/disable buttons.
   *   @history 2018-07-09 Tracie Sucharski -  Remove setSizePolicy and sizeHint method which is now
   *                           taken care of in the parent class, AbstractProjectItemView.
   */

class ControlPointEditView : public AbstractProjectItemView {

  Q_OBJECT

  public:
    ControlPointEditView(Directory *directory, QWidget *parent = 0);
    ~ControlPointEditView();

    ControlPointEditWidget *controlPointEditWidget();

  private slots:
    void disableActions();
    void enableActions();

  private:
    QPointer<ControlPointEditWidget> m_controlPointEditWidget;
    QMap<Control *, ProjectItem *> m_controlItemMap;  //!<Maps control net to project item
    QList<QPushButton *> m_buttons;
  };
}

#endif // CONTROLPOINTEDITVIEW_H

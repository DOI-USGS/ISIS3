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
   */

class ControlPointEditView : public AbstractProjectItemView {

  Q_OBJECT

  public:
    ControlPointEditView(Directory *directory, QWidget *parent = 0);
    ~ControlPointEditView();

    ControlPointEditWidget *controlPointEditWidget();

//  setEditPoint(ControlPoint *editPoint);
//  createNewPoint(QString serialNumber, Latitude lat, Longitude lon);

  QSize sizeHint() const;

  private:
    void enterEvent(QEvent *event);
    void leaveEvent(QEvent *event);
    void disableActions();
    void enableActions();

    QPointer<ControlPointEditWidget> m_controlPointEditWidget;
    QMap<Control *, ProjectItem *> m_controlItemMap;  //!<Maps control net to project item
    QList<QPushButton *> m_buttons;
  };
}

#endif // CONTROLPOINTEDITVIEW_H

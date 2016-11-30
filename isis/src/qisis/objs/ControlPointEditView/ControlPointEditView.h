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
#include <QToolBar>
#include <QWidgetAction>

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
   */

class ControlPointEditView : public AbstractProjectItemView {

  Q_OBJECT

  public:
    ControlPointEditView(Directory *directory, QWidget *parent = 0);
    ~ControlPointEditView();

    virtual QList<QAction *> permToolBarActions();
    virtual QList<QAction *> activeToolBarActions();
    virtual QList<QAction *> toolPadActions();

    ControlPointEditWidget *controlPointEditWidget() const;

//  setEditPoint(ControlPoint *editPoint);
//  createNewPoint(QString serialNumber, Latitude lat, Longitude lon);

    QSize sizeHint() const;

  public slots:

  private slots:

  private:
    ControlPointEditWidget *m_controlPointEditWidget;
    QMap<Control *, ProjectItem *> m_controlItemMap;  //!<Maps control net to project item

    QToolBar *m_permToolBar; //!< The permanent tool bar
    QToolBar *m_activeToolBar; //!< The active tool bar
    ToolPad *m_toolPad; //!< The tool pad

    QWidgetAction *m_activeToolBarAction; //!< Stores the active tool bar
  };
}

#endif // CONTROLPOINTEDITVIEW_H

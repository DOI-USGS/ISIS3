#ifndef CnetEditorView_h
#define CnetEditorView_h

/** This is free and unencumbered software released into the public domain.

The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */

#include <QList>
#include <QMap>
#include <QPointer>
#include <QSize>

#include "AbstractProjectItemView.h"
#include "FileName.h"
#include "XmlStackedHandler.h"

class QAction;
class QToolBar;
class QWidgetAction;
class QXmlStreamWriter;

namespace Isis {
  class Control;
  class CnetEditorWidget;
  class Directory;
  class FileName;
  class Project;
  class ToolPad;
  class XmlStackedHandlerReader;
  class ProjectItemViewMenu;

  /**
   * Ipce view containing the CnetEditorWidget
   *
   * @author 2018-04-04 Tracie Sucharski
   *
   * @internal
   *    @history 2018-06-01 Kaitlyn Lee - Because AbstractProjectItemView now inherits from QMainWindow,
   *                            I added a dummy central widget and set its layout to the grid layout.
   *                            We used to set the whole CnetEditorView widget's layout, now we only
   *                            set the central widget's layout.
   *    @history 2018-06-05 Kaitlyn Lee - Added createMenus() and createToolBars(). The body of createMenus()
   *                            was moved from the constructor. createToolBars() was copied and edited
   *                            from CnetEditorWindow. Fixes #5416. Fixes #4988
   *    @history 2018-06-13 Kaitlyn Lee - Since views now inherit from QMainWindow, each individual
   *                            view has its own toolbar, so having getters that return toolbar
   *                            actions to fill the toolbar of the IpceMainWindow are unnecessary.
   *                            Removed methods that returned menu and toolbar actions.
   *    @history 2018-06-25 Kaitlyn Lee - When multiple views are open, there is a possibility of getting
   *                            ambiguous shortcut errors. To counter this, we enable/disable actions. Overrode
   *                            leaveEvent() to handle open menus causing a leave event. On default, a view's
   *                            actions are disabled. To enable the actions, move the cursor over the view.
   *                            When a user moves the cursor outside of the view, the actions are disabled.
   *   @history 2018-07-09 Tracie Sucharski - Serialize the objectName for this view so that the
   *                            view can be re-created with the same objectName for restoring the
   *                            project state. Qt's save/restoreState use the objectName. Remove
   *                            sizeHint method which is now taken care of in the parent class,
   *                            AbstractProjectItemView.
   */

class CnetEditorView : public AbstractProjectItemView {

  Q_OBJECT

  public:
    CnetEditorView(Directory *directory, Control *control, FileName configFile,
                   QWidget *parent = 0);
    ~CnetEditorView();

    CnetEditorWidget *cnetEditorWidget();
    Control *control();

    void load(XmlStackedHandlerReader *xmlReader);
    void save(QXmlStreamWriter &stream, Project *project, FileName newProjectRoot) const;

  private:
    void createToolBars();
    void createMenus();
    void leaveEvent(QEvent *event);


      /**
       * @author 2012-09-?? Steven Lambright
       *
       * @internal
       *   @history 2018-04-04 Tracie Sucharski - Implemented for CnetEditorView
       */
      class XmlHandler : public XmlStackedHandler {
        public:
          XmlHandler(CnetEditorView *cnetEditorView);
          ~XmlHandler();

          virtual bool startElement(const QString &namespaceURI, const QString &localName,
                                    const QString &qName, const QXmlAttributes &atts);
          virtual bool endElement(const QString &namespaceURI, const QString &localName,
                                  const QString &qName);

        private:
          Q_DISABLE_COPY(XmlHandler);

          CnetEditorView *m_cnetEditorView; //!< The view we are working with
      };

  private:
    QPointer<CnetEditorWidget> m_cnetEditorWidget;
    QPointer<Control> m_control;

    QToolBar *m_permToolBar; //!< The permanent tool bar
    ProjectItemViewMenu *m_tablesMenu; //!< View menu for storing actions

  };
}

#endif // CNETEDITORVIEW_H

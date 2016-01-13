#ifndef CubeDnView_h
#define CubeDnView_h
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

#include <QMdiSubWindow>
#include <QWidgetAction>

#include "AbstractProjectItemView.h"
#include "MdiCubeViewport.h"
#include "ProjectItemProxyModel.h"
#include "ToolPad.h"

namespace Isis {
  
  /**
   * View that displays cubes in a QView-like way. 
   *
   * @author Jeffrey Covington
   */
  class CubeDnView : public AbstractProjectItemView {

    Q_OBJECT

    public:
      CubeDnView(QWidget *parent=0);
      ~CubeDnView();

      virtual QList<QAction *> fileMenuActions();
      virtual QList<QAction *> projectMenuActions();
      virtual QList<QAction *> editMenuActions();
      virtual QList<QAction *> viewMenuActions();
      virtual QList<QAction *> settingsMenuActions();
      virtual QList<QAction *> helpMenuActions();

      virtual QList<QAction *> permToolBarActions();
      virtual QList<QAction *> activeToolBarActions();
      virtual QList<QAction *> toolPadActions();

      QSize sizeHint() const;

    public slots:
      void addItem(ProjectItem *item);
    
    private slots:
      void onCurrentChanged(const QModelIndex &current);
      void onCubeViewportActivated(MdiCubeViewport *);
      void onItemAdded(ProjectItem *item);
      void onCubeViewportAdded(MdiCubeViewport *viewport);
      void onCubeViewportDeleted(QObject *obj);

    private:
      Cube *workspaceActiveCube();
      void setWorkspaceActiveCube(Image *image);

      QMap<Cube *, ProjectItem *> m_cubeItemMap; //!< Maps cubes to their items
      Workspace *m_workspace; //!< The workspace

      QMenu *m_fileMenu; //!< File menu for storing actions
      QMenu *m_viewMenu; //!< View menu for storing actions
      QMenu *m_optionsMenu; //!< Options menu for storing actions
      QMenu *m_windowMenu; //!< Window menu for storing actions
      QMenu *m_helpMenu; //!< Help menu for storing actions

      QAction *m_separatorAction; //!< A separator action that is reused
      
      QToolBar *m_permToolBar; //!< A tool bar for storing actions
      QToolBar *m_activeToolBar; //!< A tool bar for storing actions
      ToolPad *m_toolPad; //!< A tool bar for storing actions

      QList<QAction *> m_permToolBarActions; //!< The permanent tool bar actions
      QWidgetAction *m_activeToolBarAction; //!< Widget of the active tool
      QList<QAction *> m_toolPadActions; //!< The tool pad actions
  };






}

#endif

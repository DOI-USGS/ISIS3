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
#include "ProjectItemTreeView.h"

#include <QEvent>
#include <QObject>
#include <QTreeView>
#include <QVBoxLayout>
#include <QWidget>

#include "ProjectItem.h"
#include "ProjectItemModel.h"

namespace Isis {
  /**
   * Constructs a ProjectItemTreeView.
   *
   * @param[in] parent (QWidget *) The parent widget.
   */
  ProjectItemTreeView::ProjectItemTreeView(QWidget *parent) : AbstractProjectItemView(parent) {
        
    m_treeView = new QTreeView(this);
    m_treeView->installEventFilter(this);
    setInternalModel( internalModel() );
    m_treeView->setDragEnabled(true);
    m_treeView->setAcceptDrops(false);
    m_treeView->setHeaderHidden(true);

    setAcceptDrops(true);
    
    QBoxLayout *layout = new QVBoxLayout;
    layout->addWidget(m_treeView);
    layout->setContentsMargins(0, 0, 0, 0);

    setLayout(layout);
  }

  
  /**
   * Default destructor.
   */
  ProjectItemTreeView::~ProjectItemTreeView() {}


  /**
   * Returns the tree view.
   *
   * @return @b QTreeView* The tree view.
   */
  QTreeView *ProjectItemTreeView::treeView() {
    return m_treeView;
  }

  
  /**
   * Sets the model so that the internal proxy model exactly matches the
   * source model.
   *
   * @param[in] model (ProjectItemModel *) The source model.
   */
  void ProjectItemTreeView::setInternalModel(ProjectItemModel *model) {
    disconnect(internalModel(), 0, this, 0);

    AbstractProjectItemView::setInternalModel(model);
    m_treeView->reset();
    m_treeView->setModel(model);
    m_treeView->setSelectionModel( model->selectionModel() );
    m_treeView->setSelectionMode(QAbstractItemView::ExtendedSelection);

    connect( model, SIGNAL( itemAdded(ProjectItem *) ),
             this, SLOT( onItemAdded(ProjectItem *) ) );
  }


  /**
   * Expands the parent project item in the tree view to show the added item.
   *
   * @param[in] item (ProjectItem *) The added project item.
   */
  void ProjectItemTreeView::onItemAdded(ProjectItem *item) {
    ProjectItem *parent = item->parent();
    if (!parent) {
      return;
    }
    if ( !parent->isImageList() && !parent->isControlList() ) {
      m_treeView->expand( parent->index() );
    }
  }
  
  
  /**
   * Filters out drag and drop events so that they are handled by the
   * ProjectItemTreeView.
   *
   * @param[in] watched (QObject *) The object being filtered.
   * @param[in] event (QEvent *) The event passed to the object.
   *
   * @return @b bool True if the event is filtered by the project item tree view
   */
  bool ProjectItemTreeView::eventFilter(QObject *watched, QEvent *event) {
    if (event->type() == QEvent::DragEnter) {
      return true;
    }
    else if (event->type() == QEvent::Drop) {
      return true;
    }

    return AbstractProjectItemView::eventFilter(watched, event);
  }
  
}

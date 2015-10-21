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

#include <QtGui>

namespace Isis {
  /**
   * Constructs a ProjectItemTreeView.
   *
   * @param[in] parent (QWidget *) The parent widget.
   */
  ProjectItemTreeView::ProjectItemTreeView(QWidget *parent) : AbstractProjectItemView(parent) {
        
    m_treeView = new QTreeView(this);
    m_treeView->installEventFilter(this);
    m_treeView->setModel( proxyModel() );
    m_treeView->setSelectionModel( proxyModel()->selectionModel() );
    m_treeView->setSelectionMode(QAbstractItemView::ExtendedSelection);

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
   * Sets the model so that the internal proxy model exactly matches the
   * source model.
   *
   * @param[in] model (ProjectItemModel *) The source model.
   */
  void ProjectItemTreeView::setSourceModel(ProjectItemModel *model) {
    proxyModel()->clear();
    proxyModel()->setSourceModel(model);
    for (int row=0; row < model->rowCount(); row++) {
      proxyModel()->addItem( model->item(row) );
    }
    connect( model, SIGNAL( itemAdded(ProjectItem *) ),
             proxyModel(), SLOT( addItem(ProjectItem *) ) );
    
  }


  /**
   * Filters out drag and drop events so that they are handled by the
   * ProjectItemTreeView.
   *
   * @param[in] watched (QObject *) The object being filtered.
   * @param[in] event (QEvent *) The event passed to the object.
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

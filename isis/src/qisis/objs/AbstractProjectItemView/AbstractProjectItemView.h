#ifndef AbstractProjectItemView_h
#define AbstractProjectItemView_h
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

#include <QItemSelectionModel>

#include "ProjectItem.h"
#include "ProjectItemModel.h"
#include "ProjectItemProxyModel.h"

namespace Isis {
  /**
   * AbstractProjectItemView is a base class for views of a
   * ProjectItemModel in Qt's model-view
   * framework. AbstractProjectItemView is not meant to be
   * instantiated directly. A view usually only shows items that have
   * been added to the view. The views contains an internal
   * ProjectItemProxyModel that represents the items appropriately for
   * the view.
   *
   * An AbstractProjectItemView may provide QActions for manipulating
   * the view. These actions can be accessed in different contexts
   * through toolBarActions(), menuActions(), and
   * contextMenuActions().
   *
   * When mime data is dropped on a view the view adds the selected
   * items from the source model to the view.
   *
   * Note that AbstractProjectItemView does not inherit from QAbstractItemView.
   *
   * @author Jeffrey Covington
   *
   */
  class AbstractProjectItemView : public QWidget {

    Q_OBJECT

    public:
      AbstractProjectItemView(QWidget *parent=0);

      virtual void setModel(ProjectItemModel *model);

      void dragEnterEvent(QDragEnterEvent *event);
      void dragMoveEvent(QDragMoveEvent *event);
      void dropEvent(QDropEvent *event);

      virtual QList<QAction *> toolBarActions();
      virtual QList<QAction *> menuActions();
      virtual QList<QAction *> contextMenuActions();

      virtual ProjectItem *currentItem();
      virtual QList<ProjectItem *> selectedItems();

    public slots:
      virtual void addItem(ProjectItem *item);
      virtual void addItems(QList<ProjectItem *> items);

    protected:
      ProjectItemProxyModel *proxyModel();
      void setProxyModel(ProjectItemProxyModel *proxyModel);

    private:
      ProjectItemProxyModel *m_proxyModel;
  };

}

#endif

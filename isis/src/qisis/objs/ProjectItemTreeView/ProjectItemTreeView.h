#ifndef ProjectItemTreeView_h
#define ProjectTreeView_h
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

#include <QTreeView>

#include "AbstractProjectItemView.h"
#include "ProjectItemProxyModel.h"

namespace Isis {
  /**
   * A ProjectItemTreeView displays items from a ProjectItemProxyModel
   * in a tree structure. The view can display the contents of the
   * model directly without adding items to the model using the
   * setSourceModel() method instead of setModel().
   *
   * @author Jeffrey Covington
   */
  class ProjectItemTreeView : public AbstractProjectItemView {

    Q_OBJECT

    public:
      ProjectItemTreeView(QWidget *parent=0);
      void setSourceModel(ProjectItemModel *model);

    protected:
      bool eventFilter(QObject *watched, QEvent *event);
      
    private:

      QTreeView *m_treeView;
  };
}

#endif

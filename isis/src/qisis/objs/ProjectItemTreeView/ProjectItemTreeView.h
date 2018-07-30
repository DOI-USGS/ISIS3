#ifndef ProjectItemTreeView_h
#define ProjectItemTreeView_h
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

#include "AbstractProjectItemView.h"

class QEvent;
class QTreeView;
class QWidget;

namespace Isis {

  class ProjectItem;
  class ProjectItemModel;

  /**
   * A ProjectItemTreeView displays items from a ProjectItemProxyModel
   * in a tree structure. The view can display the contents of the
   * model directly without adding items to the model using the
   * setInternalModel() method instead of setModel().
   *
   *
   * @author 2015-10-21 Jeffrey Covington
   *
   * @internal
   *   @history 2015-10-21 Jeffrey Covington - Original version.
   *   @history 2016-01-13 Jeffrey Covington - Added destructor and treeView() methods. Added
   *                           onItemAdded() slot. Replaced setSourceModel() with
   *                           setInternalModel() method.
   *   @history 2016-06-27 Ian Humphrey - Added documentation (treeView() and onItemAdded()),
   *                           checked coding standards. Fixes #4006.
   *   @history 2016-08-25 Adam Paquette - Updated documentation. Fixes #4299.
   *   @history 2016-12-01 Ian Humphrey - Updated #define header guard to match #ifndef pattern.
   *                           Resolves [-Wheader-guard] warnings for prog17 (clang).
   *   @history 2017-04-12 Tracie Sucharski - Turn off dragging on the treeView for now since it is
   *                           does not work and is causing errors.
   *   @history 2018-05-29 Summer Stapleton - updated the view to include a central widget and to
   *                           remove layout capacity. This change was made to adjust to parent
   *                           class now inheriting from QMainWindow instead of QWidget.
   *   @history 2018-07-12 Kaitlyn Lee - Changed the sizeHint to be calculated based on the deskTop
   *                           size, instead of being hard-coded. The percentages chosen allow for
   *                           2 CubeDnViews to be opened at once, since CubeDnView has an internal
   *                           size policy and cannot be made smaller. Changed the setSizePolicy to
   *                           minimum so that the tree does not expand when a view is closed. References #5433
   *   @history 2018-07-25 Tracie Sucharski - Changed vertical sizePolicy so that other widgets such
   *                           as JigsawRunWidget, ControlHealthMonitor can be split with the
   *                           Project view.
   */
  class ProjectItemTreeView : public AbstractProjectItemView {

    Q_OBJECT

    public:
      ProjectItemTreeView(QWidget *parent=0);
      ~ProjectItemTreeView();

      virtual QSize sizeHint() const;

      virtual void setInternalModel(ProjectItemModel *model);

      QTreeView *treeView();

    protected:
      bool eventFilter(QObject *watched, QEvent *event);

    private slots:
      void onItemAdded(ProjectItem *item);

    private:
      QTreeView *m_treeView; //!< The tree view (widget)
  };
}

#endif

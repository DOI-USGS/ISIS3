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

#include <QMainWindow>

class QAction;
class QDragEnterEvent;
class QWidget;
template <typename T> class QList;

namespace Isis {

  class ProjectItem;
  class ProjectItemModel;
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
   * @author 2015-10-21 Jeffrey Covington
   * @internal
   *   @history 2015-10-21 Jeffrey Covington - Original version.
   *   @history 2016-06-27 Ian Humphrey - Minor updates to documentation and coding standards.
   *                           Fixes #4004.
   *   @history 2016-07-28 Tracie Sucharski - Implemented removeItem and removeItems methods.
   *   @history 2016-08-25 Adam Paquette - Minor updates to documentation.
   *                           Fixes #4299.
   *   @history 2018-05-29 Tracie Sucharski & Summer Stapleton - updated to inherit from QMainWindow
   *                           instead of QWidget. This updates all views in the ipce main window
   *                           to be main windows themselves, changing from an mdi interface to an
   *                           sdi interface.
   *   @history 2018-05-30 Tracie Sucharski - Added the WindowFlag to set this as a Widget.
   *   @history 2018-06-15 Kaitlyn Lee - Removed methods returing toolbar and menu actions because each
   *                            individual has its own toolbar. These methods are not needed anymore.
   *   @History 2018-06-18 Summer Stapleton - Overloaded moveEvent and resizeEvent and added a
   *                           windowChangeEvent signal to allow project to recognize a new save
   *                           state. Fixes #5114
   */
  class AbstractProjectItemView : public QMainWindow {

    Q_OBJECT

    public:
      AbstractProjectItemView(QWidget *parent=0);

      virtual void setModel(ProjectItemModel *model);
      virtual ProjectItemModel *model();

      virtual void dragEnterEvent(QDragEnterEvent *event);
      virtual void dragMoveEvent(QDragMoveEvent *event);
      virtual void dropEvent(QDropEvent *event);

      virtual void moveEvent(QMoveEvent *event);
      virtual void resizeEvent(QResizeEvent *event);

      virtual QList<QAction *> contextMenuActions();

      virtual ProjectItem *currentItem();
      virtual QList<ProjectItem *> selectedItems();

      virtual ProjectItemModel *internalModel();
      virtual void setInternalModel(ProjectItemModel *model);
      
    signals:
      void windowChangeEvent(bool event);

    public slots:
      virtual void addItem(ProjectItem *item);
      virtual void addItems(QList<ProjectItem *> items);

      virtual void removeItem(ProjectItem *item);
      virtual void removeItems(QList<ProjectItem *> items);

    private:
      ProjectItemModel *m_internalModel; //!< The internal model used by the view
  };

}

#endif

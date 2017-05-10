#ifndef ProjectItemModel_h
#define ProjectItemModel_h
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

#include <QStandardItemModel>

class QItemSelection;
class QItemSelectionModel;
class QMimeData;
class QModelIndex;
class QString;
class QVariant;

namespace Isis {

  class ShapeList;
  class BundleSolutionInfo;
  class Control;
  class ControlList;
  class GuiCameraList;
  class ImageList;
  class Project;
  class ProjectItem;
  class TargetBodyList;

  /**
   * Provides access to data stored in a Project through Qt's model-view
   * framework. Items corresponding to data are organized in a tree structure.
   * Data can be accessed through a ProjectItem or through a QModelIndex. Views
   * associated with the model can access it either through the model directly
   * or through a ProjectItemProxyModel.
   *
   * Top-level items can be accessed through the item() method using the row
   * where the item is stored. Children of items can be accessed through the
   * child() method on the parent item. The data stored in an item can be
   * accessed as a QVariant using the data() method of the item or through the
   * various convenience methods in ProjectItem. Alternatively, the data in an
   * item can be accessed with its corresponding QModelIndex and the model's
   * data() method. It should be noted that the data() method in the model
   * and the data() method of ProjectItem require a Qt::ItemDataRole interpreted
   * as an int. Data stored in the model that correspond to Isis classes are
   * stored as Qt::UserRole + 1.
   *
   * Top-level items should be added to the model using the appendRow() or the
   * insertRow() method. To add child items to a parent item the appendRow()
   * or insertRow() methods of the parent item should be used.
   *
   * The model keeps track of selected items and the current item using an
   * internal QItemSelectionModel.
   *
   * Views that only need access to a subset of the items or the items organized
   * in a different way should use a ProjectItemProxyModel.
   *
   * @code
   * Image *image = new Image("example.cub");
   * ProjectItemModel *model = new ProjectItemModel();
   * ProjectItem *item = new ProjectItem(image);
   * model->appendRow(item);
   * @endcode
   *
   * @ingroup
   *
   * @author 2015-10-21 Jeffrey Covington
   *  
   * @internal 
   *   @history 2015-10-21 Jeffrey Covington - Original version.
   *   @history 2016-01-13 Jeffrey Covington - Added canDropMimeData() method.
   *   @history 2016-06-27 Ian Humphrey - Added documentation to canDropMimeData(), checked coding
   *                           standards. Fixes #4006.
   *   @history 2016-07-18 Tracie Sucharski - Added Project Item slots for adding shape models. 
   *   @history 2016-08-25 Adam Paquette - Updated documentation. Fixes #4299.
   *   @history 2017-04-17 Tracie Sucharski - Made changeds to allow project name to be edited from
   *                           the ProjectItemTree, by double-clicking on the project name.  This
   *                           functionality required the addition of the setData and flags methods.
   *                           The projectNameEdited signal is also emitted.  Fixes #2295
   *     @history 2017-05-04 J Bonn - Added FileItem to project tree. Fixes #4838.
   */
  class ProjectItemModel : public QStandardItemModel {

    Q_OBJECT

    public:
      ProjectItemModel(QObject *parent = 0);
      ~ProjectItemModel();

      QItemSelectionModel *selectionModel();

      ProjectItem *addProject(Project *project);

      ProjectItem *findItemData(const QVariant &data, int role = Qt::UserRole+1);

      virtual bool canDropMimeData(const QMimeData *data,
                                   Qt::DropAction action,
                                   int row, int column,
                                   const QModelIndex& parent) const;
      
      virtual void removeItem(ProjectItem *item);
      virtual void removeItems(QList<ProjectItem *> items);

      ProjectItem *currentItem();
      QList<ProjectItem *> selectedItems();

      void appendRow(ProjectItem *item);
      QModelIndex indexFromItem(const ProjectItem *item);
      void insertRow(int row, ProjectItem *item);
      ProjectItem *item(int row);
      ProjectItem *itemFromIndex(const QModelIndex &index);
      void setItem(int row, ProjectItem *item);
      ProjectItem *takeItem(int row);

      bool setData(const QModelIndex &index, const QVariant &value, int role);
      Qt::ItemFlags flags(const QModelIndex &index) const;

    signals:
      /**
       * This signal is emitted when a ProjectItem is added to the model.
       */
      void itemAdded(ProjectItem *);
      void itemRemoved(ProjectItem *);
      void projectNameEdited(QString);

    protected slots:
      void onSelectionChanged(const QItemSelection &selected, const QItemSelection &deselected);

    private slots:
      void onNameChanged(QString newName);
      void onBundleSolutionInfoAdded(BundleSolutionInfo *bundleSolutionInfo);
      void onImagesAdded(ImageList *images);
      void onShapesAdded(ShapeList *shapes);
      void onControlAdded(Control *control);
      void onControlListAdded(ControlList *controlList);
      void onTargetsAdded(TargetBodyList *targets);
      void onGuiCamerasAdded(GuiCameraList *cameras);
      void onRowsInserted(const QModelIndex &parent, int start, int end);
      void onRowsRemoved(const QModelIndex &parent, int start, int end);

    private:
      QItemSelectionModel *m_selectionModel; //!< The internal selection model.

  };

}

#endif


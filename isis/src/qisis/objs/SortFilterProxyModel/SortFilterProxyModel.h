#ifndef SortFilterProxyModel_h
#define SortFilterProxyModel_h

/** This is free and unencumbered software released into the public domain.

The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */

#include <QList>
#include <QIdentityProxyModel>
#include <QModelIndex>
#include <QPersistentModelIndex>
#include <QSortFilterProxyModel>

class QAbstractProxyModel;
class QObject;
class QStandardItem;

class QVariant;

namespace Isis {

class ProjectItem;
class ProjectItemModel;

  /**
   *
   * @author 2018-06-18 Tyler Wilson
   *
   * @internal
   *   @history 2018-06-18 Tyler Wilson - Original version. A proxy class for filtering data
   *   within the JigsawSetupDialog Bundle Observation Solve Settings (BOSS) tab.
   */

    class SortFilterProxyModel : public QSortFilterProxyModel  {
    Q_OBJECT

    public:
      explicit SortFilterProxyModel(QObject *parent = 0);

      //QModelIndex mapFromSource(const QModelIndex &sourceIndex) const Q_DECL_OVERRIDE;
      //QModelIndex mapToSource(const QModelIndex &proxyIndex) const Q_DECL_OVERRIDE;

      //void setSourceModel(ProjectItemModel *newSourceModel) Q_DECL_OVERRIDE;
      void setSourceModel(ProjectItemModel *newSourceModel);

      bool setRoot(const QStandardItem *item);

      void setSelectedItems(QList<ProjectItem*> selected);


     protected:
       bool filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const override;

    private:
      ProjectItemModel * baseModel;
      QList<QModelIndex> selectedIndices;
      QList<int> selectedIndexRows;
      QPersistentModelIndex m_root;
      QModelIndex root;


  };

};


#endif

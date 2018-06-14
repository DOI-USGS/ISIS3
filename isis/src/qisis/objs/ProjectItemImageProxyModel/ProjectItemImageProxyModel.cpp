#include "ProjectItemImageProxyModel.h"

#include <QModelIndex>
#include <QObject>
#include <QSortFilterProxyModel>

#include "ProjectItem.h"

namespace Isis {

  ProjectItemImageProxyModel::ProjectItemImageProxyModel(QObject *parent)
    : QSortFilterProxyModel(parent) {
  }


  bool ProjectItemImageProxyModel::filterAcceptsColumn(int sourceColumn,
                                                       const QModelIndex &sourceParent) const {

  }


  bool ProjectItemImageProxyModel::filterAcceptsRow(int sourceRow,
                                                    const QModelIndex &sourceParent) const {
    ProjectItemModel *source = static_cast<ProjectItemModel *>(sourceModel());
    QModelIndex modelIndex = source->index(sourceRow, 0, sourceParent);
    ProjectItem *item = source->itemFromIndex(modelIndex);
    if (item->isImage() || item->isImageList()) {
        return true;
    }
    return false;
  }

}

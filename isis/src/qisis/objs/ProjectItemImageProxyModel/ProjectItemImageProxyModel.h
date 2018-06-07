#ifndef ProjectItemImageProxyModel_h
#define ProjectItemImageProxyModel_h

#include <QSortFilterProxyModel>

namespace Isis {

  class ProjectItemImageProxyModel : public QSortFilterProxyModel {
    public:
      ProjectItemImageProxyModel(QObject *parent = NULL);
      ~ProjectItemImageProxyModel();

    protected:
      virtual bool filterAcceptsColumn(int sourceColumn,
                                     const QModelIndex &sourceParent) const override;
      virtual bool filterAcceptsRow(int sourceRow,
                                  const QModelIndex &sourceParent) const override;
  };
}

#endif

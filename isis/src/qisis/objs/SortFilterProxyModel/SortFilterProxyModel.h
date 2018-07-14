
#ifndef SortFilterProxyModel_h
#define SortFilterProxyModel_h

/**
 * @file
 * $Date: 2018/06/88 16:40:33 $ $Revision: 1.0 $
 *
 *  Unless noted otherwise, the portions of Isis written by the USGS are public domain. See
 *  individual third-party library and package descriptions for intellectual property information,
 *  user agreements, and related information.
 *
 *  Although Isis has been used by the USGS, no warranty, expressed or implied, is made by the
 *  USGS as to the accuracy and functioning of such software and related material nor shall the
 *  fact of distribution constitute any such warranty, and no responsibility is assumed by the
 *  USGS in connection therewith.
 *
 *  For additional information, launch $ISISROOT/doc//documents/Disclaimers/Disclaimers.html
 *  in a browser or see the Privacy &amp; Disclaimers page on the Isis website,
 *  http://isis.astrogeology.usgs.gov, and the USGS privacy and disclaimers on
 *  http://www.usgs.gov/privacy.html.
 */

#include <QList>
#include <QIdentityProxyModel>
#include <QModelIndex>
#include <QPersistentModelIndex>
#include <QSharedPointer>
#include <QSortFilterProxyModel>
#include <QUuid>

#include "BundleObservationSolveSettings.h"

class QAbstractProxyModel;
class QObject;
class QStandardItem;


class QVariant;

namespace Isis {

  /**
   * @brief A proxy class for filtering data within the JigsawSetupDialog
   *    Bundle Observation Solve Settings (BOSS) tab.
   *   
   * @ingroup
   *
   * @author 2018-06-18 Tyler Wilson
   *
   * @internal
   *   @history 2018-06-18 Tyler Wilson - Original version.
   */
  

   class BundleSettings;
   class Image;
   class ProjectItem;   
   class ProjectItemModel;
 
    class SortFilterProxyModel : public QSortFilterProxyModel  {
    Q_OBJECT

    public:
      explicit SortFilterProxyModel(QObject *parent = 0);

      //QModelIndex mapFromSource(const QModelIndex &sourceIndex) const Q_DECL_OVERRIDE;

      bool lessThan(const QModelIndex &source_left, const QModelIndex &source_right) const Q_DECL_OVERRIDE;
      QModelIndex mapToSource(const QModelIndex &proxyIndex) const Q_DECL_OVERRIDE;
      QVariant headerData(int section, Qt::Orientation orientation, int role) const Q_DECL_OVERRIDE;
      QModelIndex parent(const QModelIndex & child) const Q_DECL_OVERRIDE;
      QModelIndex index(int row, int column,const QModelIndex &parent=QModelIndex() ) const Q_DECL_OVERRIDE;
      int columnCount(const QModelIndex & parent = QModelIndex()) const Q_DECL_OVERRIDE;
      QVariant data(const QModelIndex & ix, int role = Qt::DisplayRole) const Q_DECL_OVERRIDE;
      bool setData(const QModelIndex & ix, const QVariant & value, int role = Qt::EditRole) const;
      void setSourceModel(ProjectItemModel *newSourceModel);

      bool setRoot(const QStandardItem *item);

      void setSelectedItems(QList<ProjectItem*> selected, QSharedPointer<BundleSettings> bundleQSP);


     protected:
       bool filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const Q_DECL_OVERRIDE;

    private:
      QMap<QModelIndex,QPair<QString,int> > images;
      QMap<int,Image *> imagePointers;
      ProjectItemModel * baseModel;
      QList<QModelIndex> selectedIndices;





      QList<BundleObservationSolveSettings> m_bossList;

      QMap<QUuid *,QSet<QString> > m_bossMap;
      QPersistentModelIndex m_root;
      QModelIndex root;


  };

};


#endif

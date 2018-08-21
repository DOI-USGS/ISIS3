
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

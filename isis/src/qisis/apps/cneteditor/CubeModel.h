#ifndef CubeModel_H
#define CubeModel_H

#include <QAbstractItemModel>


class QVariant;
class QModelIndex;

template< class A, class B > class QMap;
template< class T > class QVector;
template< typename A, typename B > class QHash;

namespace Isis
{
  class ControlNet;
}


namespace Qisis
{

  class CubeModel : public QAbstractItemModel
  {
      Q_OBJECT
    
    public:
      CubeModel(Isis::ControlNet* cNet, QObject * parent = 0);
      virtual ~CubeModel();

      QVariant data(const QModelIndex & index, int role) const;
      QModelIndex index(int row, int column,
          const QModelIndex & parent = QModelIndex()) const;

      Qt::ItemFlags flags(const QModelIndex &index) const;
      QModelIndex parent(const QModelIndex & index) const;
      int rowCount(const QModelIndex &parent = QModelIndex()) const;
      int columnCount(const QModelIndex &parent = QModelIndex()) const;
      
      void printCubeStructure();

    private:
      void populateCubeStructure();

    private:
      Isis::ControlNet* controlNet;
      QMap< QString, QVector< int > >* cubeStructure;

      //! Used to get an index from a cube serial number
      QHash< QString, qlonglong > * cubeIdToIndexHash;

      //! Used to get a cube serial number from an index
      QHash< qlonglong, QString > * cubeIndexToIdHash;
      
      
  };

}

#endif


#include "CubeModel.h"

#include <iostream>

#include <QAbstractItemModel>
#include <QMap>
#include <QModelIndex>
#include <QString>
#include <QVariant>
#include <QVector>

#include "ControlMeasure.h"
#include "ControlNet.h"
#include "ControlPoint.h"

using namespace Isis;
using std::cerr;

namespace Qisis
{


  CubeModel::CubeModel(ControlNet* cNet, QObject * parent) :
      QAbstractItemModel(parent), controlNet(cNet), cubeStructure(NULL)
  {
    // cubeIdToIndexHash provides a way of assigning unique sequential indices
    // to all of the Cube Serial numbers in the ControlNet, while
    // cubeIndexToIdHash prvides a way to get the Cube Serial numbers back.
    cubeIdToIndexHash = new QHash< QString, qlonglong >();
    cubeIndexToIdHash = new QHash< qlonglong, QString >();
    cubeIdToIndexHash->reserve(controlNet->Size() / 5);
    cubeIndexToIdHash->reserve(controlNet->Size() / 5);

    populateCubeStructure();
  }


  CubeModel::~CubeModel()
  {
    if (cubeIdToIndexHash)
    {
      delete cubeIdToIndexHash;
      cubeIdToIndexHash = NULL;
    }

    if (cubeIndexToIdHash)
    {
      delete cubeIndexToIdHash;
      cubeIndexToIdHash = NULL;
    }
  }


  void CubeModel::populateCubeStructure()
  {
    int cubeIndex = -1;
    cubeStructure = new QMap< QString, QVector< int > >;

    for (int cpIndex = 0; cpIndex < controlNet->Size(); cpIndex++) 
    {
      // use a reference for the current ControlPoint for clearity
      const ControlPoint & curCtrlPoint = (*controlNet)[cpIndex];

      for (int cmIndex = 0; cmIndex < curCtrlPoint.Size(); cmIndex++) 
      {
        // get current cube's serial number and hash if new
        QString curCube = curCtrlPoint[cmIndex].GetCubeSerialNumber();

        if (!cubeStructure->contains(curCube)) 
        {
          QVector< int > newPointList;
          newPointList.append(cpIndex);
          cubeStructure->insert(curCube, newPointList);
          cubeIdToIndexHash->insert(curCube, ++cubeIndex);
          cubeIndexToIdHash->insert(cubeIndex, curCube);
        }
        else
        {
          (*cubeStructure)[curCube].append(cpIndex);
        }
      } // of for all measures in cp
    } // of for all ControlPoints in net
  }


  QVariant CubeModel::data(const QModelIndex & index, int role) const
  {
    if (!index.isValid())
      return QVariant();

    if (index.column() != 0)
      return QVariant();

    if (role != Qt::DisplayRole)
       return QVariant();

    // is a point
    if (index.parent().isValid())
    {
      int parentsRow = index.parent().row();
      QString cubeId = cubeIndexToIdHash->value(parentsRow);
      int pointIndex = cubeStructure->value(cubeId)[index.row()];

      QString id = (*controlNet)[pointIndex].Id();
      return QVariant(id);
    }
    else // is a cube
    {
      return cubeIndexToIdHash->value(index.row());
    }
  }
  
  
  int CubeModel::rowCount(const QModelIndex & parent) const
  {
    int rowCount = -1;
    
    // is a control Point
    if (parent.isValid())
    {
      QString cubeId = parent.data().toString();
      rowCount = cubeStructure->value(cubeId).size();
    }
    else // is a cube
    {
      rowCount = cubeStructure->size();
    }
    
    return rowCount;
  }
  
  
  int CubeModel::columnCount(const QModelIndex & parent) const
  {
    return 1;
  }
  
  
  Qt::ItemFlags CubeModel::flags(const QModelIndex &index) const
  {
    if (index.isValid())
      return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
    else
      return 0;
  }
  
  
  QModelIndex CubeModel::parent(const QModelIndex & index) const
  {
    if (index.isValid())
    {
      void * ip = index.internalPointer();
      if (!ip)
      {
        /*
        QString * cubeId = (QString *) ip;
        int row = (*cubeIdtoIndexHash)[*cubeId];
        */
        qlonglong id = (qlonglong) ip;
        if (id != -1)
          return createIndex((int) id, 0, 0);
      }
    }
    
    return QModelIndex();
  }
  
  
  QModelIndex CubeModel::index(int row, int column,
      const QModelIndex & parent) const
  {
    if (!parent.isValid())
    {
/*      QHash< int, QString > * mutableHash =
          const_cast< QHash< int, QString > >(cubeIndexToIdHash);
      QString * cubeId = &((*mutableHash)[row]);
*/
      return createIndex(row, column, row);
    }
    else
    {
      return createIndex(row, column, -1);
    }
  }
  
  
  void CubeModel::printCubeStructure()
  {
    QMapIterator< QString, QVector< int > > i(*cubeStructure);
    while (i.hasNext())
    {
      i.next();
      cerr << qPrintable(i.key()) << "\n";
    }
  }
  
}


















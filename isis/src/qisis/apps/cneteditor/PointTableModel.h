#ifndef PointTableModel_H
#define PointTableModel_H

#include <QAbstractTableModel>

template< class T > class QList;

class QModelIndex;

namespace Isis
{
  class ControlPoint;

  class PointTableModel : public QAbstractTableModel
  {
      Q_OBJECT

    public:
      // If a column is added or removed then make sure you also update
      // the COLS constant that immediately follows this enum.
      enum Column
      {
        Id = 0,
        ChooserName = 1,
        DateTime = 2,
        EditLock = 3,
        Ignored = 4,
        RefIndex = 5,
        Reference = 6,
        SPLat = 7,
        SPLon = 8,
        SPRadius = 9,
        AprioriSPLat = 10,
        AprioriSPLon = 11,
        AprioriSPRadius = 12,
        AprioriSPSource = 13,
        AprioriSPSourceFile = 14,
        AprioriRadiusSource = 15,
        AprioriRadiusSourceFile = 16,
        JigsawRejected = 17,
      };
      static const int COLS = 18;


    public:
      explicit PointTableModel(QObject * parent = 0);
      virtual ~PointTableModel();

      void setPoints(QList< ControlPoint * > newPoints);
      ControlPoint * getPoint(int row) const;

      int rowCount(const QModelIndex & parent) const;
      int columnCount(const QModelIndex & parent) const;
      QVariant data(const QModelIndex & index, int role) const;
      QVariant headerData(int column, Qt::Orientation orientation,
          int role) const;
      Qt::ItemFlags flags(const QModelIndex & index) const;
      bool setData(const QModelIndex & index,
          const QVariant & value, int role = Qt::EditRole);
      bool insertRows(int position, int rows,
          const QModelIndex & index = QModelIndex());
      bool removeRows(int position, int rows,
          const QModelIndex & index = QModelIndex());


    private:
      PointTableModel(const PointTableModel &);
      const PointTableModel & operator=(PointTableModel);


    private:
      bool validateRowColumn(int row, int column, bool checkPoint = true) const;
      double catchNULL(QString) const;
      QString catchNULL(double) const;


    private:
      QList< ControlPoint * > * points;
  };
}

#endif

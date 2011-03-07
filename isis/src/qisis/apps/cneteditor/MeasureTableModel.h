#ifndef MeasureTableModel_H
#define MeasureTableModel_H

#include <QAbstractTableModel>

template< class T > class QList;

class QModelIndex;

namespace Isis
{
  class ControlMeasure;

  class MeasureTableModel : public QAbstractTableModel
  {
      Q_OBJECT
      
    public:
      // If a column is added or removed then make sure you also update
      // the COLS constant that immediately follows this enum.
      enum Column
      {
        Sample = 0,
        Line = 1,
        EditLock = 2,
        Ignored = 3,
        Type = 4,
        Eccentricity = 5,
        GoodnessOfFit = 6,
        MinPixelZScore = 7,
        MaxPixelZScore = 8,
        PixelShift = 9,
        AprioriSample = 10,
        AprioriLine = 11,
        Diameter = 12,
        FocalPlaneMeasuredX = 13,
        FocalPlaneMeasuredY = 14,
        FocalPlaneComputedX = 15,
        FocalPlaneComputedY = 16,
        JigsawRejected = 17,
        ResidualSample = 18,
        ResidualLine = 19,
        ResidualMagnitude = 20
      };
      static const int COLS = 21;
      

    public:
      explicit MeasureTableModel(QObject * parent = 0);
      virtual ~MeasureTableModel();

      void setMeasure(ControlMeasure * measure, int row);
      void setMeasure(ControlMeasure * measure);
      ControlMeasure * getMeasure(int row) const;

      int rowCount(const QModelIndex & parent) const;
      int columnCount(const QModelIndex & parent) const;
      QVariant data(const QModelIndex & index, int role) const;
      QVariant headerData(int section, Qt::Orientation orientation,
          int role) const;
      Qt::ItemFlags flags(const QModelIndex & index) const;
      bool setData(const QModelIndex & index,
          const QVariant & value, int role = Qt::EditRole);
      bool insertRows(int position, int rows,
          const QModelIndex & index = QModelIndex());
      bool removeRows(int position, int rows,
          const QModelIndex & index = QModelIndex());


    private:
      MeasureTableModel(const MeasureTableModel &);
      const MeasureTableModel & operator=(MeasureTableModel);


    private:
      bool validateRowColumn(int row, int column,
          bool checkMeasure = true) const;
      double catchNULL(QString) const;
      QString catchNULL(double) const;


    private:
      QList< ControlMeasure * > * measures;
  };
}

#endif

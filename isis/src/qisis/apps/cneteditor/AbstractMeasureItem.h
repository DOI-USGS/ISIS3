#ifndef AbstractMeasureItem_H
#define AbstractMeasureItem_H


#include "AbstractTreeItem.h"


class QString;


namespace Isis
{
  class ControlMeasure;
  class CnetTableColumnList;

  class AbstractMeasureItem : public virtual AbstractTreeItem
  {
    public:
      // If a column is added or removed then make sure you also update
      // the COLS constant that immediately follows this enum.
      enum Column
      {
        PointId = 0,
        CubeSerialNumber = 1,
        Sample = 2,
        Line = 3,
        EditLock = 4,
        Ignored = 5,
        Type = 6,
        Eccentricity = 7,
        GoodnessOfFit = 8,
        MinPixelZScore = 9,
        MaxPixelZScore = 10,
        SampleShift = 11,
        LineShift = 12,
        SampleSigma = 13,
        LineSigma = 14,
        APrioriSample = 15,
        APrioriLine = 16,
        Diameter = 17,
        JigsawRejected = 18,
        ResidualSample = 19,
        ResidualLine = 20,
        ResidualMagnitude = 21
      };
      static const int COLS = 22;

      static QString getColumnName(Column);
      static Column getColumn(QString);
      static CnetTableColumnList createColumns();


    public:
      AbstractMeasureItem(ControlMeasure * cm, int avgCharWidth,
          AbstractTreeItem * parent = 0);
      virtual ~AbstractMeasureItem();

      QString getData() const;
      QString getData(QString columnTitle) const;
      void setData(QString columnTitle, QString newData);
      void deleteSource();
      InternalPointerType getPointerType() const;
      void * getPointer() const;
      bool hasMeasure(ControlMeasure *) const;


    private: // disable copying of this class
      AbstractMeasureItem(const AbstractMeasureItem & other);
      const AbstractMeasureItem & operator=(const AbstractMeasureItem & other);

      static void setLogData(ControlMeasure *, int, const QString &);



    private:
      ControlMeasure * measure;
  };
}

#endif

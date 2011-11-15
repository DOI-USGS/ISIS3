#ifndef AbstractPointItem_H
#define AbstractPointItem_H

#include "AbstractTreeItem.h"


class QString;
class QVariant;


namespace Isis
{
  class ControlPoint;
  class Distance;
  class SurfacePoint;

  namespace CnetViz
  {
    class TableColumnList;
    
    class AbstractPointItem : public virtual AbstractTreeItem
    {
        Q_OBJECT

      public:
        // If a column is added or removed then make sure you also update
        // the COLS constant that immediately follows this enum.
        enum Column
        {
          Id = 0,
          PointType = 1,
          ChooserName = 2,
          DateTime = 3,
          EditLock = 4,
          Ignored = 5,
          Reference = 6,
          AdjustedSPLat = 7,
          AdjustedSPLon = 8,
          AdjustedSPRadius = 9,
          AdjustedSPLatSigma = 10,
          AdjustedSPLonSigma = 11,
          AdjustedSPRadiusSigma = 12,
          APrioriSPLat = 13,
          APrioriSPLon = 14,
          APrioriSPRadius = 15,
          APrioriSPLatSigma = 16,
          APrioriSPLonSigma = 17,
          APrioriSPRadiusSigma = 18,
          APrioriSPSource = 19,
          APrioriSPSourceFile = 20,
          APrioriRadiusSource = 21,
          APrioriRadiusSourceFile = 22,
          JigsawRejected = 23
        };
        static const int COLS = 24;

        static QString getColumnName(Column);
        static Column getColumn(QString);
        static TableColumnList * createColumns();


      public:
        AbstractPointItem(ControlPoint * cp, int avgCharWidth,
            AbstractTreeItem * parent = 0);
        virtual ~AbstractPointItem();

        QVariant getData() const;
        QVariant getData(QString columnTitle) const;
        void setData(QString const & columnTitle, QString const & newData);
        void deleteSource();
        InternalPointerType getPointerType() const;
        void * getPointer() const;
        bool hasPoint(ControlPoint *) const;


      private slots:
        void sourceDeleted();


      private:
        AbstractPointItem(const AbstractPointItem & other);
        const AbstractPointItem & operator=(const AbstractPointItem & other);

        SurfacePoint prepareSigmas(Distance newSigma, SurfacePoint surfacePoint);


      private:
        ControlPoint * point;
    };
  }
}

#endif

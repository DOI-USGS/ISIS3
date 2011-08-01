#ifndef AbstractPointItem_H
#define AbstractPointItem_H

#include "AbstractTreeItem.h"


class QString;


namespace Isis
{
  class CnetTableColumnList;
  class ControlPoint;

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
        APrioriSPLat = 10,
        APrioriSPLon = 11,
        APrioriSPRadius = 12,
        APrioriSPSource = 13,
        APrioriSPSourceFile = 14,
        APrioriRadiusSource = 15,
        APrioriRadiusSourceFile = 16,
        JigsawRejected = 17
      };
      static const int COLS = 18;

      static QString getColumnName(Column);
      static Column getColumn(QString);
      static CnetTableColumnList createColumns();


    public:
      AbstractPointItem(Isis::ControlPoint * cp, int avgCharWidth,
          AbstractTreeItem * parent = 0);
      virtual ~AbstractPointItem();

      QString getData() const;
      QString getData(QString columnTitle) const;
      void setData(QString const & columnTitle, QString const & newData);
      void deleteSource();
      InternalPointerType getPointerType() const;
      void * getPointer() const;
      bool hasPoint(ControlPoint *) const;


    private slots:
      void sourceDeleted();


    private: // disable copying of this class
      AbstractPointItem(const AbstractPointItem & other);
      const AbstractPointItem & operator=(const AbstractPointItem & other);


    private:
      ControlPoint * point;
  };
}

#endif

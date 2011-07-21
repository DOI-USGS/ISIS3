#ifndef CnetPointTableModel_H
#define CnetPointTableModel_H


#include <QAbstractTableModel>


template< class T > class QList;


namespace Isis
{
  class AbstractTreeItem;
  class ControlPoint;
  class TreeModel;

  class CnetPointTableModel : public QObject
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
        JigsawRejected = 17,
      };
      static const int COLS = 18;


    public:
      explicit CnetPointTableModel(TreeModel * dataModel);
      virtual ~CnetPointTableModel();

      static QString getColName(Column);
      int columnCount() const;
      
      QList< AbstractTreeItem * > getItems(int, int) const;
      QList< AbstractTreeItem * > getItems(AbstractTreeItem *,
          AbstractTreeItem *) const;
      QList< AbstractTreeItem * > getSelectedItems() const;


    private:
      CnetPointTableModel(const CnetPointTableModel &);
      CnetPointTableModel & operator=(CnetPointTableModel);


    private:
      void nullify();
      double catchNULL(QString) const;
      QString catchNULL(double) const;


    private:
      TreeModel * dataModel;
  };
}

#endif

#ifndef AbstractMeasureItem_H
#define AbstractMeasureItem_H


#include "AbstractTreeItem.h"


class QString;
class QVariant;


namespace Isis {
  class ControlMeasure;

  namespace CnetViz {
    class TableColumnList;

    /**
     * @brief Base class for a measure item in the tree
     *
     * This class represents a measure item in the tree. This is generally
     * visualized as a serial number. This has columns for compatibility with
     * the table models.
     *
     * @author ????-??-?? Eric Hyer
     *
     * @internal
     *   @history 2012-09-28 Kimberly Oyama - Changed member variables to be prefixed with "m_".
     */
    class AbstractMeasureItem : public virtual AbstractTreeItem {
      public:
        // If a column is added or removed then make sure you also update
        // the COLS constant that immediately follows this enum.
        enum Column {
          PointId = 0,
          ImageId = 1,
          Sample = 2,
          Line = 3,
          EditLock = 4,
          Ignored = 5,
          Reference = 6,
          Type = 7,
          Obsolete_Eccentricity = 8,
          GoodnessOfFit = 9,
          MinPixelZScore = 10,
          MaxPixelZScore = 11,
          SampleShift = 12,
          LineShift = 13,
          SampleSigma = 14,
          LineSigma = 15,
          APrioriSample = 16,
          APrioriLine = 17,
          Diameter = 18,
          JigsawRejected = 19,
          ResidualSample = 20,
          ResidualLine = 21,
          ResidualMagnitude = 22
        };
        static const int COLS = 23;

        static QString getColumnName(Column);
        static Column getColumn(QString);
        static TableColumnList *createColumns();


      public:
        AbstractMeasureItem(ControlMeasure *cm, int avgCharWidth,
            AbstractTreeItem *parent = 0);
        virtual ~AbstractMeasureItem();

        virtual QVariant getData() const;
        virtual QVariant getData(QString columnTitle) const;
        virtual void setData(QString const &columnTitle,
            QString const &newData);
        bool isDataEditable(QString columnTitle) const;
        virtual void deleteSource();
        virtual InternalPointerType getPointerType() const;
        virtual void *getPointer() const;
        virtual bool hasMeasure(ControlMeasure *) const;


      protected:
        virtual void sourceDeleted();


      private: // disable copying of this class
        AbstractMeasureItem(const AbstractMeasureItem &other);
        const AbstractMeasureItem &operator=(
          const AbstractMeasureItem &other);


      private:
        static void setLogData(ControlMeasure *, int, const QString &);


      private:
        ControlMeasure *m_measure;
    };
  }
}

#endif

#ifndef AbstractPointItem_H
#define AbstractPointItem_H

/** This is free and unencumbered software released into the public domain.

The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */

#include "AbstractTreeItem.h"


class QString;
class QVariant;


namespace Isis {
  class ControlPoint;
  class Distance;
  class Latitude;
  class Longitude;
  class SurfacePoint;
  class TableColumnList;

  /**
   * @brief Base class for a point item in the tree
   *
   * This class represents a point item in the tree. This is generally
   * visualized as a point id. This has columns for compatibility with the
   * table models.
   *
   * @author ????-??-?? Eric Hyer
   *
   * @internal
   *   @history 2012-09-28 Kimberly Oyama - Changed member variables to be prefixed with "m_".
   *   @history 2017-07-25 Summer Stapleton - Removed the CnetViz namespace. Fixes #5054.
   */
  class AbstractPointItem : public virtual AbstractTreeItem {
    public:
      // If a column is added or removed then make sure you also update
      // the COLS constant that immediately follows this enum.
      enum Column {
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
      static TableColumnList *createColumns();


    public:
      AbstractPointItem(ControlPoint *cp, int avgCharWidth,
          AbstractTreeItem *parent = 0);
      virtual ~AbstractPointItem();

      QVariant getData() const;
      QVariant getData(QString columnTitle) const;
      void setData(QString const &columnTitle, QString const &newData);
      bool isDataEditable(QString columnTitle) const;
      void deleteSource();
      InternalPointerType getPointerType() const;
      void *getPointer() const;
      bool hasPoint(ControlPoint *) const;


    protected:
      virtual void sourceDeleted();


    private:
      AbstractPointItem(const AbstractPointItem &other);
      const AbstractPointItem &operator=(const AbstractPointItem &other);

      SurfacePoint prepareSigmas(Distance, SurfacePoint);
      SurfacePoint prepareSurfacePoint(Latitude, SurfacePoint);
      SurfacePoint prepareSurfacePoint(Longitude, SurfacePoint);
      SurfacePoint prepareSurfacePoint(Distance, SurfacePoint);
      SurfacePoint prepareSurfacePoint(SurfacePoint);


    private:
      ControlPoint *m_point;
  };
}

#endif

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
   *   @history 2019-07-26 Ken Edmundson - Modifications to support display/edit of control point
   *                           coordinates/sigmas in either Lat, Lon, Radius or XYZ.
   *                           1) Made the column headers in the enum "Column" generic to refer to
   *                              any point coordinate definition, e.g., AdjustedSPCoord1 instead of
   *                              AdjustedSPLatitude.
   *                           2) Added static method resetColumnHeaders() which feels pretty kludgy
   *                              right now and will likely need to be modified.
   *                           3) Added prepareXYZSigmas() method.
   *                           4) Modified static methods getColumnName(), createColumns() and
   *                              methods getData(QString columnTitle),
   *                              setData(QString const &columnTitle, QString const &newData).
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
          AdjustedSPCoord1 = 7,
          AdjustedSPCoord2 = 8,
          AdjustedSPCoord3 = 9,
          AdjustedSPCoord1Sigma = 10,
          AdjustedSPCoord2Sigma = 11,
          AdjustedSPCoord3Sigma = 12,
          APrioriSPCoord1 = 13,
          APrioriSPCoord2 = 14,
          APrioriSPCoord3 = 15,
          APrioriSPCoord1Sigma = 16,
          APrioriSPCoord2Sigma = 17,
          APrioriSPCoord3Sigma = 18,
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
      static void resetColumnHeaders(TableColumnList *columns);


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
      SurfacePoint prepareXYZSigmas(Distance, SurfacePoint);
      SurfacePoint prepareSurfacePoint(Latitude, SurfacePoint);
      SurfacePoint prepareSurfacePoint(Longitude, SurfacePoint);
      SurfacePoint prepareSurfacePoint(Distance, SurfacePoint);
      SurfacePoint prepareSurfacePoint(SurfacePoint);


    private:
      ControlPoint *m_point;
  };
}

#endif

#include "IsisDebug.h"

#include "PointTableModel.h"

#include <iostream>

#include <QList>

#include "ControlPoint.h"
#include "Distance.h"
#include "iException.h"
#include "iString.h"
#include "Latitude.h"
#include "Longitude.h"
#include "SpecialPixel.h"


using std::cerr;

namespace Isis
{
  PointTableModel::PointTableModel(QObject * parent) : QAbstractTableModel(
      parent)
  {
    points = NULL;
    points = new QList< ControlPoint * >;
    points->append(NULL);

  }


  PointTableModel::~PointTableModel()
  {
    if (points)
    {
      points->clear();
      delete points;
      points = NULL;
    }
  }


  void PointTableModel::setPoint(ControlPoint * point, int row)
  {
    if (validateRowColumn(row, 0, false))
    {
      (*points)[row] = point;
      emit(dataChanged(QModelIndex(), QModelIndex()));
    }
    else
    {
      cerr << "PointTableModel::setPoint: row [" << row << "] out of bounds\n";
    }
  }


  void PointTableModel::setPoint(ControlPoint * point)
  {
    setPoint(point, points->size() - 1);
  }


  ControlPoint * PointTableModel::getPoint(int row) const
  {
    if (validateRowColumn(row, 0, true))
      return (*points)[row];
    else
      return NULL;
  }


  int PointTableModel::rowCount(const QModelIndex & parent) const
  {
    Q_UNUSED(parent);
    return points->size();
  }


  int PointTableModel::columnCount(const QModelIndex & parent) const
  {
    Q_UNUSED(parent);
    return COLS;
  }


  QVariant PointTableModel::data(const QModelIndex & index, int role) const
  {
    if (index.isValid() && validateRowColumn(index.row(), index.column()) &&
        role == Qt::DisplayRole)
    {
      ControlPoint * point = points->at(index.row());
      if (point)
      {
        switch ((Column) index.column())
        {
          case Id:
            return QVariant::fromValue((QString) point->GetId());
          case ChooserName:
            return QVariant::fromValue((QString) point->GetChooserName());
          case DateTime:
            return QVariant::fromValue((QString) point->GetDateTime());
          case EditLock:
            if (point->IsEditLocked())
              return QVariant::fromValue(QString("Yes"));
            else
              return QVariant::fromValue(QString("No"));
            break;
          case Ignored:
            if (point->IsIgnored())
              return QVariant::fromValue(QString("Yes"));
            else
              return QVariant::fromValue(QString("No"));
          case RefIndex:
            return QVariant::fromValue(point->IndexOfRefMeasure());
          case Reference:
            return QVariant::fromValue(
                (QString) point->GetRefMeasure()->GetCubeSerialNumber());
          case SPLat:
            return QVariant::fromValue(catchNULL(
                point->GetSurfacePoint().GetLatitude().GetDegrees()));
          case SPLon:
            return QVariant::fromValue(catchNULL(
                point->GetSurfacePoint().GetLongitude().GetDegrees()));
          case SPRadius:
            return QVariant::fromValue(catchNULL(
                point->GetSurfacePoint().GetLocalRadius().GetMeters()));
          case AprioriSPLat:
            return QVariant::fromValue(catchNULL(
                point->GetAprioriSurfacePoint().GetLatitude().GetDegrees()));
          case AprioriSPLon:
            return QVariant::fromValue(catchNULL(
                point->GetAprioriSurfacePoint().GetLongitude().GetDegrees()));
          case AprioriSPRadius:
            return QVariant::fromValue(catchNULL(
                point->GetAprioriSurfacePoint().GetLocalRadius().GetMeters()));
          case AprioriSPSource:
            return QVariant::fromValue(
                (QString) point->GetSurfacePointSourceString());
          case AprioriSPSourceFile:
            return QVariant::fromValue(
                (QString) point->GetAprioriSurfacePointSourceFile());
          case AprioriRadiusSource:
            return QVariant::fromValue(
                (QString) point->GetRadiusSourceString());
          case AprioriRadiusSourceFile:
            return QVariant::fromValue(
                (QString) point->GetAprioriRadiusSourceFile());
          case JigsawRejected:
            if (point->IsRejected())
              return QVariant::fromValue(QString("Yes"));
            else
              return QVariant::fromValue(QString("No"));
        }
      }
    }
//     else
//       cerr << "data called but failed if\n";

    return QVariant();
  }


  QVariant PointTableModel::headerData(int section,
      Qt::Orientation orientation, int role) const
  {
    QVariant result;

    if (role == Qt::DisplayRole)
    {
      if (orientation == Qt::Horizontal)
      {
        switch ((Column) section)
        {
          case Id:
            result = QVariant::fromValue(QString("Point ID"));
            break;
          case ChooserName:
            result = QVariant::fromValue(QString("Chooser Name"));
            break;
          case DateTime:
            result = QVariant::fromValue(QString("Date Time"));
            break;
          case EditLock:
            result = QVariant::fromValue(QString("Edit Lock"));
            break;
          case Ignored:
            result = QVariant::fromValue(QString("Ignored"));
            break;
          case RefIndex:
            result = QVariant::fromValue(QString("Ref index"));
            break;
          case Reference:
            result = QVariant::fromValue(QString("Reference"));
            break;
          case SPLat:
            result = QVariant::fromValue(QString("SP Lat"));
            break;
          case SPLon:
            result = QVariant::fromValue(QString("SP Lon"));
            break;
          case SPRadius:
            result = QVariant::fromValue(QString("SP Radius (m)"));
            break;
          case AprioriSPLat:
            result = QVariant::fromValue(QString("Apriori SP Lat"));
            break;
          case AprioriSPLon:
            result = QVariant::fromValue(QString("Apriori SP Lon"));
            break;
          case AprioriSPRadius:
            result = QVariant::fromValue(QString("Apriori SP Radius (m)"));
            break;
          case AprioriSPSource:
            result = QVariant::fromValue(QString("Apriori SP Source    "));
            break;
          case AprioriSPSourceFile:
            result = QVariant::fromValue(QString("Apriori SP Source File"));
            break;
          case AprioriRadiusSource:
            result = QVariant::fromValue(QString("Apriori Radius Source"));
            break;
          case AprioriRadiusSourceFile:
            result = QVariant::fromValue(QString("Apriori Radius Source File"));
            break;
          case JigsawRejected:
            result = QVariant::fromValue(QString("Jigsaw Rejected"));
            break;
        }
      }
      else
      {
        const ControlPoint * point = points->at(section);
        if (point)
          result = QVariant::fromValue((QString) point->GetId());
        else
          result = QVariant::fromValue(section);
      }
    }

    return result;
  }


  Qt::ItemFlags PointTableModel::flags(const QModelIndex & index) const
  {
    int row = index.row();
    Column column = (Column) index.column();

    // write permissions are granted, NOT ASSUMED!
    // This method could be much shorter by just checking for read only, but
    // this framework is safer, clearer, and more easily extended.
    Qt::ItemFlags flags = 0;
    if (index.isValid())
    {
      flags = flags | Qt::ItemIsEnabled;

      ControlPoint * point = (*points)[row];
      if (point)
      {
        if (point->IsEditLocked())
        {
          if (column == EditLock)
            flags = flags | Qt::ItemIsEditable;
        }
        else
        {
          switch (column)
          {
            case Id:
            case ChooserName:
            case DateTime:
            case EditLock:
            case Ignored:
            case RefIndex:
            case Reference:
            case SPLat:
            case SPLon:
            case SPRadius:
            case AprioriSPLat:
            case AprioriSPLon:
            case AprioriSPRadius:
            case AprioriSPSource:
            case AprioriSPSourceFile:
            case AprioriRadiusSource:
            case AprioriRadiusSourceFile:
              flags = flags | Qt::ItemIsEditable;
              break;
            case JigsawRejected:
              break;
          }
        }
      }
    }

    return flags;
  }


  bool PointTableModel::setData(const QModelIndex & index,
      const QVariant & value, int role)
  {
    bool success = false;
    if (index.isValid() && role == Qt::EditRole)
    {
      int row = index.row();
      int col = index.column();
      if (validateRowColumn(row, col))
      {
        ControlPoint * point = points->at(row);
        try
        {
          switch ((Column) col)
          {
            case Id:
              point->SetId(value.toString());
              break;
            case ChooserName:
              point->SetChooserName(value.toString());
              break;
            case DateTime:
              point->SetDateTime(value.toString());
              break;
            case EditLock:
              if (value.toString() == "Yes")
                point->SetEditLock(true);
              else
                if (value.toString() == "No")
                  point->SetEditLock(false);
              break;
            case Ignored:
              if (value.toString() == "Yes")
                point->SetIgnored(true);
              else
                if (value.toString() == "No")
                  point->SetIgnored(false);
              break;
            case RefIndex:
              if (value.toInt() >= 0 && value.toInt() < point->GetNumMeasures())
                point->SetRefMeasure(value.toInt());
              break;
            case Reference:
              if (point->HasSerialNumber(value.toString()))
                point->SetRefMeasure(value.toString());
              break;
            case SPLat:
              point->SetSurfacePoint(SurfacePoint(
                  Latitude(catchNULL(value.toString()), Angle::Degrees),
                  point->GetSurfacePoint().GetLongitude(),
                  point->GetSurfacePoint().GetLocalRadius()));
              break;
            case SPLon:
              point->SetSurfacePoint(SurfacePoint(
                  point->GetSurfacePoint().GetLatitude(),
                  Longitude(catchNULL(value.toString()), Angle::Degrees),
                  point->GetSurfacePoint().GetLocalRadius()));
              break;
            case SPRadius:
              point->SetSurfacePoint(SurfacePoint(
                  point->GetSurfacePoint().GetLatitude(),
                  point->GetSurfacePoint().GetLongitude(),
                  Distance(catchNULL(value.toString()), Distance::Meters)));
              break;
            case AprioriSPLat:
              point->SetAprioriSurfacePoint(SurfacePoint(
                  Latitude(catchNULL(value.toString()), Angle::Degrees),
                  point->GetAprioriSurfacePoint().GetLongitude(),
                  point->GetAprioriSurfacePoint().GetLocalRadius()));
              break;
            case AprioriSPLon:
              point->SetAprioriSurfacePoint(SurfacePoint(
                  point->GetAprioriSurfacePoint().GetLatitude(),
                  Longitude(catchNULL(value.toString()), Angle::Degrees),
                  point->GetAprioriSurfacePoint().GetLocalRadius()));
              break;
            case AprioriSPRadius:
              point->SetAprioriSurfacePoint(SurfacePoint(
                  point->GetAprioriSurfacePoint().GetLatitude(),
                  point->GetAprioriSurfacePoint().GetLongitude(),
                  Distance(catchNULL(value.toString()), Distance::Meters)));
              break;
            case AprioriSPSource:
              point->SetAprioriSurfacePointSource(
                point->StringToSurfacePointSource(value.toString()));
              break;
            case AprioriSPSourceFile:
              point->SetAprioriSurfacePointSourceFile(value.toString());
              break;
            case AprioriRadiusSource:
              point->SetAprioriRadiusSource(
                point->StringToRadiusSource(value.toString()));
              break;
            case AprioriRadiusSourceFile:
              point->SetAprioriRadiusSourceFile(value.toString());
              break;
            case JigsawRejected:
              // jigsaw rejected is not editable!
              break;
          }
          success = true;
        }
        catch (iException e)
        {
          e.Clear();
        }
        emit(dataChanged(index, index));
      }
    }
    return success;
  }


  bool PointTableModel::insertRows(int position, int rows,
      const QModelIndex & index)
  {
    Q_UNUSED(index);
    beginInsertRows(QModelIndex(), position, position + rows - 1);

    for (int i = 0; i < rows; i++)
    {
      ControlPoint * newPoint = NULL;
      points->insert(position, newPoint);
    }

    endInsertRows();
    return true;
  }


  bool PointTableModel::removeRows(int position, int rows,
      const QModelIndex & index)
  {
    Q_UNUSED(index);
    beginRemoveRows(QModelIndex(), position, position + rows - 1);

    for (int i = 0; i < rows; i++)
      points->removeAt(position);

    endInsertRows();
    return true;
  }


  bool PointTableModel::validateRowColumn(int row, int column,
      bool checkPoint) const
  {
    return row >= 0 && row < points->size() && column >= 0 &&
        column < COLS && (!checkPoint || (checkPoint && points->at(row)));
  }


  QString PointTableModel::catchNULL(double d) const
  {
    QString str = "NULL";
    if (d != Isis::NULL8)
      str = QString::number(d);

    return str;
  }


  double PointTableModel::catchNULL(QString str) const
  {
    double d = Isis::NULL8;
    if (str != "NULL")
      d = str.toDouble();

    return d;
  }
}

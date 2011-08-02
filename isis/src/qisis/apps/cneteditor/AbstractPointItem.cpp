#include "IsisDebug.h"

#include "AbstractPointItem.h"

#include <QMessageBox>
#include <QString>

#include "CnetTableColumn.h"
#include "CnetTableColumnList.h"
#include "ControlMeasure.h"
#include "ControlNet.h"
#include "ControlPoint.h"
#include "iException.h"
#include "Latitude.h"
#include "Longitude.h"


namespace Isis
{
  QString AbstractPointItem::getColumnName(Column col)
  {
    switch (col)
    {
      case Id:
        return "Point ID";
      case PointType:
        return "Point Type";
      case ChooserName:
        return "Chooser Name";
      case DateTime:
        return "Date Time";
      case EditLock:
        return "Edit Lock";
      case Ignored:
        return "Ignored";
      case Reference:
        return "Reference";
      case AdjustedSPLat:
        return "Adjusted SP Lat";
      case AdjustedSPLon:
        return "Adjusted SP Lon";
      case AdjustedSPRadius:
        return "Adjusted SP Radius";
      case APrioriSPLat:
        return "A Priori SP Lat";
      case APrioriSPLon:
        return "A Priori SP Lon";
      case APrioriSPRadius:
        return "A Priori SP Radius";
      case APrioriSPSource:
        return "A Priori SP Source";
      case APrioriSPSourceFile:
        return "A Priori SP Source File";
      case APrioriRadiusSource:
        return "A Priori Radius Source";
      case APrioriRadiusSourceFile:
        return "A Priori Radius Source File";
      case JigsawRejected:
        return "Jigsaw Rejected";
    }

    ASSERT(0);
    return QString();
  }


  AbstractPointItem::Column AbstractPointItem::getColumn(QString columnTitle)
  {
    for (int i = 0; i < COLS; i++)
    {
      if (columnTitle == getColumnName((Column)i))
        return (Column)i;
    }

    iString msg = "Column title [" + columnTitle + "] does not match any of "
        "the defined column types";
    throw iException::Message(iException::Programmer, msg, _FILEINFO_);
  }


  CnetTableColumnList AbstractPointItem::createColumns()
  {
    CnetTableColumnList columnList;

    columnList.append(new CnetTableColumn(getColumnName(Id), false, false));
    columnList.append(
        new CnetTableColumn(getColumnName(PointType), false, false));
    columnList.append(
        new CnetTableColumn(getColumnName(ChooserName), false, false));
    columnList.append(
        new CnetTableColumn(getColumnName(DateTime), true, false));
    columnList.append(
        new CnetTableColumn(getColumnName(EditLock), false, false));
    columnList.append(
        new CnetTableColumn(getColumnName(Ignored), false, true));
    columnList.append(
        new CnetTableColumn(getColumnName(Reference), false, false));
    columnList.append(
        new CnetTableColumn(getColumnName(AdjustedSPLat), true, false));
    columnList.append(
        new CnetTableColumn(getColumnName(AdjustedSPLon), true, false));
    columnList.append(
        new CnetTableColumn(getColumnName(AdjustedSPRadius), true, false));
    columnList.append(
        new CnetTableColumn(getColumnName(APrioriSPLat), false, false));
    columnList.append(
        new CnetTableColumn(getColumnName(APrioriSPLon), false, false));
    columnList.append(
        new CnetTableColumn(getColumnName(APrioriSPRadius), false, false));
    columnList.append(
        new CnetTableColumn(getColumnName(APrioriSPSource), false, false));
    columnList.append(
        new CnetTableColumn(getColumnName(APrioriSPSourceFile), false, false));
    columnList.append(
        new CnetTableColumn(getColumnName(APrioriRadiusSource), false, false));
    columnList.append(new CnetTableColumn(
        getColumnName(APrioriRadiusSourceFile), false, false));
    columnList.append(
        new CnetTableColumn(getColumnName(JigsawRejected), true, false));

    return columnList;
  }


  AbstractPointItem::AbstractPointItem(ControlPoint * cp,
      int avgCharWidth, AbstractTreeItem * parent)
      : AbstractTreeItem(parent)
  {
    ASSERT(cp);
    point = cp;
    calcDataWidth(avgCharWidth);

    connect(point, SIGNAL(destroyed(QObject *)), this, SLOT(sourceDeleted()));
  }


  AbstractPointItem::~AbstractPointItem()
  {
    point = NULL;
  }


  QString AbstractPointItem::getData() const
  {
    return getData(getColumnName(Id));
  }


  QString AbstractPointItem::getData(QString columnTitle) const
  {
    if (point)
    {
      Column column = getColumn(columnTitle);

      switch ((Column) column)
      {
        case Id:
          return (QString) point->GetId();
        case PointType:
          return (QString) point->GetPointTypeString();
        case ChooserName:
          return (QString) point->GetChooserName();
        case DateTime:
          return (QString) point->GetDateTime();
        case EditLock:
          if (point->IsEditLocked())
            return QString("Yes");
          else
            return QString("No");
          break;
        case Ignored:
          if (point->IsIgnored())
            return QString("Yes");
          else
            return QString("No");
        case Reference:
          if (point->GetNumMeasures())
            return (QString) point->GetRefMeasure()->GetCubeSerialNumber();
          else
            return QString();
        case AdjustedSPLat:
          return catchNull(
              point->GetAdjustedSurfacePoint().GetLatitude().GetDegrees());
        case AdjustedSPLon:
          return catchNull(
              point->GetAdjustedSurfacePoint().GetLongitude().GetDegrees());
        case AdjustedSPRadius:
          return catchNull(
              point->GetAdjustedSurfacePoint().GetLocalRadius().GetMeters());
        case APrioriSPLat:
          return catchNull(
              point->GetAprioriSurfacePoint().GetLatitude().GetDegrees());
        case APrioriSPLon:
          return catchNull(
              point->GetAprioriSurfacePoint().GetLongitude().GetDegrees());
        case APrioriSPRadius:
          return catchNull(
              point->GetAprioriSurfacePoint().GetLocalRadius().GetMeters());
        case APrioriSPSource:
          return (QString) point->GetSurfacePointSourceString();
        case APrioriSPSourceFile:
          return (QString) point->GetAprioriSurfacePointSourceFile();
        case APrioriRadiusSource:
          return (QString) point->GetRadiusSourceString();
        case APrioriRadiusSourceFile:
          return (QString) point->GetAprioriRadiusSourceFile();
        case JigsawRejected:
          if (point->IsRejected())
            return QString("Yes");
          else
            return QString("No");
      }
    }

    return QString();
  }


  void AbstractPointItem::setData(QString const & columnTitle,
                                  QString const & newData)
  {
    if (point)
    {
      Column column = getColumn(columnTitle);

      switch ((Column) column)
      {
        case Id:
          point->SetId(newData);
          break;
        case PointType:
          point->SetType(point->StringToPointType(newData));
          break;
        case ChooserName:
          point->SetChooserName(newData);
          break;
        case DateTime:
          point->SetDateTime(newData);
          break;
        case EditLock:
          if (newData == "Yes")
          {
            point->SetEditLock(true);
          }
          else
          {
            if (newData == "No" && point->IsEditLocked())
            {
              // Prompt the user for confirmation before turning off edit lock
              // on a point.
              QMessageBox::StandardButton status = QMessageBox::warning(
                  NULL, "cneteditor", "You requested to turn edit lock OFF "
                  "for this point.  Are you sure you want to continue?",
                  QMessageBox::Yes | QMessageBox::No);

              if (status == QMessageBox::Yes)
                point->SetEditLock(false);
            }
          }
          break;
        case Ignored:
          point->SetIgnored(newData == "Yes");
          break;
        case Reference:
          ASSERT(point->HasSerialNumber(newData));
          point->SetRefMeasure(newData);
          break;
        case AdjustedSPLat:
          point->SetAdjustedSurfacePoint(SurfacePoint(
              Latitude(catchNull(newData), Angle::Degrees),
              point->GetAdjustedSurfacePoint().GetLongitude(),
              point->GetAdjustedSurfacePoint().GetLocalRadius()));
          break;
        case AdjustedSPLon:
          point->SetAdjustedSurfacePoint(SurfacePoint(
              point->GetAdjustedSurfacePoint().GetLatitude(),
              Longitude(catchNull(newData), Angle::Degrees),
              point->GetAdjustedSurfacePoint().GetLocalRadius()));
          break;
        case AdjustedSPRadius:
          point->SetAdjustedSurfacePoint(SurfacePoint(
              point->GetAdjustedSurfacePoint().GetLatitude(),
              point->GetAdjustedSurfacePoint().GetLongitude(),
              Distance(catchNull(newData), Distance::Meters)));
          break;
        case APrioriSPLat:
          point->SetAprioriSurfacePoint(SurfacePoint(
              Latitude(catchNull(newData), Angle::Degrees),
              point->GetAprioriSurfacePoint().GetLongitude(),
              point->GetAprioriSurfacePoint().GetLocalRadius()));
          break;
        case APrioriSPLon:
          point->SetAprioriSurfacePoint(SurfacePoint(
              point->GetAprioriSurfacePoint().GetLatitude(),
              Longitude(catchNull(newData), Angle::Degrees),
              point->GetAprioriSurfacePoint().GetLocalRadius()));
          break;
        case APrioriSPRadius:
          point->SetAprioriSurfacePoint(SurfacePoint(
              point->GetAprioriSurfacePoint().GetLatitude(),
              point->GetAprioriSurfacePoint().GetLongitude(),
              Distance(catchNull(newData), Distance::Meters)));
          break;
        case APrioriSPSource:
          point->SetAprioriSurfacePointSource(
            point->StringToSurfacePointSource(newData));
          break;
        case APrioriSPSourceFile:
          point->SetAprioriSurfacePointSourceFile(newData);
          break;
        case APrioriRadiusSource:
          point->SetAprioriRadiusSource(
            point->StringToRadiusSource(newData));
          break;
        case APrioriRadiusSourceFile:
          point->SetAprioriRadiusSourceFile(newData);
          break;
        case JigsawRejected:
          // jigsaw rejected is not editable!
          break;
      }
    }
  }


  void AbstractPointItem::deleteSource()
  {
    if (point)
    {
      ControlPoint * tempPoint = point;
      point = NULL;
      tempPoint->Parent()->DeletePoint(tempPoint);
    }
  }


  AbstractTreeItem::InternalPointerType AbstractPointItem::getPointerType() const
  {
    return AbstractTreeItem::Point;
  }


  void * AbstractPointItem::getPointer() const
  {
    return point;
  }


  bool AbstractPointItem::hasPoint(ControlPoint * p) const
  {
    return point == p;
  }

  void AbstractPointItem::sourceDeleted() {
//     std::cerr << "Point item - " << point << " lost\n";
    point = NULL;
  }
}


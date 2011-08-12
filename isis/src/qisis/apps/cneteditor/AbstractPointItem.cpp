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
      case AdjustedSPLatSigma:
        return "Adjusted SP Lat Sigma";
      case AdjustedSPLonSigma:
        return "Adjusted SP Lon Sigma";
      case AdjustedSPRadiusSigma:
        return "Adjusted SP Radius Sigma";
      case APrioriSPLat:
        return "A Priori SP Lat";
      case APrioriSPLon:
        return "A Priori SP Lon";
      case APrioriSPRadius:
        return "A Priori SP Radius";
      case APrioriSPLatSigma:
        return "A Priori SP Lat Sigma";
      case APrioriSPLonSigma:
        return "A Priori SP Lon Sigma";
      case APrioriSPRadiusSigma:
        return "A Priori SP Radius Sigma";
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


  CnetTableColumnList * AbstractPointItem::createColumns()
  {
    CnetTableColumnList * columnList = new CnetTableColumnList;

    columnList->append(new CnetTableColumn(getColumnName(Id), false, false));
    columnList->append(
        new CnetTableColumn(getColumnName(PointType), false, false));
    columnList->append(
        new CnetTableColumn(getColumnName(ChooserName), false, false));
    columnList->append(
        new CnetTableColumn(getColumnName(DateTime), true, false));
    columnList->append(
        new CnetTableColumn(getColumnName(EditLock), false, false));
    columnList->append(
        new CnetTableColumn(getColumnName(Ignored), false, true));
    columnList->append(
        new CnetTableColumn(getColumnName(Reference), false, false));
    columnList->append(
        new CnetTableColumn(getColumnName(AdjustedSPLat), true, false));
    columnList->append(
        new CnetTableColumn(getColumnName(AdjustedSPLon), true, false));
    columnList->append(
        new CnetTableColumn(getColumnName(AdjustedSPRadius), true, false));
    columnList->append(
        new CnetTableColumn(getColumnName(AdjustedSPLatSigma), true, false));
    columnList->append(
        new CnetTableColumn(getColumnName(AdjustedSPLonSigma), true, false));
    columnList->append(
        new CnetTableColumn(getColumnName(AdjustedSPRadiusSigma), true, false));
    columnList->append(
        new CnetTableColumn(getColumnName(APrioriSPLat), true, false));
    columnList->append(
        new CnetTableColumn(getColumnName(APrioriSPLon), true, false));
    columnList->append(
        new CnetTableColumn(getColumnName(APrioriSPRadius), true, false));
    columnList->append(
        new CnetTableColumn(getColumnName(APrioriSPLatSigma), false, false));
    columnList->append(
        new CnetTableColumn(getColumnName(APrioriSPLonSigma), false, false));
    columnList->append(
        new CnetTableColumn(getColumnName(APrioriSPRadiusSigma), false, false));
    columnList->append(
        new CnetTableColumn(getColumnName(APrioriSPSource), false, false));
    columnList->append(
        new CnetTableColumn(getColumnName(APrioriSPSourceFile), false, false));
    columnList->append(
        new CnetTableColumn(getColumnName(APrioriRadiusSource), false, false));
    columnList->append(new CnetTableColumn(
        getColumnName(APrioriRadiusSourceFile), false, false));
    columnList->append(
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
        case AdjustedSPLatSigma:
          return catchNull(
              point->GetAdjustedSurfacePoint().
                GetLatSigmaDistance().GetMeters());
        case AdjustedSPLonSigma:
          return catchNull(
              point->GetAdjustedSurfacePoint().
                GetLonSigmaDistance().GetMeters());
        case AdjustedSPRadiusSigma:
          return catchNull(
              point->GetAdjustedSurfacePoint().
                GetLocalRadiusSigma().GetMeters());
        case APrioriSPLat:
          return catchNull(
              point->GetAprioriSurfacePoint().GetLatitude().GetDegrees());
        case APrioriSPLon:
          return catchNull(
              point->GetAprioriSurfacePoint().GetLongitude().GetDegrees());
        case APrioriSPRadius:
          return catchNull(
              point->GetAprioriSurfacePoint().GetLocalRadius().GetMeters());
        case APrioriSPLatSigma:
          return catchNull(
              point->GetAprioriSurfacePoint().
                GetLatSigmaDistance().GetMeters());
        case APrioriSPLonSigma:
          return catchNull(
              point->GetAprioriSurfacePoint().
                GetLonSigmaDistance().GetMeters());
        case APrioriSPRadiusSigma:
          return catchNull(
              point->GetAprioriSurfacePoint().
                GetLocalRadiusSigma().GetMeters());
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
            point->SetEditLock(true);
          else
            point->SetEditLock(false);
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
        case AdjustedSPLatSigma: {
          iString msg = "Cannot set adjusted surface point latitude sigma";
          throw iException::Message(iException::Programmer, msg, _FILEINFO_);
          break;
        }
        case AdjustedSPLonSigma: {
          iString msg = "Cannot set adjusted surface point longitude sigma";
          throw iException::Message(iException::Programmer, msg, _FILEINFO_);
          break;
        }
        case AdjustedSPRadiusSigma: {
          iString msg = "Cannot set adjusted surface point radius sigma";
          throw iException::Message(iException::Programmer, msg, _FILEINFO_);
          break;
        }
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
        case APrioriSPLatSigma: {
          Distance newSigma(catchNull(newData), Distance::Meters);
          SurfacePoint newSurfacePoint(prepareSigmas(newSigma,
              point->GetAprioriSurfacePoint()));

          newSurfacePoint.SetSphericalSigmasDistance(
              newSigma, newSurfacePoint.GetLonSigmaDistance(),
              newSurfacePoint.GetLocalRadiusSigma());

          point->SetAprioriSurfacePoint(newSurfacePoint);
          break;
        }
        case APrioriSPLonSigma: {
          Distance newSigma(catchNull(newData), Distance::Meters);
          SurfacePoint newSurfacePoint(prepareSigmas(newSigma,
              point->GetAprioriSurfacePoint()));

          newSurfacePoint.SetSphericalSigmasDistance(
              newSurfacePoint.GetLatSigmaDistance(), newSigma,
              newSurfacePoint.GetLocalRadiusSigma());

          point->SetAprioriSurfacePoint(newSurfacePoint);
          break;
        }
        case APrioriSPRadiusSigma: {
          Distance newSigma(catchNull(newData), Distance::Meters);
          SurfacePoint newSurfacePoint(prepareSigmas(newSigma,
              point->GetAprioriSurfacePoint()));

          newSurfacePoint.SetSphericalSigmasDistance(
              newSurfacePoint.GetLatSigmaDistance(),
              newSurfacePoint.GetLonSigmaDistance(),
              newSigma);

          point->SetAprioriSurfacePoint(newSurfacePoint);
          break;
        }
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
      if (point->IsEditLocked()) {
        iString msg = "Point [" + getData() + "] is edit locked and cannot be "
            "deleted";
        throw iException::Message(iException::User, msg, _FILEINFO_);
      }

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

  void AbstractPointItem::sourceDeleted()
  {
//     std::cerr << "Point item - " << point << " lost\n";
    point = NULL;
  }


  SurfacePoint AbstractPointItem::prepareSigmas(Distance newSigma,
      SurfacePoint surfacePoint)
  {
    const Distance free(10000, Distance::Meters);
    Distance latSigDist = surfacePoint.GetLatSigmaDistance();
    Distance lonSigDist = surfacePoint.GetLonSigmaDistance();
    Distance radiusSigDist = surfacePoint.GetLocalRadiusSigma();

    if (newSigma.Valid())
    {
      if (!latSigDist.Valid())
        latSigDist = free;
      if (!lonSigDist.Valid())
        lonSigDist = free;
      if (!radiusSigDist.Valid())
        radiusSigDist = free;
    }
    else
    {
      latSigDist = Distance();
      lonSigDist = Distance();
      radiusSigDist = Distance();
    }

    surfacePoint.SetSphericalSigmasDistance(
        latSigDist, lonSigDist, radiusSigDist);
    return surfacePoint;
  }
}


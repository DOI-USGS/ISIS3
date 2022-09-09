/** This is free and unencumbered software released into the public domain.

The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */

#include "AbstractPointItem.h"

#include <QDateTime>
#include <QMessageBox>
#include <QString>
#include <QVariant>

#include "CnetDisplayProperties.h"
#include "ControlMeasure.h"
#include "ControlNet.h"
#include "ControlPoint.h"
#include "IException.h"
#include "Latitude.h"
#include "Longitude.h"
#include "TableColumn.h"
#include "TableColumnList.h"


namespace Isis {
  QString AbstractPointItem::getColumnName(Column col) {
    switch (col) {
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

    return QString();
  }


  AbstractPointItem::Column AbstractPointItem::getColumn(QString columnTitle) {
    for (int i = 0; i < COLS; i++) {
      if (columnTitle == getColumnName((Column)i))
        return (Column)i;
    }

    QString msg = "Column title [" + columnTitle + "] does not match any of "
        "the defined column types";
    throw IException(IException::Programmer, msg, _FILEINFO_);
  }


  TableColumnList *AbstractPointItem::createColumns() {
    TableColumnList *columnList = new TableColumnList;

    columnList->append(new TableColumn(getColumnName(Id), false, false));
    columnList->append(
      new TableColumn(getColumnName(PointType), false, false));
    columnList->append(
      new TableColumn(getColumnName(ChooserName), false, false));
    columnList->append(
      new TableColumn(getColumnName(DateTime), true, false));
    columnList->append(
      new TableColumn(getColumnName(EditLock), false, false));
    columnList->append(
      new TableColumn(getColumnName(Ignored), false, true));
    columnList->append(
      new TableColumn(getColumnName(Reference), false, false));
    columnList->append(
      new TableColumn(getColumnName(AdjustedSPLat), true, false));
    columnList->append(
      new TableColumn(getColumnName(AdjustedSPLon), true, false));
    columnList->append(
      new TableColumn(getColumnName(AdjustedSPRadius), true, false));
    columnList->append(
      new TableColumn(getColumnName(AdjustedSPLatSigma), true, false));
    columnList->append(
      new TableColumn(getColumnName(AdjustedSPLonSigma), true, false));
    columnList->append(
      new TableColumn(getColumnName(AdjustedSPRadiusSigma), true, false));
    columnList->append(
      new TableColumn(getColumnName(APrioriSPLat), false, false));
    columnList->append(
      new TableColumn(getColumnName(APrioriSPLon), false, false));
    columnList->append(
      new TableColumn(getColumnName(APrioriSPRadius), false, false));
    columnList->append(
      new TableColumn(getColumnName(APrioriSPLatSigma), false, false));
    columnList->append(
      new TableColumn(getColumnName(APrioriSPLonSigma), false, false));
    columnList->append(
      new TableColumn(getColumnName(APrioriSPRadiusSigma), false, false));
    columnList->append(
      new TableColumn(getColumnName(APrioriSPSource), false, false));
    columnList->append(
      new TableColumn(getColumnName(APrioriSPSourceFile), false, false));
    columnList->append(
      new TableColumn(getColumnName(APrioriRadiusSource), false, false));
    columnList->append(new TableColumn(
        getColumnName(APrioriRadiusSourceFile), false, false));
    columnList->append(
      new TableColumn(getColumnName(JigsawRejected), true, false));

    return columnList;
  }


  AbstractPointItem::AbstractPointItem(ControlPoint *cp,
      int avgCharWidth, AbstractTreeItem *parent)
    : AbstractTreeItem(parent) {

    m_point = cp;
    calcDataWidth(avgCharWidth);

    connect(m_point, SIGNAL(destroyed(QObject *)), this, SLOT(sourceDeleted()));
  }


  AbstractPointItem::~AbstractPointItem() {
    m_point = NULL;
  }


  QVariant AbstractPointItem::getData() const {
    return getData(getColumnName(Id));
  }


  QVariant AbstractPointItem::getData(QString columnTitle) const {
    if (m_point) {
      Column column = getColumn(columnTitle);

      switch ((Column) column) {
        case Id:
          return QVariant((QString)m_point->GetId());
        case PointType:
          return QVariant((QString)m_point->GetPointTypeString());
        case ChooserName:
          return QVariant((QString)m_point->GetChooserName());
        case DateTime:
          //          return QVariant(QDateTime::fromString(
          //              m_point->GetDateTime(), "yyyy-MM-ddTHH:mm:ss"));
          return QVariant((QString)m_point->GetDateTime());
        case EditLock:
          if (m_point->IsEditLocked())
            return QVariant("Yes");
          else
            return QVariant("No");
          break;
        case Ignored:
          if (m_point->IsIgnored())
            return QVariant("Yes");
          else
            return QVariant("No");
        case Reference:
          if (m_point->GetNumMeasures())
            return QVariant(
                CnetDisplayProperties::getInstance()->getImageName(
                    (QString) m_point->GetRefMeasure()->GetCubeSerialNumber()));
          else
            return QVariant();
        case AdjustedSPLat:
          return QVariant(
              m_point->GetAdjustedSurfacePoint().GetLatitude().degrees());
        case AdjustedSPLon:
          return QVariant(
              m_point->GetAdjustedSurfacePoint().GetLongitude().degrees());
        case AdjustedSPRadius:
          return QVariant(
              m_point->GetAdjustedSurfacePoint().GetLocalRadius().meters());
        case AdjustedSPLatSigma:
          return QVariant(
              m_point->GetAdjustedSurfacePoint().
              GetLatSigmaDistance().meters());
        case AdjustedSPLonSigma:
          return QVariant(
              m_point->GetAdjustedSurfacePoint().
              GetLonSigmaDistance().meters());
        case AdjustedSPRadiusSigma:
          return QVariant(
              m_point->GetAdjustedSurfacePoint().
              GetLocalRadiusSigma().meters());
        case APrioriSPLat:
          return QVariant(
              m_point->GetAprioriSurfacePoint().GetLatitude().degrees());
        case APrioriSPLon:
          return QVariant(
              m_point->GetAprioriSurfacePoint().GetLongitude().degrees());
        case APrioriSPRadius:
          return QVariant(
              m_point->GetAprioriSurfacePoint().GetLocalRadius().meters());
        case APrioriSPLatSigma:
          return QVariant(
              m_point->GetAprioriSurfacePoint().
              GetLatSigmaDistance().meters());
        case APrioriSPLonSigma:
          return QVariant(
              m_point->GetAprioriSurfacePoint().
              GetLonSigmaDistance().meters());
        case APrioriSPRadiusSigma:
          return QVariant(
              m_point->GetAprioriSurfacePoint().
              GetLocalRadiusSigma().meters());
        case APrioriSPSource:
          return QVariant((QString)m_point->GetSurfacePointSourceString());
        case APrioriSPSourceFile:
          return QVariant((QString)m_point->GetAprioriSurfacePointSourceFile());
        case APrioriRadiusSource:
          return QVariant((QString)m_point->GetRadiusSourceString());
        case APrioriRadiusSourceFile:
          return QVariant((QString)m_point->GetAprioriRadiusSourceFile());
        case JigsawRejected:
          if (m_point->IsRejected())
            return QVariant("Yes");
          else
            return QVariant("No");
      }
    }

    return QVariant();
  }


  void AbstractPointItem::setData(QString const &columnTitle,
      QString const &newData) {
    if (m_point) {
      Column column = getColumn(columnTitle);

      switch ((Column) column) {
        case Id:
          m_point->SetId(newData);
          break;
        case PointType:
          m_point->SetType(m_point->StringToPointType(newData));
          break;
        case ChooserName:
          m_point->SetChooserName(newData);
          break;
        case DateTime:
          m_point->SetDateTime(newData);
          break;
        case EditLock:
          if (newData == "Yes")
            m_point->SetEditLock(true);
          else
            m_point->SetEditLock(false);
          break;
        case Ignored:
          m_point->SetIgnored(newData == "Yes");
          break;
        case Reference:
          m_point->SetRefMeasure(newData);
          break;
        case AdjustedSPLat:
          m_point->SetAdjustedSurfacePoint(SurfacePoint(
              Latitude(catchNull(newData), Angle::Degrees),
              m_point->GetAdjustedSurfacePoint().GetLongitude(),
              m_point->GetAdjustedSurfacePoint().GetLocalRadius()));
          break;
        case AdjustedSPLon:
          m_point->SetAdjustedSurfacePoint(SurfacePoint(
              m_point->GetAdjustedSurfacePoint().GetLatitude(),
              Longitude(catchNull(newData), Angle::Degrees),
              m_point->GetAdjustedSurfacePoint().GetLocalRadius()));
          break;
        case AdjustedSPRadius:
          m_point->SetAdjustedSurfacePoint(SurfacePoint(
              m_point->GetAdjustedSurfacePoint().GetLatitude(),
              m_point->GetAdjustedSurfacePoint().GetLongitude(),
              Distance(catchNull(newData), Distance::Meters)));
          break;
        case AdjustedSPLatSigma: {
          QString msg = "Cannot set adjusted surface point latitude sigma";
          throw IException(IException::Programmer, msg, _FILEINFO_);
          break;
        }
        case AdjustedSPLonSigma: {
          QString msg = "Cannot set adjusted surface point longitude sigma";
          throw IException(IException::Programmer, msg, _FILEINFO_);
          break;
        }
        case AdjustedSPRadiusSigma: {
          QString msg = "Cannot set adjusted surface point radius sigma";
          throw IException(IException::Programmer, msg, _FILEINFO_);
          break;
        }
        case APrioriSPLat: {
          Latitude newLat(catchNull(newData), Angle::Degrees);
          SurfacePoint newSurfacePoint(prepareSurfacePoint(newLat,
              m_point->GetAprioriSurfacePoint()));

          newSurfacePoint.SetSphericalCoordinates(newLat,
              newSurfacePoint.GetLongitude(),
              newSurfacePoint.GetLocalRadius());
          m_point->SetAprioriSurfacePoint(newSurfacePoint);
          break;
        }
        case APrioriSPLon: {
          Longitude newLon(catchNull(newData), Angle::Degrees);
          SurfacePoint newSurfacePoint(prepareSurfacePoint(newLon,
              m_point->GetAprioriSurfacePoint()));

          newSurfacePoint.SetSphericalCoordinates(
            newSurfacePoint.GetLatitude(),
            newLon,
            newSurfacePoint.GetLocalRadius());
          m_point->SetAprioriSurfacePoint(newSurfacePoint);
          break;
        }
        case APrioriSPRadius: {
          Distance newRadius(catchNull(newData), Distance::Meters);
          SurfacePoint newSurfacePoint(prepareSurfacePoint(newRadius,
              m_point->GetAprioriSurfacePoint()));

          newSurfacePoint.SetSphericalCoordinates(
            newSurfacePoint.GetLatitude(),
            newSurfacePoint.GetLongitude(),
            newRadius);
          m_point->SetAprioriSurfacePoint(newSurfacePoint);
          break;
        }
        case APrioriSPLatSigma: {
          Distance newSigma(catchNull(newData), Distance::Meters);
          SurfacePoint newSurfacePoint(prepareSigmas(newSigma,
              m_point->GetAprioriSurfacePoint()));

          newSurfacePoint.SetSphericalSigmasDistance(
            newSigma, newSurfacePoint.GetLonSigmaDistance(),
            newSurfacePoint.GetLocalRadiusSigma());

          m_point->SetAprioriSurfacePoint(newSurfacePoint);
          break;
        }
        case APrioriSPLonSigma: {
          Distance newSigma(catchNull(newData), Distance::Meters);
          SurfacePoint newSurfacePoint(prepareSigmas(newSigma,
              m_point->GetAprioriSurfacePoint()));

          newSurfacePoint.SetSphericalSigmasDistance(
            newSurfacePoint.GetLatSigmaDistance(), newSigma,
            newSurfacePoint.GetLocalRadiusSigma());

          m_point->SetAprioriSurfacePoint(newSurfacePoint);
          break;
        }
        case APrioriSPRadiusSigma: {
          Distance newSigma(catchNull(newData), Distance::Meters);
          SurfacePoint newSurfacePoint(prepareSigmas(newSigma,
              m_point->GetAprioriSurfacePoint()));

          newSurfacePoint.SetSphericalSigmasDistance(
            newSurfacePoint.GetLatSigmaDistance(),
            newSurfacePoint.GetLonSigmaDistance(),
            newSigma);

          m_point->SetAprioriSurfacePoint(newSurfacePoint);
          break;
        }
        case APrioriSPSource:
          m_point->SetAprioriSurfacePointSource(
            m_point->StringToSurfacePointSource(newData));
          break;
        case APrioriSPSourceFile:
          m_point->SetAprioriSurfacePointSourceFile(newData);
          break;
        case APrioriRadiusSource:
          m_point->SetAprioriRadiusSource(
            m_point->StringToRadiusSource(newData));
          break;
        case APrioriRadiusSourceFile:
          m_point->SetAprioriRadiusSourceFile(newData);
          break;
        case JigsawRejected:
          // jigsaw rejected is not editable!
          break;
      }
    }
  }


  // Returns true if the data at the given column is locked (i.e.
  // edit-locked). If the m_point is edit-locked, all columns except the edit
  // lock column should be uneditable.
  bool AbstractPointItem::isDataEditable(QString columnTitle) const {
    bool locked = true;
    if (m_point->IsEditLocked()) {
      if (getColumn(columnTitle) == EditLock)
        locked = false;
    }
    else {
      locked = false;
    }

    return !locked;
  }


  void AbstractPointItem::deleteSource() {
    if (m_point) {
      if (m_point->IsEditLocked()) {
        QString msg = "Point [" + getFormattedData() + "] is edit locked and "
            "cannot be deleted";
        throw IException(IException::User, msg, _FILEINFO_);
      }
      else if (m_point->GetNumLockedMeasures() > 0) {
        QString msg = "Point [" + getFormattedData() + "] has at least one "
            "edit locked measure and cannot be deleted";
        throw IException(IException::User, msg, _FILEINFO_);
      }

      ControlPoint *tempPoint = m_point;
      m_point = NULL;
      tempPoint->Parent()->DeletePoint(tempPoint);
    }
  }


  AbstractTreeItem::InternalPointerType AbstractPointItem::getPointerType() const {
    return AbstractTreeItem::Point;
  }


  void *AbstractPointItem::getPointer() const {
    return m_point;
  }


  bool AbstractPointItem::hasPoint(ControlPoint *p) const {
    return m_point == p;
  }

  void AbstractPointItem::sourceDeleted() {
    m_point = NULL;
  }


  SurfacePoint AbstractPointItem::prepareSigmas(Distance newSigma,
      SurfacePoint surfacePoint) {
    const Distance free(10000, Distance::Meters);
    Distance latSigDist = surfacePoint.GetLatSigmaDistance();
    Distance lonSigDist = surfacePoint.GetLonSigmaDistance();
    Distance radiusSigDist = surfacePoint.GetLocalRadiusSigma();

    if (newSigma.isValid()) {
      if (!latSigDist.isValid())
        latSigDist = free;
      if (!lonSigDist.isValid())
        lonSigDist = free;
      if (!radiusSigDist.isValid())
        radiusSigDist = free;
    }
    else {
      latSigDist = Distance();
      lonSigDist = Distance();
      radiusSigDist = Distance();
    }

    surfacePoint.SetSphericalSigmasDistance(
      latSigDist, lonSigDist, radiusSigDist);
    return surfacePoint;
  }


  SurfacePoint AbstractPointItem::prepareSurfacePoint(Latitude newLat,
      SurfacePoint surfacePoint) {
    if (newLat.isValid()) {
      surfacePoint = prepareSurfacePoint(surfacePoint);
    }
    else {
      surfacePoint.SetSphericalCoordinates(Latitude(), Longitude(),
          Distance());
    }

    return surfacePoint;
  }


  SurfacePoint AbstractPointItem::prepareSurfacePoint(Longitude newLon,
      SurfacePoint surfacePoint) {
    if (newLon.isValid()) {
      surfacePoint = prepareSurfacePoint(surfacePoint);
    }
    else {
      surfacePoint.SetSphericalCoordinates(Latitude(), Longitude(),
          Distance());
    }

    return surfacePoint;
  }


  SurfacePoint AbstractPointItem::prepareSurfacePoint(
    Distance newRadius, SurfacePoint surfacePoint) {
    if (newRadius.isValid()) {
      surfacePoint = prepareSurfacePoint(surfacePoint);
    }
    else {
      surfacePoint.SetSphericalCoordinates(Latitude(), Longitude(),
          Distance());
    }

    return surfacePoint;
  }


  SurfacePoint AbstractPointItem::prepareSurfacePoint(
    SurfacePoint surfacePoint) {
    Latitude lat = surfacePoint.GetLatitude();
    Longitude lon = surfacePoint.GetLongitude();
    Distance radius = surfacePoint.GetLocalRadius();

    if (!lat.isValid())
      lat = Latitude(0, Angle::Degrees);
    if (!lon.isValid())
      lon = Longitude(0, Angle::Degrees);
    if (!radius.isValid())
      radius = Distance(10000, Distance::Meters);

    surfacePoint.SetSphericalCoordinates(lat, lon, radius);
    return surfacePoint;
  }
}

#include "IsisDebug.h"

#include "CnetPointTableModel.h"

#include <iostream>

#include <QList>
#include <QMessageBox>

#include "ControlMeasure.h"
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
  CnetPointTableModel::CnetPointTableModel(TreeModel * model)
  {
    nullify();
    
    if (!model)
    {
      iString msg = "The data model is NULL";
      throw iException::Message(iException::Programmer, msg, _FILEINFO_);
    }
    
    dataModel = model;
  }


  CnetPointTableModel::~CnetPointTableModel()
  {
    dataModel = NULL;
  }


  QString CnetPointTableModel::getColName(CnetPointTableModel::Column col)
  {
    switch (col)
    {
      case Id:
        return "Point ID";
      case PointType:
        return "   Point Type   ";
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
        return "JigsawRejected";
    }

    ASSERT(0);
    return QString();
  }


  int CnetPointTableModel::getColumnCount() const
  {
    return COLS;
  }


//   QList< AbstractTreeItem * > CnetPointTableModel::getItems(
//       int start, int end) const
//   {
//     
//   }
//   
//   
//   QList< AbstractTreeItem * > CnetPointTableModel::getItems(
//       AbstractTreeItem * item1, AbstractTreeItem * item2) const
//   {
//     
//   }


//   QVariant CnetPointTableModel::data(int row, int column) const
//   {
//     ControlPoint * point = points->at(index.row());
//     if (point)
//     {
//       switch ((Column) index.column())
//       {
//         case Id:
//           return QVariant::fromValue((QString) point->GetId());
//         case PointType:
//           return QVariant::fromValue((QString) point->GetPointTypeString());
//         case ChooserName:
//           return QVariant::fromValue((QString) point->GetChooserName());
//         case DateTime:
//           return QVariant::fromValue((QString) point->GetDateTime());
//         case EditLock:
//           if (point->IsEditLocked())
//             return QVariant::fromValue(QString("Yes"));
//           else
//             return QVariant::fromValue(QString("No"));
//           break;
//         case Ignored:
//           if (point->IsIgnored())
//             return QVariant::fromValue(QString("Yes"));
//           else
//             return QVariant::fromValue(QString("No"));
//         case Reference:
//           return QVariant::fromValue(
//               (QString) point->GetRefMeasure()->GetCubeSerialNumber());
//         case AdjustedSPLat:
//           return QVariant::fromValue(catchNULL(
//               point->GetAdjustedSurfacePoint().GetLatitude().GetDegrees()));
//         case AdjustedSPLon:
//           return QVariant::fromValue(catchNULL(
//               point->GetAdjustedSurfacePoint().GetLongitude().GetDegrees()));
//         case AdjustedSPRadius:
//           return QVariant::fromValue(catchNULL(
//               point->GetAdjustedSurfacePoint().GetLocalRadius().GetMeters()));
//         case APrioriSPLat:
//           return QVariant::fromValue(catchNULL(
//               point->GetAprioriSurfacePoint().GetLatitude().GetDegrees()));
//         case APrioriSPLon:
//           return QVariant::fromValue(catchNULL(
//               point->GetAprioriSurfacePoint().GetLongitude().GetDegrees()));
//         case APrioriSPRadius:
//           return QVariant::fromValue(catchNULL(
//               point->GetAprioriSurfacePoint().GetLocalRadius().GetMeters()));
//         case APrioriSPSource:
//           return QVariant::fromValue(
//               (QString) point->GetSurfacePointSourceString());
//         case APrioriSPSourceFile:
//           return QVariant::fromValue(
//               (QString) point->GetAprioriSurfacePointSourceFile());
//         case APrioriRadiusSource:
//           return QVariant::fromValue(
//               (QString) point->GetRadiusSourceString());
//         case APrioriRadiusSourceFile:
//           return QVariant::fromValue(
//               (QString) point->GetAprioriRadiusSourceFile());
//         case JigsawRejected:
//           if (point->IsRejected())
//             return QVariant::fromValue(QString("Yes"));
//           else
//             return QVariant::fromValue(QString("No"));
//       }
//     }
// 
//     return QVariant();
//   }
// 
// 
//   QVariant CnetPointTableModel::headerData(int section,
//       Qt::Orientation orientation, int role) const
//   {
//     QVariant result;
// 
//     if (role == Qt::DisplayRole)
//     {
//       if (orientation == Qt::Horizontal)
//       {
//         result = QVariant::fromValue(getColName((Column) section));
//       }
//       else
//       {
//         QString label = "   " + QString::number(section) + "   ";
//         result = QVariant::fromValue(label);
//       }
//     }
// 
//     return result;
//   }

  
  void CnetPointTableModel::nullify()
  {
    dataModel = NULL;
  }


  QString CnetPointTableModel::catchNULL(double d) const
  {
    QString str = "NULL";
    if (d != Isis::Null)
      str = QString::number(d);

    return str;
  }


  double CnetPointTableModel::catchNULL(QString str) const
  {
    double d = Isis::Null;
    if (str.toLower() != "null")
      d = str.toDouble();

    return d;
  }
}


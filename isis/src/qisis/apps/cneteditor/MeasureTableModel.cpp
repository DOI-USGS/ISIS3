#include "IsisDebug.h"

#include "MeasureTableModel.h"

#include <iostream>

#include <QList>

#include "ControlMeasure.h"
#include "ControlMeasureLogData.h"
#include "ControlPoint.h"
#include "Distance.h"
#include "iException.h"
#include "iString.h"
#include "Latitude.h"
#include "Longitude.h"
#include "SpecialPixel.h"


const int COLS = 22;

using std::cerr;

namespace Isis
{
  MeasureTableModel::MeasureTableModel(QObject * parent) :
    QAbstractTableModel(parent)
  {
    measures = NULL;
    measures = new QList< ControlMeasure * >;
  }


  MeasureTableModel::~MeasureTableModel()
  {
    if (measures)
    {
      measures->clear();
      delete measures;
      measures = NULL;
    }
  }


  void MeasureTableModel::setMeasures(QList< ControlMeasure * > newMeasures)
  {
    beginRemoveRows(QModelIndex(), 0, measures->size() - 1);
    measures->clear();
    endRemoveRows();

    beginInsertRows(QModelIndex(), 0, newMeasures.size() - 1);
    *measures = newMeasures;
    endInsertRows();

    emit(dataChanged(QModelIndex(), QModelIndex()));
  }


  ControlMeasure * MeasureTableModel::getMeasure(int row) const
  {
    return (*measures)[row];
  }


  int MeasureTableModel::rowCount(const QModelIndex & parent) const
  {
    Q_UNUSED(parent);
    return measures->size();
  }


  int MeasureTableModel::columnCount(const QModelIndex & parent) const
  {
    Q_UNUSED(parent);
    return COLS;
  }


  QVariant MeasureTableModel::data(const QModelIndex & index, int role) const
  {
    if (index.isValid() && validateRowColumn(index.row(), index.column()) &&
        role == Qt::DisplayRole)
    {
      ControlMeasure * measure = measures->at(index.row());
      if (measure)
      {
        switch ((Column) index.column())
        {

          case PointId:
            return QVariant::fromValue((QString) measure->Parent()->GetId());
          case CubeSerialNumber:
            return QVariant::fromValue(
                (QString) measure->GetCubeSerialNumber());
          case Sample:
            return QVariant::fromValue(catchNULL(measure->GetSample()));
          case Line:
            return QVariant::fromValue(catchNULL(measure->GetLine()));
          case EditLock:
            if (measure->IsEditLocked())
              return QVariant::fromValue(QString("Yes"));
            else
              return QVariant::fromValue(QString("No"));
          case Ignored:
            if (measure->IsIgnored())
              return QVariant::fromValue(QString("Yes"));
            else
              return QVariant::fromValue(QString("No"));
          case Type:
            return QVariant::fromValue(
                (QString) measure->MeasureTypeToString(measure->GetType()));
          case Eccentricity:
          case GoodnessOfFit:
          case MinPixelZScore:
          case MaxPixelZScore:
          case PixelShift:
            return QVariant::fromValue(catchNULL(
                measure->GetLogData(index.column() - 4).GetNumericalValue()));
          case AprioriSample:
            return QVariant::fromValue(catchNULL(measure->GetAprioriSample()));
          case AprioriLine:
            return QVariant::fromValue(catchNULL(measure->GetAprioriLine()));
          case Diameter:
            return QVariant::fromValue(catchNULL(measure->GetDiameter()));
          case FocalPlaneMeasuredX:
            return QVariant::fromValue(catchNULL(
                measure->GetFocalPlaneMeasuredX()));
          case FocalPlaneMeasuredY:
            return QVariant::fromValue(catchNULL(
                measure->GetFocalPlaneMeasuredY()));
          case FocalPlaneComputedX:
            return QVariant::fromValue(catchNULL(
                measure->GetFocalPlaneComputedX()));
          case FocalPlaneComputedY:
            return QVariant::fromValue(catchNULL(
                measure->GetFocalPlaneComputedY()));
          case JigsawRejected:
            if (measure->IsRejected())
              return QVariant::fromValue(QString("Yes"));
            else
              return QVariant::fromValue(QString("No"));
          case ResidualSample:
            return QVariant::fromValue(catchNULL(measure->GetSampleResidual()));
          case ResidualLine:
            return QVariant::fromValue(catchNULL(measure->GetLineResidual()));
          case ResidualMagnitude:
            return QVariant::fromValue(catchNULL(
                measure->GetResidualMagnitude()));
        }
      }
    }

    return QVariant();
  }


  QVariant MeasureTableModel::headerData(int section,
      Qt::Orientation orientation, int role) const
  {
    QVariant result;

    if (role == Qt::DisplayRole)
    {
      if (orientation == Qt::Horizontal)
      {
        switch ((Column) section)
        {
          case PointId:
            result = QVariant::fromValue(QString("Point ID"));
            break;
          case CubeSerialNumber:
            result = QVariant::fromValue(QString("Cube Serial #"));
            break;
          case Sample:
            result = QVariant::fromValue(QString("Sample"));
            break;
          case Line:
            result = QVariant::fromValue(QString("Line"));
            break;
          case EditLock:
            result = QVariant::fromValue(QString("Edit Lock"));
            break;
          case Ignored:
            result = QVariant::fromValue(QString("Ignored"));
            break;
          case Type:
            result = QVariant::fromValue(QString("Measure Type           "));
            break;
          case Eccentricity:
            result = QVariant::fromValue(QString("Eccentricity"));
            break;
          case GoodnessOfFit:
            result = QVariant::fromValue(QString("Goodness of Fit"));
            break;
          case MinPixelZScore:
            result = QVariant::fromValue(QString("Minimum Pixel Z-Score"));
            break;
          case MaxPixelZScore:
            result = QVariant::fromValue(QString("Maximum Pixel Z-Score"));
            break;
          case PixelShift:
            result = QVariant::fromValue(QString("Pixel Shift"));
            break;
          case AprioriSample:
            result = QVariant::fromValue(QString("A Priori Sample"));
            break;
          case AprioriLine:
            result = QVariant::fromValue(QString("A Priori Line"));
            break;
          case Diameter:
            result = QVariant::fromValue(QString("Diameter"));
            break;
          case FocalPlaneMeasuredX:
            result = QVariant::fromValue(QString("Focal Plane Measured X"));
            break;
          case FocalPlaneMeasuredY:
            result = QVariant::fromValue(QString("Focal Plane Measured Y"));
            break;
          case FocalPlaneComputedX:
            result = QVariant::fromValue(QString("Focal Plane Computed X"));
            break;
          case FocalPlaneComputedY:
            result = QVariant::fromValue(QString("Focal Plane Computed Y"));
            break;
          case JigsawRejected:
            result = QVariant::fromValue(QString("Jigsaw Rejected"));
            break;
          case ResidualSample:
            result = QVariant::fromValue(QString("Residual Sample"));
            break;
          case ResidualLine:
            result = QVariant::fromValue(QString("Residual Line"));
            break;
          case ResidualMagnitude:
            result = QVariant::fromValue(QString("Residual Magnitude"));
            break;
        }
      }
      else
      {
        QString label = "   " + QString::number(section) + "   ";
        result = QVariant::fromValue(label);
      }
    }
    return result;
  }


  Qt::ItemFlags MeasureTableModel::flags(const QModelIndex & index) const
  {
    int row = index.row();
    Column column = (Column) index.column();

    // write permissions are granted, NOT ASSUMED!
    // This method could be much shorter by just checking for read only, but
    // this framework is safer, clearer, and more easily extended.
    Qt::ItemFlags flags = 0;

    if (index.isValid())
    {
      ControlMeasure * measure = (*measures)[row];
      if (measure)
      {
        if (measure->IsEditLocked())
        {
          if (column == EditLock)
            flags = flags | Qt::ItemIsEditable | Qt::ItemIsEnabled |
                Qt::ItemIsSelectable;
        }
        else
        {
          switch (column)
          {
            case CubeSerialNumber:
            case Sample:
            case Line:
            case EditLock:
            case Ignored:
            case Type:
            case Eccentricity:
            case GoodnessOfFit:
            case MinPixelZScore:
            case MaxPixelZScore:
            case PixelShift:
            case AprioriSample:
            case AprioriLine:
            case Diameter:
            case FocalPlaneMeasuredX:
            case FocalPlaneMeasuredY:
            case FocalPlaneComputedX:
            case FocalPlaneComputedY:
            case ResidualSample:
            case ResidualLine:
              flags = flags | Qt::ItemIsEditable | Qt::ItemIsEnabled |
                  Qt::ItemIsSelectable;
              break;

              // READ ONLY
            case PointId:
            case ResidualMagnitude:
            case JigsawRejected:
              break;
          }
        }
      }
    }

    return flags;
  }


  bool MeasureTableModel::setData(const QModelIndex & index,
      const QVariant & value, int role)
  {
    bool success = false;
    if (index.isValid() && role == Qt::EditRole)
    {
      int row = index.row();
      int col = index.column();
      if (validateRowColumn(row, col))
      {
        ControlMeasure * measure = measures->at(row);
        switch ((Column) col)
        {
          case PointId:
            // PointId is not editable in the measure table
            break;
          case CubeSerialNumber:
            measure->SetCubeSerialNumber(value.toString());
            break;
          case Sample:
            measure->SetCoordinate(catchNULL(value.toString()),
                measure->GetLine());
            break;
          case Line:
            measure->SetCoordinate(measure->GetSample(),
                catchNULL(value.toString()));
            break;
          case EditLock:
            if (value.toString() == "Yes")
              measure->SetEditLock(true);
            else if (value.toString() == "No")
              measure->SetEditLock(false);
            break;
          case Ignored:
            if (value.toString() == "Yes")
              measure->SetIgnored(true);
            else if (value.toString() == "No")
              measure->SetIgnored(false);
            break;
          case Type:
            measure->SetType(measure->StringToMeasureType(value.toString()));
            break;
          case Eccentricity:
          case GoodnessOfFit:
          case MinPixelZScore:
          case MaxPixelZScore:
          case PixelShift:
//             try
            {
              QString newDataStr = value.toString().toLower();
              ControlMeasureLogData::NumericLogDataType type =
                (ControlMeasureLogData::NumericLogDataType)(col - 4);
              if (newDataStr == "null")
              {
                measure->DeleteLogData(type);
              }
              else
              {
                measure->SetLogData(ControlMeasureLogData(type,
                    value.toDouble()));
              }
//             }
//             catch (iException e)
//             {
//               cerr << "MeasureTableModel::setData... FAILED!!!\n";
//               e.Clear();
            }
            break;
          case AprioriSample:
            measure->SetAprioriSample(catchNULL(value.toString()));
            break;
          case AprioriLine:
            measure->SetAprioriLine(catchNULL(value.toString()));
            break;
          case Diameter:
            measure->SetDiameter(catchNULL(value.toString()));
            break;
          case FocalPlaneMeasuredX:
            measure->SetFocalPlaneMeasured(
              catchNULL(value.toString()), measure->GetFocalPlaneMeasuredY());
            break;
          case FocalPlaneMeasuredY:
            measure->SetFocalPlaneMeasured(
              measure->GetFocalPlaneMeasuredX(), catchNULL(value.toString()));
            break;
          case FocalPlaneComputedX:
            measure->SetFocalPlaneComputed(
              catchNULL(value.toString()), measure->GetFocalPlaneComputedY());
            break;
          case FocalPlaneComputedY:
            measure->SetFocalPlaneComputed(
              measure->GetFocalPlaneComputedX(), catchNULL(value.toString()));
            break;
          case JigsawRejected:
            // jigsaw rejected is not editable!
            break;
          case ResidualSample:
            measure->SetResidual(
              catchNULL(value.toString()), measure->GetLineResidual());
            break;
          case ResidualLine:
            measure->SetResidual(
              measure->GetSampleResidual(), catchNULL(value.toString()));
            break;
          case ResidualMagnitude:
            // residual magnitude is not editable!
            break;
        }
        success = true;
        emit(dataChanged(index, index));
      }
    }
    return success;
  }


  bool MeasureTableModel::validateRowColumn(int row, int column,
      bool checkPoint) const
  {
    return row >= 0 && row < measures->size() && column >= 0 &&
        column < COLS && (!checkPoint || (checkPoint && measures->at(row)));
  }


  QString MeasureTableModel::catchNULL(double d) const
  {
    QString str = "NULL";
    if (d != Isis::Null)
      str = QString::number(d);

    return str;
  }


  double MeasureTableModel::catchNULL(QString str) const
  {
    double d = Isis::Null;
    if (str.toLower() != "null")
      d = str.toDouble();

    return d;
  }

}


#include "IsisDebug.h"

#include "MeasureTableModel.h"

#include <iostream>

#include <QList>
#include <QMessageBox>

#include "ControlMeasure.h"
#include "ControlMeasureLogData.h"
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


  QString MeasureTableModel::getColName(MeasureTableModel::Column col)
  {
    switch (col)
    {
      case PointId:
        return "Point ID";
      case CubeSerialNumber:
        return "Cube Serial Number";
      case Sample:
        return "Sample";
      case Line:
        return "Line";
      case EditLock:
        return "Edit Locked";
      case Ignored:
        return "Ignored";
      case Type:
        return "Measure Type";
      case Eccentricity:
        return "Eccentricity";
      case GoodnessOfFit:
        return "Goodness of Fit";
      case MinPixelZScore:
        return "Minimum Pixel Z-Score";
      case MaxPixelZScore:
        return "Maximum Pixel Z-Score";
      case SampleShift:
        return "Sample Shift";
      case LineShift:
        return "Line Shift";
      case SampleSigma:
        return "Sample Sigma";
      case LineSigma:
        return "Line Sigma";
      case APrioriSample:
        return "A Priori Sample";
      case APrioriLine:
        return "A Priori Line";
      case Diameter:
        return "Diameter";
      case JigsawRejected:
        return "Jigsaw Rejected";
      case ResidualSample:
        return "Residual Sample";
      case ResidualLine:
        return "Residual Line";
      case ResidualMagnitude:
        return "Residual Magnitude";
    }

    ASSERT(0);
    return QString();
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
            return QVariant::fromValue(catchNULL(
                measure->GetLogData(
                  ControlMeasureLogData::Eccentricity).GetNumericalValue()));
          case GoodnessOfFit:
            return QVariant::fromValue(catchNULL(
                measure->GetLogData(
                  ControlMeasureLogData::GoodnessOfFit).GetNumericalValue()));
          case MinPixelZScore:
            return QVariant::fromValue(catchNULL(
                measure->GetLogData(ControlMeasureLogData::MinimumPixelZScore).
                  GetNumericalValue()));
          case MaxPixelZScore:
            return QVariant::fromValue(catchNULL(
                measure->GetLogData(ControlMeasureLogData::MaximumPixelZScore).
                  GetNumericalValue()));
          case SampleShift:
            return QVariant::fromValue(catchNULL(measure->GetSampleShift()));
          case LineShift:
            return QVariant::fromValue(catchNULL(measure->GetLineShift()));
          case SampleSigma:
            return QVariant::fromValue(catchNULL(measure->GetSampleSigma()));
          case LineSigma:
            return QVariant::fromValue(catchNULL(measure->GetLineSigma()));
          case APrioriSample:
            return QVariant::fromValue(catchNULL(measure->GetAprioriSample()));
          case APrioriLine:
            return QVariant::fromValue(catchNULL(measure->GetAprioriLine()));
          case Diameter:
            return QVariant::fromValue(catchNULL(measure->GetDiameter()));
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
        result = QVariant::fromValue(getColName((Column) section));
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
            case SampleSigma:
            case LineSigma:
            case APrioriSample:
            case APrioriLine:
            case Diameter:
            case ResidualSample:
            case ResidualLine:
              flags = flags | Qt::ItemIsEditable | Qt::ItemIsEnabled |
                  Qt::ItemIsSelectable;
              break;

              // READ ONLY
            case PointId:
            case ResidualMagnitude:
            case SampleShift:
            case LineShift:
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
            if (value.toString() == "Yes" && !measure->IsEditLocked()) {
              measure->SetEditLock(true);
            }
            else if (value.toString() == "No" && measure->IsEditLocked()) {
              // Prompt the user for confirmation before turning off edit lock
              // on a measure.
              int status = QMessageBox::warning(NULL, tr("cneteditor"),
                  "You requested to turn edit lock OFF for this"
                  " measure. Are you sure you want to continue?",
                  QMessageBox::Yes | QMessageBox::No);

              if (status == QMessageBox::Yes)
                measure->SetEditLock(false);
            }
            break;
          case Ignored:
            if (value.toString() == "Yes")
              measure->SetIgnored(true);
            else
              if (value.toString() == "No")
                measure->SetIgnored(false);
            break;
          case Type:
            measure->SetType(measure->StringToMeasureType(value.toString()));
            break;
          case Eccentricity:
            setLogData(measure, ControlMeasureLogData::Eccentricity, value);
            break;
          case GoodnessOfFit:
            setLogData(measure, ControlMeasureLogData::GoodnessOfFit, value);
            break;
          case MinPixelZScore:
            setLogData(measure, ControlMeasureLogData::MinimumPixelZScore,
                       value);
            break;
          case MaxPixelZScore:
            setLogData(measure, ControlMeasureLogData::MaximumPixelZScore,
                       value);
            break;
          case SampleShift:
            // This is not editable anymore.
            break;
          case LineShift:
            // This is not editable anymore.
            break;
          case SampleSigma:
            measure->SetSampleSigma(catchNULL(value.toString()));
            break;
          case LineSigma:
            measure->SetLineSigma(catchNULL(value.toString()));
            break;
          case APrioriSample:
            measure->SetAprioriSample(catchNULL(value.toString()));
            break;
          case APrioriLine:
            measure->SetAprioriLine(catchNULL(value.toString()));
            break;
          case Diameter:
            measure->SetDiameter(catchNULL(value.toString()));
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


  void MeasureTableModel::setLogData(ControlMeasure * measure,
      int measureLogDataEnum, const QVariant & value) {
    QString newDataStr = value.toString().toLower();
    ControlMeasureLogData::NumericLogDataType type =
        (ControlMeasureLogData::NumericLogDataType) measureLogDataEnum;
    if (newDataStr == "null")
    {
      measure->DeleteLogData(type);
    }
    else
    {
      measure->SetLogData(ControlMeasureLogData(type,
          value.toDouble()));
    }
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


#include "IsisDebug.h"

#include "PointTableDelegate.h"

#include <iostream>

#include <QComboBox>
#include <QLineEdit>
#include <QSpinBox>
#include <QTableView>

#include "ControlMeasure.h"
#include "ControlPoint.h"
#include "PointTableModel.h"


using std::cerr;


namespace Isis
{
  PointTableDelegate::PointTableDelegate(PointTableModel * tm, QTableView * tv,
      QObject * parent) : QItemDelegate(parent), tableModel(tm), tableView(tv)
  {
  }


  PointTableDelegate::~PointTableDelegate()
  {
    tableModel = NULL;
  }


  QWidget * PointTableDelegate::createEditor(QWidget * parent,
      const QStyleOptionViewItem & option, const QModelIndex & index) const
  {
//     cerr << "PointTableDelegate::createEditor called...\n";
    int row = index.row();
    int col = index.column();
    ControlPoint * point = tableModel->getPoint(row);
    if (point)
    {
      switch ((PointTableModel::Column) col)
      {
        case PointTableModel::PointType:
        case PointTableModel::EditLock:
        case PointTableModel::Ignored:
        case PointTableModel::Reference:
        case PointTableModel::APrioriSPSource:
        case PointTableModel::APrioriRadiusSource:
          {
            QComboBox * combo = new QComboBox(parent);

            switch ((PointTableModel::Column) col)
            {
              case PointTableModel::PointType:
                for (int i = 0; i < ControlPoint::PointTypeCount; i++)
                {
                  combo->insertItem(i, ControlPoint::PointTypeToString(
                      (ControlPoint::PointType) i));
                }
                combo->setCurrentIndex((int) point->GetType());
                break;
              case PointTableModel::EditLock:
                combo->insertItem(0, "Yes");
                combo->insertItem(1, "No");
                combo->setCurrentIndex(point->IsEditLocked() ? 0 : 1);
                break;
              case PointTableModel::Ignored:
                combo->insertItem(0, "Yes");
                combo->insertItem(1, "No");
                combo->setCurrentIndex(point->IsIgnored() ? 0 : 1);
                break;
              case PointTableModel::Reference:
                for (int i = 0; i < point->GetNumMeasures(); i++)
                  combo->insertItem(i,
                      point->GetMeasure(i)->GetCubeSerialNumber());
                combo->setCurrentIndex(point->IndexOfRefMeasure());
                break;
              case PointTableModel::APrioriSPSource:
                combo->insertItem(0, "None");
                combo->insertItem(1, "User");
                combo->insertItem(2, "AverageOfMeasures");
                combo->insertItem(3, "Reference");
                combo->insertItem(4, "Basemap");
                combo->insertItem(5, "BundleSolution");
                combo->setCurrentIndex((int) point->GetAprioriSurfacePointSource());
                break;
              case PointTableModel::APrioriRadiusSource:
                combo->insertItem(0, "None");
                combo->insertItem(1, "User");
                combo->insertItem(2, "AverageOfMeasures");
                combo->insertItem(3, "Ellipsoid");
                combo->insertItem(4, "DEM");
                combo->insertItem(5, "BundleSolution");
                combo->setCurrentIndex((int) point->GetAprioriRadiusSource());
                break;
              default:
                break;
            }
            return combo;
          }
        default:
          {
            QLineEdit * lineEdit = new QLineEdit(parent);
            return lineEdit;
          }
      }
    }
    return new QWidget;
  }


  void PointTableDelegate::setEditorData(QWidget * editor,
      const QModelIndex & index) const
  {
//     cerr << "PointTableDelegate::setEditorData called...\n";
    int row = index.row();
    int col = index.column();

    ControlPoint * point = tableModel->getPoint(row);

    if (point)
    {
      QString value = tableModel->data(index, Qt::DisplayRole).toString();

      switch ((PointTableModel::Column) col)
      {
        case PointTableModel::PointType:
        case PointTableModel::EditLock:
        case PointTableModel::Ignored:
        case PointTableModel::Reference:
        case PointTableModel::APrioriSPSource:
        case PointTableModel::APrioriRadiusSource:
          {
            QComboBox * combo = static_cast< QComboBox * >(editor);
            switch ((PointTableModel::Column) col)
            {
              case PointTableModel::PointType:
                combo->setCurrentIndex(
                  (int) point->StringToPointType(value));
                break;
              case PointTableModel::EditLock:
                combo->setCurrentIndex(point->IsEditLocked() ? 0 : 1);
                break;
              case PointTableModel::Ignored:
                combo->setCurrentIndex(point->IsIgnored() ? 0 : 1);
                break;
              case PointTableModel::Reference:
                combo->setCurrentIndex(point->IndexOfRefMeasure());
                break;
              case PointTableModel::APrioriSPSource:
                combo->setCurrentIndex(
                  (int) point->StringToSurfacePointSource(value));
                break;
              case PointTableModel::APrioriRadiusSource:
                combo->setCurrentIndex(
                  (int) point->StringToRadiusSource(value));
                break;
              default:
                break;
            }
          }
          break;
        default:
          {
            QLineEdit * lineEdit = static_cast< QLineEdit * >(editor);
            lineEdit->setText(value);
          }
      }
    }
//     cerr << "PointTableDelegate::setEditorData done\n";
  }


  void PointTableDelegate::setModelData(QWidget * editor,
      QAbstractItemModel * model, const QModelIndex & index) const
  {
//     cerr << "PointTableDelegate::setModelData called...\n";
    int col = index.column();

    QVariant newData;

    switch ((PointTableModel::Column) col)
    {
      case PointTableModel::PointType:
      case PointTableModel::EditLock:
      case PointTableModel::Ignored:
      case PointTableModel::Reference:
      case PointTableModel::APrioriSPSource:
      case PointTableModel::APrioriRadiusSource:
        {
          QComboBox * combo = static_cast< QComboBox * >(editor);
          newData = QVariant::fromValue(combo->currentText());
        }
        break;
      default:
        {
          QLineEdit * lineEdit = static_cast< QLineEdit * >(editor);
          newData = QVariant::fromValue(lineEdit->text());
        }
    }

    // The cell doing the editing may or may not be selected, so we need to
    // always set it in case it is not selected.
    model->setData(index, newData, Qt::EditRole);

    // now look for all other selected cells in the same column and set them
    // as well
    if (col != PointTableModel::Reference)
    {
      QList< QModelIndex > selection =
        tableView->selectionModel()->selectedIndexes();
      for (int i = 0; i < selection.size(); i++)
        if (selection[i].column() == col)
          model->setData(selection[i], newData, Qt::EditRole);
    }

    emit dataEdited();
  }


  void PointTableDelegate::updateEditorGeometry(QWidget * editor,
      const QStyleOptionViewItem & option, const QModelIndex & index) const
  {
//     cerr << "PointTableDelegate::updateEditorGeometry called...\n";
    editor->setGeometry(option.rect);
//     cerr << "PointTableDelegate::updateEditorGeometry done\n";
  }

}

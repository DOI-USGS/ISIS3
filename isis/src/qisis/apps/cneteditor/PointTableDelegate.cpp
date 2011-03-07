#include "IsisDebug.h"

#include "PointTableDelegate.h"

#include <iostream>

#include <QComboBox>
#include <QSpinBox>
#include <QLineEdit>

#include "ControlPoint.h"
#include "PointTableModel.h"


using std::cerr;


namespace Isis
{
  PointTableDelegate::PointTableDelegate(PointTableModel * tm,
      QObject * parent) : QItemDelegate(parent), tableModel(tm)
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
        case PointTableModel::EditLock:
          {
            QComboBox * combo = new QComboBox(parent);
//             combo->setSizeAdjustPolicy(QComboBox::AdjustToContents);
            combo->insertItem(0, "Yes");
            combo->insertItem(1, "No");
            if (point->IsEditLocked())
              combo->setCurrentIndex(0);
            else
              combo->setCurrentIndex(1);
            return combo;
          }
        case PointTableModel::Ignored:
          {
            QComboBox * combo = new QComboBox(parent);
//             combo->setSizeAdjustPolicy(QComboBox::AdjustToContents);
            combo->insertItem(0, "Yes");
            combo->insertItem(1, "No");
            if (point->IsIgnored())
              combo->setCurrentIndex(0);
            else
              combo->setCurrentIndex(1);
            return combo;
          }
        case PointTableModel::RefIndex:
          {
            QSpinBox * spinBox = new QSpinBox(parent);
            spinBox->setMinimum(0);
            spinBox->setMaximum(point->GetNumMeasures() - 1);
            return spinBox;
          }
        case PointTableModel::AprioriSPSource:
          {
            QComboBox * combo = new QComboBox(parent);
//             combo->setSizeAdjustPolicy(QComboBox::AdjustToContents);
            combo->insertItem(0, "None");
            combo->insertItem(1, "User");
            combo->insertItem(2, "AverageOfMeasures");
            combo->insertItem(3, "Reference");
            combo->insertItem(4, "Basemap");
            combo->insertItem(5, "BundleSolution");
            combo->setCurrentIndex((int) point->GetAprioriSurfacePointSource());
            return combo;
          }
        case PointTableModel::AprioriRadiusSource:
          {
            QComboBox * combo = new QComboBox(parent);
//             combo->setSizeAdjustPolicy(QComboBox::AdjustToContents);
            combo->insertItem(0, "None");
            combo->insertItem(1, "User");
            combo->insertItem(2, "AverageOfMeasures");
            combo->insertItem(3, "Ellipsoid");
            combo->insertItem(4, "DEM");
            combo->insertItem(5, "BundleSolution");
            combo->setCurrentIndex((int) point->GetAprioriRadiusSource());
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
      switch ((PointTableModel::Column) col)
      {
        case PointTableModel::EditLock:
          {
            QString value = tableModel->data(index, Qt::DisplayRole).toString();
            QComboBox * combo = static_cast< QComboBox * >(editor);
            if (point->IsEditLocked())
              combo->setCurrentIndex(0);
            else
              combo->setCurrentIndex(1);
          }
          break;
        case PointTableModel::Ignored:
          {
            QString value = tableModel->data(index, Qt::DisplayRole).toString();
            QComboBox * combo = static_cast< QComboBox * >(editor);
            if (point->IsIgnored())
              combo->setCurrentIndex(0);
            else
              combo->setCurrentIndex(1);
          }
          break;
        case PointTableModel::RefIndex:
          {
            int value = tableModel->data(index, Qt::DisplayRole).toInt();
            QSpinBox * spinBox = static_cast< QSpinBox * >(editor);
            spinBox->setValue(value);
          }
          break;
        case PointTableModel::AprioriSPSource:
          {
            QString value = tableModel->data(index, Qt::DisplayRole).toString();
            QComboBox * combo = static_cast< QComboBox * >(editor);
            combo->setCurrentIndex(
              (int) point->StringToSurfacePointSource(value));
          }
          break;
        case PointTableModel::AprioriRadiusSource:
          {
            QString value = tableModel->data(index, Qt::DisplayRole).toString();
            QComboBox * combo = static_cast< QComboBox * >(editor);
            combo->setCurrentIndex(
              (int) point->StringToRadiusSource(value));
          }
          break;
        default:
          {
            QString value = tableModel->data(index, Qt::DisplayRole).toString();
            QLineEdit * lineEdit = static_cast< QLineEdit * >(editor);
            lineEdit->setText(value);
          }
      }
      emit editorDataSet();
    }
//     cerr << "PointTableDelegate::setEditorData done\n";
  }


  void PointTableDelegate::setModelData(QWidget * editor,
      QAbstractItemModel * model, const QModelIndex & index) const
  {
//     cerr << "PointTableDelegate::setModelData called...\n";
    int col = index.column();

    switch ((PointTableModel::Column) col)
    {
      case PointTableModel::EditLock:
      case PointTableModel::Ignored:
      case PointTableModel::AprioriSPSource:
      case PointTableModel::AprioriRadiusSource:
        {
          QComboBox * combo = static_cast< QComboBox * >(editor);
          model->setData(index, combo->currentText(), Qt::EditRole);
        }
        break;
      case PointTableModel::RefIndex:
        {
          QSpinBox * spinBox = static_cast< QSpinBox * >(editor);
          spinBox->interpretText();
          model->setData(index, spinBox->value(), Qt::EditRole);
        }
        break;
      default:
        {
          QLineEdit * lineEdit = static_cast< QLineEdit * >(editor);
          model->setData(index, lineEdit->text(), Qt::EditRole);
        }
    }

    emit dataEdited();
//     cerr << "PointTableDelegate::setModelData done\n";
  }


  void PointTableDelegate::updateEditorGeometry(QWidget * editor,
      const QStyleOptionViewItem & option, const QModelIndex & index) const
  {
//     cerr << "PointTableDelegate::updateEditorGeometry called...\n";
    editor->setGeometry(option.rect);
//     cerr << "PointTableDelegate::updateEditorGeometry done\n";
  }

}

#include "IsisDebug.h"

#include "MeasureTableDelegate.h"

#include <iostream>

#include <QComboBox>
#include <QLineEdit>
#include <QSpinBox>
#include <QTableView>

#include "ControlMeasure.h"
#include "ControlPoint.h"
#include "MeasureTableModel.h"


using std::cerr;


namespace Isis
{
  MeasureTableDelegate::MeasureTableDelegate(MeasureTableModel * tm,
      QTableView * tv, QObject * parent) : QItemDelegate(parent),
    tableModel(tm), tableView(tv)
  {
  }


  MeasureTableDelegate::~MeasureTableDelegate()
  {
    tableModel = NULL;
  }


  QWidget * MeasureTableDelegate::createEditor(QWidget * parent,
      const QStyleOptionViewItem & option, const QModelIndex & index) const
  {
//     cerr << "MeasureTableDelegate::createEditor called...\n";
    int row = index.row();
    int col = index.column();
    ControlMeasure * measure = tableModel->getMeasure(row);
    if (measure)
    {
      switch ((MeasureTableModel::Column) col)
      {
        case MeasureTableModel::EditLock:
          {
            QComboBox * combo = new QComboBox(parent);
//             combo->setSizeAdjustPolicy(QComboBox::AdjustToContents);
            combo->insertItem(0, "Yes");
            combo->insertItem(1, "No");
            if (measure->IsEditLocked())
              combo->setCurrentIndex(0);
            else
              combo->setCurrentIndex(1);
            return combo;
          }
        case MeasureTableModel::Ignored:
          {
            QComboBox * combo = new QComboBox(parent);
//             combo->setSizeAdjustPolicy(QComboBox::AdjustToContents);
            combo->insertItem(0, "Yes");
            combo->insertItem(1, "No");
            if (measure->IsIgnored())
              combo->setCurrentIndex(0);
            else
              combo->setCurrentIndex(1);
            return combo;
          }
        case MeasureTableModel::Type:
          {
            QComboBox * combo = new QComboBox(parent);
//             combo->setSizeAdjustPolicy(QComboBox::AdjustToContents);
            combo->insertItem(0, "Candidate");
            combo->insertItem(1, "Manual");
            combo->insertItem(2, "RegisteredPixel");
            combo->insertItem(3, "RegisteredSubPixel");
            combo->setCurrentIndex((int) measure->GetType());
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


  void MeasureTableDelegate::setEditorData(QWidget * editor,
      const QModelIndex & index) const
  {
//     cerr << "MeasureTableDelegate::setEditorData called...\n";
    int row = index.row();
    int col = index.column();

    ControlMeasure * measure = tableModel->getMeasure(row);

    if (measure)
    {
      switch ((MeasureTableModel::Column) col)
      {
        case MeasureTableModel::EditLock:
          {
            QString value = tableModel->data(index, Qt::DisplayRole).toString();
            QComboBox * combo = static_cast< QComboBox * >(editor);
            if (measure->IsEditLocked())
              combo->setCurrentIndex(0);
            else
              combo->setCurrentIndex(1);
          }
          break;
        case MeasureTableModel::Ignored:
          {
            QString value = tableModel->data(index, Qt::DisplayRole).toString();
            QComboBox * combo = static_cast< QComboBox * >(editor);
            if (measure->IsIgnored())
              combo->setCurrentIndex(0);
            else
              combo->setCurrentIndex(1);
          }
          break;
        case MeasureTableModel::Type:
          {
            QString value = tableModel->data(index, Qt::DisplayRole).toString();
            QComboBox * combo = static_cast< QComboBox * >(editor);
            combo->setCurrentIndex((int) measure->StringToMeasureType(value));
          }
          break;
        default:
          {
            QString value = tableModel->data(index, Qt::DisplayRole).toString();
            QLineEdit * lineEdit = static_cast< QLineEdit * >(editor);
            lineEdit->setText(value);
          }
      }
    }
//     cerr << "MeasureTableDelegate::setEditorData done\n";
  }


  void MeasureTableDelegate::setModelData(QWidget * editor,
      QAbstractItemModel * model, const QModelIndex & index) const
  {
//     cerr << "MeasureTableDelegate::setModelData called...\n";
    int col = index.column();

    QVariant newData;

    switch ((MeasureTableModel::Column) col)
    {
      case MeasureTableModel::EditLock:
      case MeasureTableModel::Ignored:
      case MeasureTableModel::Type:
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
    // as well... unless of course it is the CubeSerialNumber, in which case
    // DO NOT ALLOW setting of more than one at a time.
    // TODO do we want to be able to do multiple selections with editlock?
    if (col != (int) MeasureTableModel::CubeSerialNumber)
    {
      QList< QModelIndex > selection =
        tableView->selectionModel()->selectedIndexes();

      // Remove our index from the list, since setData(...) was already called
      // for it.
      selection.removeOne(index);

      for (int i = 0; i < selection.size(); i++)
        if (selection[i].column() == col)
          model->setData(selection[i], newData, Qt::EditRole);
    }

    emit dataEdited();
  }


  void MeasureTableDelegate::updateEditorGeometry(QWidget * editor,
      const QStyleOptionViewItem & option, const QModelIndex & index) const
  {
//     cerr << "MeasureTableDelegate::updateEditorGeometry called...\n";
    editor->setGeometry(option.rect);
//     cerr << "MeasureTableDelegate::updateEditorGeometry done\n";
  }

}

#include "IsisDebug.h"

#include "MeasureTableDelegate.h"

#include <iostream>

#include <QComboBox>
#include <QLineEdit>
#include <QMessageBox>
#include <QString>
#include <QVariant>
#include <QWidget>

#include "ControlMeasure.h"
#include "ControlMeasure.h"
#include "iException.h"
#include "iString.h"

#include "AbstractMeasureItem.h"
#include "AbstractTreeItem.h"
#include "MeasureTableModel.h"
#include "TableColumn.h"

namespace Isis
{
  namespace CnetViz
  {
    MeasureTableDelegate::MeasureTableDelegate()
    {
    }


    MeasureTableDelegate::~MeasureTableDelegate()
    {
    }


    QWidget * MeasureTableDelegate::getWidget(TableColumn const * col)
        const
    {
      AbstractMeasureItem::Column column =
          AbstractMeasureItem::getColumn(col->getTitle());

      switch (column)
      {
        case AbstractMeasureItem::Ignored:
        case AbstractMeasureItem::EditLock:
          {
            QComboBox * combo = new QComboBox;

            combo->insertItem(0, "Yes");
            combo->insertItem(1, "No");
            return combo;
          }
        case AbstractMeasureItem::Type:
          {
            QComboBox * combo = new QComboBox;

            combo->insertItem(0, "Candidate");
            combo->insertItem(1, "Manual");
            combo->insertItem(2, "RegisteredPixel");
            combo->insertItem(3, "RegisteredSubPixel");
            return combo;
          }
        default:
          {
            QLineEdit * lineEdit = new QLineEdit;
            return lineEdit;
          }
      }

      iString msg = "Could not create delegate widget for column ["
          + col->getTitle() + "]";
      throw iException::Message(iException::Programmer, msg, _FILEINFO_);
    }


    void MeasureTableDelegate::readData(QWidget * widget,
        AbstractTreeItem * row, TableColumn const * col) const
    {
      AbstractMeasureItem::Column column =
        AbstractMeasureItem::getColumn(col->getTitle());

      QString data = row->getFormattedData(col->getTitle());
      ASSERT(row->getPointerType() == AbstractTreeItem::Measure);
      ControlMeasure * measure = (ControlMeasure *)row->getPointer();

      switch (column)
      {
        case AbstractMeasureItem::EditLock:
          {
            QComboBox * combo = static_cast< QComboBox * >(widget);
            if (measure->IsEditLocked())
              combo->setCurrentIndex(0);
            else
              combo->setCurrentIndex(1);
            break;
          }
        case AbstractMeasureItem::Ignored:
          {
            QComboBox * combo = static_cast< QComboBox * >(widget);
            if (measure->IsIgnored())
              combo->setCurrentIndex(0);
            else
              combo->setCurrentIndex(1);
            break;
          }
        case AbstractMeasureItem::Type:
          {
            QComboBox * combo = static_cast< QComboBox * >(widget);
            combo->setCurrentIndex((int) measure->StringToMeasureType(data));
            break;
          }
        default:
          {
            QLineEdit * lineEdit = static_cast< QLineEdit * >(widget);
            lineEdit->setText(data);
          }
      }
    }


    void MeasureTableDelegate::readData(QWidget * widget,
        AbstractTreeItem * row, TableColumn const * col,
        QString newData) const
    {
      AbstractMeasureItem::Column column =
        AbstractMeasureItem::getColumn(col->getTitle());

      QString data = row->getFormattedData(col->getTitle());
      ASSERT(row->getPointerType() == AbstractTreeItem::Measure);
      ControlMeasure * measure = (ControlMeasure *) row->getPointer();

      switch (column)
      {
        case AbstractMeasureItem::EditLock:
          {
            QComboBox * combo = static_cast< QComboBox * >(widget);
            
            if (measure->IsEditLocked())
              combo->setCurrentIndex(0);
            else
              combo->setCurrentIndex(1);
            
            if (QString("yes").startsWith(newData.toLower()))
              combo->setCurrentIndex(0);
            else if (QString("no").startsWith(newData.toLower()))
              combo->setCurrentIndex(1);
          }
          break;
          
        case AbstractMeasureItem::Ignored:
          {
            QComboBox * combo = static_cast< QComboBox * >(widget);
        
            if (measure->IsIgnored())
              combo->setCurrentIndex(0);
            else
              combo->setCurrentIndex(1);
            
            if (QString("yes").startsWith(newData.toLower()))
              combo->setCurrentIndex(0);
            if (QString("no").startsWith(newData.toLower()))
              combo->setCurrentIndex(1);
          }
          break;
          
        case AbstractMeasureItem::Type:
          {
            QComboBox * combo = static_cast< QComboBox * >(widget);

            combo->setCurrentIndex((int) measure->StringToMeasureType(data));

            for (int i = combo->count() - 1; i >= 0; --i)
              if (combo->itemText(i).toLower().startsWith(newData.toLower()))
                combo->setCurrentIndex(i);
          }
          break;
          
        default:
          {
            QLineEdit * lineEdit = static_cast< QLineEdit * >(widget);
            lineEdit->setText(newData);
          }
      }
    }


    void MeasureTableDelegate::saveData(QWidget * widget,
        AbstractTreeItem * row, TableColumn const * col) const
    {
      AbstractMeasureItem::Column column =
          AbstractMeasureItem::getColumn(col->getTitle());
      QString newData;

      switch (column)
      {
        case AbstractMeasureItem::EditLock:
        case AbstractMeasureItem::Ignored:
        case AbstractMeasureItem::Type:
          {
            QComboBox * combo = static_cast< QComboBox * >(widget);
            newData = combo->currentText();
            break;
          }
        default:
          {
            QLineEdit * lineEdit = static_cast< QLineEdit * >(widget);
            newData = lineEdit->text();
          }
      }

      QString warningText = MeasureTableModel::getMeasureWarningMessage(
          row, col, newData);

      bool changeData = true;

      if (!warningText.isEmpty())
      {
        QMessageBox::StandardButton status = QMessageBox::warning(
            NULL, "Change cell?", warningText, QMessageBox::Yes |
            QMessageBox::No);

        changeData = (status == QMessageBox::Yes);
      }

      if (changeData)
        row->setData(col->getTitle(), newData);
    }
  }
}

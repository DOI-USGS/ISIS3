#include "IsisDebug.h"

#include "CnetMeasureTableDelegate.h"

#include <iostream>

#include <QComboBox>
#include <QLineEdit>
#include <QMessageBox>
#include <QString>
#include <QWidget>

#include "ControlMeasure.h"
#include "ControlMeasure.h"
#include "iException.h"
#include "iString.h"

#include "AbstractMeasureItem.h"
#include "AbstractTreeItem.h"
#include "CnetMeasureTableModel.h"
#include "CnetTableColumn.h"

namespace Isis
{
  CnetMeasureTableDelegate::CnetMeasureTableDelegate()
  {
  }


  CnetMeasureTableDelegate::~CnetMeasureTableDelegate()
  {
  }


  QWidget * CnetMeasureTableDelegate::getWidget(CnetTableColumn const * col)
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


  void CnetMeasureTableDelegate::readData(QWidget * widget,
      AbstractTreeItem * row, CnetTableColumn const * col) const
  {
    AbstractMeasureItem::Column column =
      AbstractMeasureItem::getColumn(col->getTitle());

    QString data = row->getData(col->getTitle());
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


  void CnetMeasureTableDelegate::saveData(QWidget * widget,
      AbstractTreeItem * row, CnetTableColumn const * col) const
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

    QString warningText = CnetMeasureTableModel::getMeasureWarningMessage(
        row, col, newData);

    bool changeData = true;

    if (!warningText.isEmpty()) {
      QMessageBox::StandardButton status = QMessageBox::warning(
          NULL, "Change cell?", warningText, QMessageBox::Yes |
          QMessageBox::No);

      changeData = (status == QMessageBox::Yes);
    }

    if (changeData)
      row->setData(col->getTitle(), newData);
  }
}


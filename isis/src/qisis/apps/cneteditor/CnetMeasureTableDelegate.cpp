#include "IsisDebug.h"

#include "CnetMeasureTableDelegate.h"

#include <iostream>

#include <QComboBox>
#include <QLineEdit>
#include <QString>
#include <QWidget>

#include "ControlMeasure.h"
#include "ControlMeasure.h"
#include "iException.h"
#include "iString.h"

#include "AbstractMeasureItem.h"
#include "AbstractTreeItem.h"

namespace Isis
{
  CnetMeasureTableDelegate::CnetMeasureTableDelegate()
  {
  }


  CnetMeasureTableDelegate::~CnetMeasureTableDelegate()
  {
  }


  QWidget * CnetMeasureTableDelegate::getWidget(QString columnTitle) const
  {
    AbstractMeasureItem::Column column =
      AbstractMeasureItem::getColumn(columnTitle);

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
        + columnTitle + "]";
    throw iException::Message(iException::Programmer, msg, _FILEINFO_);
  }


  void CnetMeasureTableDelegate::readData(QWidget * widget,
      AbstractTreeItem * row, QString columnTitle) const
  {
    AbstractMeasureItem::Column column =
      AbstractMeasureItem::getColumn(columnTitle);

    QString data = row->getData(columnTitle);
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
      AbstractTreeItem * row, QString columnTitle) const
  {
    AbstractMeasureItem::Column column =
      AbstractMeasureItem::getColumn(columnTitle);
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

    row->setData(columnTitle, newData);
  }
}


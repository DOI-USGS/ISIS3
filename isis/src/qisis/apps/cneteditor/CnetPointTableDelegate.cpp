#include "IsisDebug.h"

#include "CnetPointTableDelegate.h"

#include <iostream>

#include <QComboBox>
#include <QLineEdit>
#include <QMessageBox>
#include <QString>
#include <QWidget>

#include "ControlMeasure.h"
#include "ControlPoint.h"
#include "iException.h"

#include "AbstractPointItem.h"
#include "AbstractTreeItem.h"
#include "CnetPointTableModel.h"
#include "CnetTableColumn.h"

namespace Isis
{
  CnetPointTableDelegate::CnetPointTableDelegate()
  {
  }


  CnetPointTableDelegate::~CnetPointTableDelegate()
  {
  }


  QWidget * CnetPointTableDelegate::getWidget(CnetTableColumn const * col) const
  {
    AbstractPointItem::Column column =
      AbstractPointItem::getColumn(col->getTitle());

    switch (column)
    {
      case AbstractPointItem::PointType:
      case AbstractPointItem::EditLock:
      case AbstractPointItem::Ignored:
      case AbstractPointItem::Reference:
      case AbstractPointItem::APrioriSPSource:
      case AbstractPointItem::APrioriRadiusSource:
        {
          QComboBox * combo = new QComboBox;

          switch (column)
          {
            case AbstractPointItem::PointType:
              for (int i = 0; i < ControlPoint::PointTypeCount; i++)
              {
                combo->insertItem(i, ControlPoint::PointTypeToString(
                    (ControlPoint::PointType) i));
              }
              break;
            case AbstractPointItem::EditLock:
              combo->insertItem(0, "Yes");
              combo->insertItem(1, "No");
              break;
            case AbstractPointItem::Ignored:
              combo->insertItem(0, "Yes");
              combo->insertItem(1, "No");
              break;
            case AbstractPointItem::Reference:
              // Reference depends on possibilities, not static, so will be
              // populated later in readData(...).
              break;
            case AbstractPointItem::APrioriSPSource:
              combo->insertItem(0, "None");
              combo->insertItem(1, "User");
              combo->insertItem(2, "AverageOfMeasures");
              combo->insertItem(3, "Reference");
              combo->insertItem(4, "Basemap");
              combo->insertItem(5, "BundleSolution");
              break;
            case AbstractPointItem::APrioriRadiusSource:
              combo->insertItem(0, "None");
              combo->insertItem(1, "User");
              combo->insertItem(2, "AverageOfMeasures");
              combo->insertItem(3, "Ellipsoid");
              combo->insertItem(4, "DEM");
              combo->insertItem(5, "BundleSolution");
              break;
            default:
              break;
          }
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


  void CnetPointTableDelegate::readData(QWidget * widget,
      AbstractTreeItem * row, CnetTableColumn const * col) const
  {
    QString columnTitle = col->getTitle();
    AbstractPointItem::Column column =
        AbstractPointItem::getColumn(columnTitle);

    QString data = row->getData(columnTitle);
    ASSERT(row->getPointerType() == AbstractTreeItem::Point);
    ControlPoint * point = (ControlPoint *)row->getPointer();

    switch (column)
    {
      case AbstractPointItem::Reference:
        {
          QComboBox * combo = static_cast< QComboBox * >(widget);

          for (int i = 0; i < point->GetNumMeasures(); i++)
            combo->insertItem(i, point->GetMeasure(i)->GetCubeSerialNumber());
          combo->setCurrentIndex(point->IndexOfRefMeasure());
          break;
        }
      case AbstractPointItem::PointType:
      case AbstractPointItem::EditLock:
      case AbstractPointItem::Ignored:
      case AbstractPointItem::APrioriSPSource:
      case AbstractPointItem::APrioriRadiusSource:
        {
          QComboBox * combo = static_cast< QComboBox * >(widget);

          switch (column)
          {
            case AbstractPointItem::PointType:
              combo->setCurrentIndex(
                (int) point->StringToPointType(data));
              break;
            case AbstractPointItem::EditLock:
              combo->setCurrentIndex(point->IsEditLocked() ? 0 : 1);
              break;
            case AbstractPointItem::Ignored:
              combo->setCurrentIndex(point->IsIgnored() ? 0 : 1);
              break;
            case AbstractPointItem::APrioriSPSource:
              combo->setCurrentIndex(
                (int) point->StringToSurfacePointSource(data));
              break;
            case AbstractPointItem::APrioriRadiusSource:
              combo->setCurrentIndex(
                (int) point->StringToRadiusSource(data));
              break;
            default:
              break;
          }
        }
        break;
      default:
        {
          QLineEdit * lineEdit = static_cast< QLineEdit * >(widget);
          lineEdit->setText(data);
        }
    }
  }


  void CnetPointTableDelegate::saveData(QWidget * widget,
      AbstractTreeItem * row, const CnetTableColumn * col) const
  {
    AbstractPointItem::Column column =
      AbstractPointItem::getColumn(col->getTitle());

    QString newData;

    switch (column)
    {
      case AbstractPointItem::PointType:
      case AbstractPointItem::EditLock:
      case AbstractPointItem::Ignored:
      case AbstractPointItem::Reference:
      case AbstractPointItem::APrioriSPSource:
      case AbstractPointItem::APrioriRadiusSource:
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

    QString warningText = CnetPointTableModel::getPointWarningMessage(
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


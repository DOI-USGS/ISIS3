#include "IsisDebug.h"

#include "MeasureChildItem.h"

#include <iostream>

#include <QVariant>

#include "ControlMeasure.h"
#include "ControlPoint.h"
#include "iException.h"
#include "iString.h"


using std::cerr;


namespace Isis
{
  MeasureChildItem::MeasureChildItem(ControlMeasure * cm,
      TreeItem * parent) : TreeItem(parent)
  {
    measure = cm;
    ASSERT(measure);
  }


  MeasureChildItem::~MeasureChildItem()
  {
    measure = NULL;
  }


  void MeasureChildItem::addChild(TreeItem * child)
  {
    cerr << "MeasureChildItem::addChild called!\n";
  }


  void MeasureChildItem::removeChild(int row)
  {
    cerr << "MeasureChildItem::removeChild called!\n";
  }


  QVariant MeasureChildItem::data(int column) const
  {
    ASSERT(measure);
    validateColumn(column);
//     return QVariant((measure->*cmGetter(column))());

    return QVariant((QString) measure->GetCubeSerialNumber());
  }


  void MeasureChildItem::setData(int column, const QVariant & value)
  {
    validateColumn(column);

//     ControlMeasure::Status(ControlMeasure::*setter)(double);
//     setter = reinterpret_cast < ControlMeasure::Status
//         (ControlMeasure:: *)(double) > (cmSetter(column));

//     (measure->*setter)(value.toDouble());
  }



  /*
    double(ControlMeasure::*MeasureChildItem::cmGetter(int col) const)() const
    {
      double (ControlMeasure::*funcPtr)() const = &ControlMeasure::GetLine;

      return funcPtr;
    }


    int (ControlMeasure::*MeasureChildItem::cmSetter(int col))(double d)
    {
      ControlMeasure::Status(ControlMeasure::*methodPtr)(double);
      methodPtr = &ControlMeasure::SetLineSigma;

      return reinterpret_cast< int (ControlMeasure:: *)(double) >(methodPtr);
    }
  */

  void MeasureChildItem::deleteSource()
  {
    ASSERT(measure);

    measure->Parent()->Delete(measure);
    measure = NULL;
  }


  TreeItem::InternalPointerType MeasureChildItem::pointerType() const
  {
    return TreeItem::Measure;
  }

}

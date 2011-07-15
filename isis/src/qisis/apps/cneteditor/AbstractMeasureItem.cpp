#include "IsisDebug.h"

#include "AbstractMeasureItem.h"

#include <QString>

#include "ControlMeasure.h"
#include "ControlPoint.h"


namespace Isis
{
  AbstractMeasureItem::AbstractMeasureItem(ControlMeasure * cm,
      int avgCharWidth, AbstractTreeItem * parent)
    : AbstractTreeItem(parent)
  {
    ASSERT(cm);
    measure = cm;
    calcDataWidth(avgCharWidth);
  }


  AbstractMeasureItem::~AbstractMeasureItem()
  {
    measure = NULL;
  }


  QString AbstractMeasureItem::getData() const
  {
    ASSERT(measure);
    return (QString) measure->GetCubeSerialNumber();
  }


  void AbstractMeasureItem::deleteSource()
  {
    ASSERT(measure);
    measure->Parent()->Delete(measure);
    measure = NULL;
  }


  AbstractTreeItem::InternalPointerType AbstractMeasureItem::getPointerType()
  const
  {
    return AbstractTreeItem::Measure;
  }


  void * AbstractMeasureItem::getPointer() const
  {
    return measure;
  }


  bool AbstractMeasureItem::hasMeasure(ControlMeasure * m) const
  {
    return measure == m;
  }
}

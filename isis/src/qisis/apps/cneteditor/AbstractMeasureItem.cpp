#include "IsisDebug.h"

#include "AbstractMeasureItem.h"

#include <QVariant>

#include "ControlMeasure.h"
#include "ControlPoint.h"


namespace Isis
{
  AbstractMeasureItem::AbstractMeasureItem(ControlMeasure * cm,
      AbstractTreeItem * parent) : AbstractTreeItem(parent)
  {
    ASSERT(cm);
    measure = cm;
  }


  AbstractMeasureItem::~AbstractMeasureItem()
  {
    measure = NULL;
  }


  QVariant AbstractMeasureItem::data() const
  {
    ASSERT(measure);
    return QVariant((QString) measure->GetCubeSerialNumber());
  }


  void AbstractMeasureItem::deleteSource()
  {
    ASSERT(measure);
    measure->Parent()->Delete(measure);
    measure = NULL;
  }


  AbstractTreeItem::InternalPointerType AbstractMeasureItem::pointerType() const
  {
    return AbstractTreeItem::Measure;
  }


  bool AbstractMeasureItem::hasMeasure(ControlMeasure * m) const
  {
    return measure == m;
  }
}

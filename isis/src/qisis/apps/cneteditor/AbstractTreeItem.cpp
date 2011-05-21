#include "IsisDebug.h"

#include "AbstractTreeItem.h"

#include <QVariant>


namespace Isis
{
  AbstractTreeItem::AbstractTreeItem(AbstractTreeItem * parent) : parentItem(
      parent)
  {
    expanded = false;
    selected = false;
  }


  AbstractTreeItem::~AbstractTreeItem()
  {
    parentItem = NULL;
  }


  AbstractTreeItem * AbstractTreeItem::parent()
  {
    return parentItem;
  }


  void AbstractTreeItem::setParent(AbstractTreeItem * newParent)
  {
    parentItem = newParent;
  }


  int AbstractTreeItem::row() const
  {
    int rowIndex = -1;
    if (parentItem)
      rowIndex = parentItem->indexOf(const_cast< AbstractTreeItem * >(this));

    return rowIndex;
  }


  bool AbstractTreeItem::hasPoint(ControlPoint * point) const
  {
    bool found = false;
    for (int i = 0; !found && i < childCount(); i++)
      found = childAt(i)->hasPoint(point);
    return found;
  }


  bool AbstractTreeItem::hasMeasure(ControlMeasure * measure) const
  {
    bool found = false;
    for (int i = 0; !found && i < childCount(); i++)
      found = childAt(i)->hasMeasure(measure);
    return found;
  }


  bool AbstractTreeItem::hasNode(ControlCubeGraphNode * cube) const
  {
    bool found = false;
    for (int i = 0; !found && i < childCount(); i++)
      found = childAt(i)->hasNode(cube);
    return found;
  }


  void AbstractTreeItem::setExpanded(bool newState) { expanded = newState; }
  bool AbstractTreeItem::isExpanded() const { return expanded; }


  void AbstractTreeItem::setSelected(bool newState) { selected = newState; }
  bool AbstractTreeItem::isSelected() const { return selected; }
}

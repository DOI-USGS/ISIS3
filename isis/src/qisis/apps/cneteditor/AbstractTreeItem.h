#ifndef AbstractTreeItem_H
#define AbstractTreeItem_H


class QFontMetrics;
template< typename T > class QList;
class QString;
class QVariant;


namespace Isis
{
  class ControlCubeGraphNode;
  class ControlPoint;
  class ControlMeasure;

  class AbstractTreeItem
  {
    public:
      enum InternalPointerType
      {
        None,
        Point,
        Measure,
        CubeGraphNode,
      };


    public:
      AbstractTreeItem(AbstractTreeItem * parent = 0);
      virtual ~AbstractTreeItem();

      AbstractTreeItem * parent() const;
      void setParent(AbstractTreeItem * newParent);

      int row() const;

      // These methods are designed to be implemented either by
      // AbstractParentItem or AbstractLeafItem.
      virtual void addChild(AbstractTreeItem * child) = 0;
      virtual QList< AbstractTreeItem * > getChildren() const = 0;
      virtual AbstractTreeItem * childAt(int row) const = 0;
      virtual int childCount() const = 0;
      virtual AbstractTreeItem * getFirstVisibleChild() const = 0;
      virtual AbstractTreeItem * getLastVisibleChild() const = 0;
      virtual int indexOf(AbstractTreeItem * child) const = 0;
      virtual void setFirstVisibleChild(AbstractTreeItem * child) = 0;
      virtual void setLastVisibleChild(AbstractTreeItem * child) = 0;

      // These methods are designed to be implemented either by
      // AbstractPointItem, AbstractMeasureItem, or AbstractSerialItem.
      virtual QString getData() const = 0;
      virtual void deleteSource() = 0;
      virtual InternalPointerType getPointerType() const = 0;
      virtual void * getPointer() const = 0;

      // There are things that every AbstractTreeItem can do.
      virtual bool hasMeasure(ControlMeasure *) const;
      virtual bool hasNode(ControlCubeGraphNode *) const;
      virtual bool hasPoint(ControlPoint *) const;

      virtual AbstractTreeItem * getNextVisiblePeer() const;
      virtual void setNextVisiblePeer(AbstractTreeItem * next);

      virtual void setExpanded(bool newState);
      virtual bool isExpanded() const;

      virtual void setSelected(bool newState);
      virtual void setSelectable(bool newSelectable);
      virtual bool isSelected() const;
      virtual bool isSelectable() const;

      virtual void setVisible(bool newState);
      virtual bool isVisible() const;

      virtual int getDataWidth() const;
      virtual int getDepth() const;

      virtual void setLastVisibleFilteredItem(AbstractTreeItem * item);
      virtual const AbstractTreeItem * getLastVisibleFilteredItem() const;


    protected:
      virtual void calcDataWidth(int avgCharWidth);


      // disable copying of this class
    private:
      AbstractTreeItem(const AbstractTreeItem &);
      const AbstractTreeItem & operator=(const AbstractTreeItem &);


    private:
      AbstractTreeItem * nextVisibleItem;
      AbstractTreeItem * parentItem;
      bool expanded;
      bool selectable;
      bool selected;
      bool visible;
      int dataWidth;
  };
}

#endif


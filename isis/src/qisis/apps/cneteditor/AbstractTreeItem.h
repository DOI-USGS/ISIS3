#ifndef AbstractTreeItem_H
#define AbstractTreeItem_H


class QVariant;


namespace Isis
{
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

      AbstractTreeItem * parent();
      void setParent(AbstractTreeItem * newParent);
      int row() const;

      virtual AbstractTreeItem * childAt(int row) const = 0;
      virtual int indexOf(AbstractTreeItem * child) const = 0;
      virtual int childCount() const = 0;
      virtual void addChild(AbstractTreeItem * child) = 0;
      virtual void removeChild(int row) = 0;

      virtual QVariant data() const = 0;
      virtual void deleteSource() = 0;
      virtual InternalPointerType pointerType() const = 0;

      virtual void setExpanded(bool newState);
      virtual bool isExpanded() const;

      virtual void setSelected(bool newState);
      virtual bool isSelected() const;


      // disable copying of this class
    private:
      AbstractTreeItem(const AbstractTreeItem &);
      const AbstractTreeItem & operator=(const AbstractTreeItem &);


    protected:
      AbstractTreeItem * parentItem;
      bool expanded;
      bool selected;
  };
}

#endif

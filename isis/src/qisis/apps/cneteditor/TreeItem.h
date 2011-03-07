#ifndef TreeItem_H
#define TreeItem_H

template< class T > class QList;
class QVariant;


namespace Isis
{
  class TreeItem
  {
    public:
      enum InternalPointerType { Point, Measure, Serial, ConnectionParent};


    public:
      TreeItem(TreeItem * parent = 0);
      virtual ~TreeItem();

      TreeItem * parent();
      TreeItem * childAt(int row);
      int childCount() const;
      int row() const;
      int columnCount() const;

      virtual void addChild(TreeItem * child) = 0;
      virtual void removeChild(int row) = 0;
      virtual QVariant data(int column) const = 0;
      virtual void setData(int column, const QVariant & value) = 0;
      virtual void deleteSource() = 0;
      virtual InternalPointerType pointerType() const = 0;


    private:
      TreeItem(const TreeItem & other);
      const TreeItem & operator=(const TreeItem & other);


    protected:
      void validateColumn(int column) const;


    protected:
      TreeItem * parentItem;
      QList< TreeItem * > * children;
      int numColumns;
  };
}

#endif

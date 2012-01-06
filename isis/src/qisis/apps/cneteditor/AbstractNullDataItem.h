#ifndef AbstractNullDataItem_H
#define AbstractNullDataItem_H


#include "AbstractTreeItem.h"


class QString;


namespace Isis
{
  namespace CnetViz
  {

    /**
     * @brief Base class for an item without data
     *
     * This class represents an item in the tree that has no data. For example,
     * RootItem and BusyLeafItem are two types of items that do not have any
     * data.
     *
     * @author ????-??-?? Eric Hyer
     *
     * @internal
     */
    class AbstractNullDataItem : public virtual AbstractTreeItem
    {
        Q_OBJECT
        
      public:
        AbstractNullDataItem(AbstractTreeItem * parent = 0);
        virtual ~AbstractNullDataItem();

        virtual QVariant getData() const;
        virtual QVariant getData(QString columnTitle) const;
        virtual void setData(QString const & columnTitle, QString const & newData);
        virtual void deleteSource();
        virtual InternalPointerType getPointerType() const;
        virtual void * getPointer() const;
        virtual bool operator<(AbstractTreeItem const & other) const;

        
      private: // disable copying of this class
        AbstractNullDataItem(const AbstractNullDataItem & other);
        AbstractNullDataItem const & operator=(AbstractNullDataItem const & other);
    };
  }
}

#endif

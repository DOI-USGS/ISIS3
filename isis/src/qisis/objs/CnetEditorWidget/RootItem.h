#ifndef RootItem_H
#define RootItem_H


#include "AbstractNullDataItem.h"
#include "AbstractParentItem.h"


class QString;


namespace Isis {
  class ControlPoint;

  namespace CnetViz {

    /**
     * @brief The root of a tree
     *
     * This class represents the root of a tree in the tree model. It is
     * different from other parent nodes because it knows of the last visible
     * item in the tree that was filtered, which is needed during a
     * partially-complete filter operation so that unfiltered items can be
     * determined. The root item never contains any data.
     *
     * @author ????-??-?? Eric Hyer
     *
     * @internal
     *   @history 2012-09-28 Kimberly Oyama - Changed member variables to be prefixed with "m_".
     */
    class RootItem : public AbstractNullDataItem, public AbstractParentItem {
        Q_OBJECT

      public:
        RootItem();
        virtual ~RootItem();
        void setLastVisibleFilteredItem(AbstractTreeItem *item);
        const AbstractTreeItem *getLastVisibleFilteredItem() const;


      private: // disable copying of this class
        RootItem(const RootItem &other);
        const RootItem &operator=(const RootItem &other);


      private:
        AbstractTreeItem *m_lastVisibleFilteredItem;
    };
  }
}

#endif

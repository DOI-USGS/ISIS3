#ifndef AbstractImageItem_H
#define AbstractImageItem_H


#include "AbstractTreeItem.h"


class QString;
class QVariant;


namespace Isis {
  class ControlCubeGraphNode;

  namespace CnetViz {

    /**
     * @brief Base class for an image item in the tree
     *
     * This class represents an image item in the tree. This is generally
     * visualized as a serial number.
     *
     * @author ????-??-?? Eric Hyer
     *
     * @internal
     *   @history 2012-09-28 Kimberly Oyama - Changed member variables to be prefixed with "m_".
     */
    class AbstractImageItem : public virtual AbstractTreeItem {
      public:
        AbstractImageItem(ControlCubeGraphNode *cubeGraphNode,
            int avgCharWidth, AbstractTreeItem *parent = 0);
        virtual ~AbstractImageItem();

        QVariant getData() const;
        QVariant getData(QString columnTitle) const;
        void setData(QString const &columnTitle, QString const &newData);
        bool isDataEditable(QString columnTitle) const;
        void deleteSource();
        InternalPointerType getPointerType() const;
        void *getPointer() const;
        bool hasNode(ControlCubeGraphNode *) const;


      protected:
        virtual void sourceDeleted();

      private: // disable copying of this class
        AbstractImageItem(const AbstractImageItem &other);
        const AbstractImageItem &operator=(const AbstractImageItem &other);


      private:
        ControlCubeGraphNode *m_ccgn;
    };
  }
}

#endif

#ifndef AbstractImageItem_H
#define AbstractImageItem_H


#include "AbstractTreeItem.h"


class QString;
class QVariant;


namespace Isis
{
  class ControlCubeGraphNode;

  namespace CnetViz
  {

    /**
     * @brief Base class for an image item in the tree
     *
     * This class represents an image item in the tree. This is generally
     * visualized as a serial number.
     *
     * @author ????-??-?? Eric Hyer
     *
     * @internal
     */
    class AbstractImageItem : public virtual AbstractTreeItem
    {

        Q_OBJECT

      public:
        AbstractImageItem(ControlCubeGraphNode * cubeGraphNode,
            int avgCharWidth, AbstractTreeItem * parent = 0);
        virtual ~AbstractImageItem();

        QVariant getData() const;
        QVariant getData(QString columnTitle) const;
        void setData(QString const & columnTitle, QString const & newData);
        bool isDataLocked(QString columnTitle) const;
        void deleteSource();
        InternalPointerType getPointerType() const;
        void * getPointer() const;
        bool hasNode(ControlCubeGraphNode *) const;


      private slots:
        void sourceDeleted();


      private: // disable copying of this class
        AbstractImageItem(const AbstractImageItem & other);
        const AbstractImageItem & operator=(const AbstractImageItem & other);


      private:
        ControlCubeGraphNode * ccgn;
    };
  }
}

#endif

#ifndef AbstractImageItem_H
#define AbstractImageItem_H


#include "AbstractTreeItem.h"

class QString;
class QVariant;

namespace Isis {
  class ControlNet;

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
   *   @history 2017-07-25 Summer Stapleton - Removed the CnetViz namespace. Fixes #5054.
   *   @history 2018-06-01 Jesse Mapel - Changed ControlCubeGraphNode to image serial number.
   *                           References #5434.
   */
  class AbstractImageItem : public virtual AbstractTreeItem {
    public:
      AbstractImageItem(QString imageSerial, ControlNet *net,
                        int avgCharWidth, AbstractTreeItem *parent = 0);
      virtual ~AbstractImageItem();

      QVariant getData() const;
      QVariant getData(QString columnTitle) const;
      void setData(QString const &columnTitle, QString const &newData);
      bool isDataEditable(QString columnTitle) const;
      void deleteSource();
      InternalPointerType getPointerType() const;
      void *getPointer() const;
      bool hasImage(QString imageSerial) const;


    protected:
      virtual void sourceDeleted();

    private: // disable copying of this class
      AbstractImageItem(const AbstractImageItem &other);
      const AbstractImageItem &operator=(const AbstractImageItem &other);


    private:
      QPair<QString, ControlNet *> *m_imageAndNet;
  };
}

#endif

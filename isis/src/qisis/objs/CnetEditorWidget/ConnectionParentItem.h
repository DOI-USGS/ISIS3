#ifndef ConnectionParentItem_H
#define ConnectionParentItem_H


#include "AbstractImageItem.h"
#include "AbstractParentItem.h"

class QString;

namespace Isis {
  class ControlNet;

  /**
   * @brief Tree item that is a parent and represents an image
   *
   * This class represents a parent item in a tree structure that holds
   * serial number data.
   *
   * @author ????-??-?? Eric Hyer
   *
   * @internal
   *   @history 2017-07-25 Summer Stapleton - Removed the CnetViz namespace. Fixes #5054.
   *   @history 2018-06-01 Jesse Mapel - Changed ControlCubeGraphNode to image serial number.
   *                           References #5434.
   */
  class ConnectionParentItem : public AbstractImageItem,
    public AbstractParentItem {
    public:
      ConnectionParentItem(QString imageSerial, ControlNet *net,
                           int avgCharWidth, AbstractTreeItem *parent = 0);
      virtual ~ConnectionParentItem();

      void addChild(AbstractTreeItem *child);


    private: // Disallow copying of this class
      ConnectionParentItem(const ConnectionParentItem &);
      const ConnectionParentItem &operator=(const ConnectionParentItem &);
  };
}

#endif

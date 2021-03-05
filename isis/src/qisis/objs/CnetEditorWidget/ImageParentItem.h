#ifndef ImageParentItem_H
#define ImageParentItem_H

/** This is free and unencumbered software released into the public domain.

The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */

#include <QString>

#include "AbstractImageItem.h"
#include "AbstractParentItem.h"


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
  class ImageParentItem : public AbstractImageItem, public AbstractParentItem {
    public:
      ImageParentItem(QString imageSerial, ControlNet *net,
                      int avgCharWidth, AbstractTreeItem *parent = 0);
      virtual ~ImageParentItem();


    private: // Disallow copying of this class
      ImageParentItem(const ImageParentItem &other);
      const ImageParentItem &operator=(const ImageParentItem &other);
  };
}

#endif

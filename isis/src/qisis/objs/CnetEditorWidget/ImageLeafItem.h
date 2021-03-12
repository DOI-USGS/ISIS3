#ifndef ImageLeafItem_H
#define ImageLeafItem_H

/** This is free and unencumbered software released into the public domain.

The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */

#include <QString>

#include "AbstractImageItem.h"
#include "AbstractLeafItem.h"


namespace Isis {
  class ControlNet;

  /**
   * @brief Tree item that is a leaf and represents an image
   *
   * This class represents a leaf item in a tree structure that holds serial
   * number data.
   *
   * @author ????-??-?? Eric Hyer
   *
   * @internal
   *   @history 2017-07-25 Summer Stapleton - Removed the CnetViz namespace. Fixes #5054.
   *   @history 2018-06-01 Jesse Mapel - Changed ControlCubeGraphNode to image serial number.
   *                           References #5434.
   */
  class ImageLeafItem : public AbstractImageItem, public AbstractLeafItem {
    public:
      ImageLeafItem(QString imageSerial, ControlNet *net,
                    int avgCharWidth, AbstractTreeItem *parent = 0);
      virtual ~ImageLeafItem();


    private: // Disallow copying of this class
      ImageLeafItem(const ImageLeafItem &other);
      const ImageLeafItem &operator=(const ImageLeafItem &other);
  };
}

#endif

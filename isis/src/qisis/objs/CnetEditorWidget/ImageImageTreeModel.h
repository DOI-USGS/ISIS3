#ifndef ImageImageTreeModel_H
#define ImageImageTreeModel_H

/** This is free and unencumbered software released into the public domain.

The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */

// parent
#include "AbstractTreeModel.h"

// parent of inner class
#include <functional>


template <typename A> class QFutureWatcher;
class QString;


namespace Isis {
  class ControlNet;
  class ImageParentItem;
  class TreeView;

  /**
   * @brief Tree model for images and images
   *
   * This class represents a model that provides access to images that are
   * connected together through a control point. The tree structure is
   * designed such that images are parent nodes with images as children (leaf)
   * nodes.
   *
   * This class also provides functionality for the multi-threaded rebuilding
   * of the image-image tree model structure, which is necessary when certain
   * types of changes are made to the underlying control network.
   *
   * @author ????-??-?? Eric Hyer
   *
   * @internal
   *   @history 2012-09-28 Kimberly Oyama - Changed member variables to be prefixed with "m_".
   *   @history 2017-07-25 Summer Stapleton - Removed the CnetViz namespace. Fixes #5054.
   *   @history 2018-06-01 Jesse Mapel - Changed ControlCubeGraphNode to image serial number.
   *                           References #5434.
   */
  class ImageImageTreeModel : public AbstractTreeModel {
      Q_OBJECT

    public:
      ImageImageTreeModel(ControlNet *cNet, TreeView *v,
          QObject *parent = 0);
      virtual ~ImageImageTreeModel();

      // This is a slot!!!  There is no "pubic slots:" because it has already
      // been marked as a slot in the parent (pure virtual).  Adding the slots
      // keyword here would do nothing except make more work for both MOC and
      // the compiler!
      void rebuildItems();


    private:
      /**
       * @author ????-??-?? Eric Hyer
       *
       * @internal
       *   @history 2012-09-28 Kimberly Oyama - Changed member variables to be prefixed with "m_".
       */
      class CreateRootItemFunctor : public std::unary_function <
          const QString, ImageParentItem * > {
        public:
          CreateRootItemFunctor(AbstractTreeModel *tm, ControlNet *net, QThread *tt);
          CreateRootItemFunctor(const CreateRootItemFunctor &);
          ~CreateRootItemFunctor();
          ImageParentItem *operator()(const QString imageSerial)
          const;
          CreateRootItemFunctor &operator=(const CreateRootItemFunctor &);

          static void addToRootItem(QAtomicPointer< RootItem > &,
              ImageParentItem *const &);

        private:
          int m_avgCharWidth;
          AbstractTreeModel *m_treeModel;
          QThread *m_targetThread;
          ControlNet *m_controlNet;
      };
  };
}

#endif

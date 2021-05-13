#ifndef QnetPointCubeNameFilter_h
#define QnetPointCubeNameFilter_h

/** This is free and unencumbered software released into the public domain.

The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */

#include "QnetFilter.h"

class QListWidget;

namespace Isis {
  /**
   * Defines the Cube Name filter for the QnetNavTool's
   * Points section. The user must choose from a list of cubes.
   * This class is designed to remove points from the current
   * filtered list if no measures match the chosen image.
   *
   * @author 2009-01-26 Jeannie Walldren
   *
   * @internal
   *   @history 2009-01-26 Jeannie Walldren - Original Version
   *   @history 2010-06-03 Jeannie Walldren - Initialized pointer
   *                          to null in constructor
   */
  class QnetPointCubeNameFilter : public QnetFilter {
      Q_OBJECT

    public:
      QnetPointCubeNameFilter(QnetNavTool *navTool, QWidget *parent = 0);
      virtual void filter();

    public slots:
      void createCubeList();

    private:
      QListWidget *p_listBox;
  };
};

#endif

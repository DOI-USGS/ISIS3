#ifndef PointMeasureFilterSelector_H
#define PointMeasureFilterSelector_H

/** This is free and unencumbered software released into the public domain.

The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */

#include "AbstractFilterSelector.h"


namespace Isis {
  class AbstractFilter;

  /**
   * @brief Allows users to choose filters for filtering points and measures
   *
   * This class is responsible for creating a list of filters that can be
   * selected for filtering points and measures.
   *
   * @author ????-??-?? Eric Hyer
   *
   * @internal
   *   @history 2017-07-25 Summer Stapleton - Removed the CnetViz namespace. Fixes #5054.
   */
  class PointMeasureFilterSelector : public AbstractFilterSelector {
      Q_OBJECT

    public:
      PointMeasureFilterSelector();
      PointMeasureFilterSelector(const PointMeasureFilterSelector &other);
      virtual ~PointMeasureFilterSelector();
      PointMeasureFilterSelector &operator=(
        const PointMeasureFilterSelector &other);


    protected:
      void createSelector();


    protected: // slots (already marked as slots inside parent)
      void changeFilter(int);
  };
}

#endif

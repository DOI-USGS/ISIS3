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
   *   @history 2019-07-26 Ken Edmundson - Modifications...
   *                           1) Added additional filters to support display of control point
   *                              coordinates in either Lat, Lon, Radius or XYZ. These are
   *                              AprioriXFilter, AprioriYFilter, AprioriZFilter,
   *                              AprioriXFilterSigma, AprioriYFilterSigma, AprioriZFilterSigma,
   *                              AdjustedXFilter, AdjustedYFilter, AdjustedZFilter,
   *                              AdjustedXFilterSigma, AdjustedYFilterSigma, AdjustedZFilterSigma.
   *                            2) Methods createSelector and changeFilter modified to accomodate
   *                               the above new filters.
   *   @history 2019-11-22 Ken Edmundson - Modifications for coordinate display in either Lat, Lon,
   *                         Radius, or XYZ...
   *                           1) Changed header file names in #includes for APrioriXFilter.h,
   *                              APrioriXSigmaFilter.h, APrioriYFilter.h, APrioriYSigmaFilter.h,
   *                              APrioriZFilter.h, APrioriZSigmaFilter.h as class names changed
   *                              to be consistant with the equivalent Lat, Lon, & Radius filters.
   *                           2) Modified the createSelector() and changeFilter() methods so that
   *                              all Lat, Lon, Radius and X,Y,Z filters are available in the drop-
   *                              down filter menu. Also added additional separators in the drop-
   *                              down menu for clarity.
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

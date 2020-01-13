/** This is free and unencumbered software released into the public domain.

The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */

#include "PointMeasureFilterSelector.h"

#include <algorithm>
#include <iostream>

#include <QComboBox>
#include <QHBoxLayout>

#include "AbstractFilter.h"
#include "AdjustedLatitudeFilter.h"
#include "AdjustedLatitudeSigmaFilter.h"
#include "AdjustedLongitudeFilter.h"
#include "AdjustedLongitudeSigmaFilter.h"
#include "AdjustedRadiusFilter.h"
#include "AdjustedRadiusSigmaFilter.h"
#include "APrioriLatitudeFilter.h"
#include "APrioriLatitudeSigmaFilter.h"
#include "APrioriLongitudeFilter.h"
#include "APrioriLongitudeSigmaFilter.h"
#include "APrioriRadiusFilter.h"
#include "APrioriRadiusSigmaFilter.h"
#include "AdjustedXFilter.h"
#include "AdjustedXSigmaFilter.h"
#include "AdjustedYFilter.h"
#include "AdjustedYSigmaFilter.h"
#include "AdjustedZFilter.h"
#include "AdjustedZSigmaFilter.h"
#include "APrioriXFilter.h"
#include "APrioriXSigmaFilter.h"
#include "APrioriYFilter.h"
#include "APrioriYSigmaFilter.h"
#include "APrioriZFilter.h"
#include "APrioriZSigmaFilter.h"
#include "ChooserNameFilter.h"
#include "GoodnessOfFitFilter.h"
#include "ImageIdFilter.h"
#include "LineFilter.h"
#include "LineResidualFilter.h"
#include "LineShiftFilter.h"
#include "MeasureCountFilter.h"
#include "MeasureIgnoredFilter.h"
#include "MeasureJigsawRejectedFilter.h"
#include "MeasureTypeFilter.h"
#include "PointEditLockedFilter.h"
#include "PointIdFilter.h"
#include "PointIgnoredFilter.h"
#include "PointJigsawRejectedFilter.h"
#include "PointTypeFilter.h"
#include "ResidualMagnitudeFilter.h"
#include "SampleFilter.h"
#include "SampleResidualFilter.h"
#include "SampleShiftFilter.h"


using std::swap;


namespace Isis {
  PointMeasureFilterSelector::PointMeasureFilterSelector() {
    createSelector();
  }


  PointMeasureFilterSelector::PointMeasureFilterSelector(
    const PointMeasureFilterSelector &other) {
    createSelector();
    getSelector()->setCurrentIndex(other.getSelector()->currentIndex());
    if (other.getFilter())
      setFilter(other.getFilter()->clone());
  }


  PointMeasureFilterSelector::~PointMeasureFilterSelector() {
  }


  PointMeasureFilterSelector &PointMeasureFilterSelector::operator=(
    const PointMeasureFilterSelector &other) {
    *((AbstractFilterSelector *) this) = other;
    return *this;
  }


  void PointMeasureFilterSelector::createSelector() {
    AbstractFilterSelector::createSelector();

    getSelector()->addItem("Adjusted SP Latitude");
    getSelector()->addItem("Adjusted SP Latitude Sigma");
    getSelector()->addItem("Adjusted SP Longitude");
    getSelector()->addItem("Adjusted SP Longitude Sigma");
    getSelector()->addItem("Adjusted SP Radius");
    getSelector()->addItem("Adjusted SP Radius Sigma");
    getSelector()->addItem("A Priori SP Latitude");
    getSelector()->addItem("A Priori SP Latitude Sigma");
    getSelector()->addItem("A Priori SP Longitude");
    getSelector()->addItem("A Priori SP Longitude Sigma");
    getSelector()->addItem("A Priori SP Radius");
    getSelector()->addItem("A Priori SP Radius Sigma");
    getSelector()->insertSeparator(getSelector()->count());

    getSelector()->addItem("Adjusted SP X");
    getSelector()->addItem("Adjusted SP X Sigma");
    getSelector()->addItem("Adjusted SP Y");
    getSelector()->addItem("Adjusted SP Y Sigma");
    getSelector()->addItem("Adjusted SP Z");
    getSelector()->addItem("Adjusted SP Z Sigma");
    getSelector()->addItem("A Priori SP X");
    getSelector()->addItem("A Priori SP X Sigma");
    getSelector()->addItem("A Priori SP Y");
    getSelector()->addItem("A Priori SP Y Sigma");
    getSelector()->addItem("A Priori SP Z");
    getSelector()->addItem("A Priori SP Z Sigma");
    getSelector()->insertSeparator(getSelector()->count());

    getSelector()->addItem("Chooser Name");
    getSelector()->addItem("Edit Locked Points");
    getSelector()->addItem("Ignored Points");
    getSelector()->addItem("Jigsaw Rejected Points");
    getSelector()->addItem("Measure Count");
    getSelector()->addItem("Point ID");
    getSelector()->addItem("Point Type");
    getSelector()->insertSeparator(getSelector()->count());

    getSelector()->addItem("Goodness Of Fit");
    getSelector()->addItem("Ignored Measures");
    getSelector()->addItem("Image ID");
    getSelector()->addItem("Jigsaw Rejected Measures");
    getSelector()->addItem("Line");
    getSelector()->addItem("Line Residual");
    getSelector()->addItem("Line Shift");
    getSelector()->addItem("Measure Type");
    getSelector()->addItem("Residual Magnitude");
    getSelector()->addItem("Sample");
    getSelector()->addItem("Sample Residual");
    getSelector()->addItem("Sample Shift");
  }


  void PointMeasureFilterSelector::changeFilter(int index) {
    deleteFilter();

    if (index != 0) {
      switch (index) {
        case 0:         // this is the ----Select----- line at the top of the drop down
          break;
        case 1:         // separator
          break;
        case 2:
          setFilter(new AdjustedLatitudeFilter(AbstractFilter::Points));
          break;
        case 3:
          setFilter(new AdjustedLatitudeSigmaFilter(AbstractFilter::Points));
          break;
        case 4:
          setFilter(new AdjustedLongitudeFilter(AbstractFilter::Points));
          break;
        case 5:
          setFilter(new AdjustedLongitudeSigmaFilter(AbstractFilter::Points));
          break;
        case 6:
          setFilter(new AdjustedRadiusFilter(AbstractFilter::Points));
          break;
        case 7:
          setFilter(new AdjustedRadiusSigmaFilter(AbstractFilter::Points));
          break;
        case 8:
          setFilter(new APrioriLatitudeFilter(AbstractFilter::Points));
          break;
        case 9:
          setFilter(new APrioriLatitudeSigmaFilter(AbstractFilter::Points));
          break;
        case 10:
          setFilter(new APrioriLongitudeFilter(AbstractFilter::Points));
          break;
        case 11:
          setFilter(new APrioriLongitudeSigmaFilter(AbstractFilter::Points));
          break;
        case 12:
          setFilter(new APrioriRadiusFilter(AbstractFilter::Points));
          break;
        case 13:
          setFilter(new APrioriRadiusSigmaFilter(AbstractFilter::Points));
          break;
        case 14:         // separator
          break;
        case 15:
          setFilter(new AdjustedXFilter(AbstractFilter::Points));
          break;
        case 16:
          setFilter(new AdjustedXSigmaFilter(AbstractFilter::Points));
          break;
        case 17:
          setFilter(new AdjustedYFilter(AbstractFilter::Points));
          break;
        case 18:
          setFilter(new AdjustedYSigmaFilter(AbstractFilter::Points));
          break;
        case 19:
          setFilter(new AdjustedZFilter(AbstractFilter::Points));
          break;
        case 20:
          setFilter(new AdjustedZSigmaFilter(AbstractFilter::Points));
          break;
        case 21:
          setFilter(new APrioriXFilter(AbstractFilter::Points));
          break;
        case 22:
          setFilter(new APrioriXSigmaFilter(AbstractFilter::Points));
          break;
        case 23:
          setFilter(new APrioriYFilter(AbstractFilter::Points));
          break;
        case 24:
          setFilter(new APrioriYSigmaFilter(AbstractFilter::Points));
          break;
        case 25:
          setFilter(new APrioriZFilter(AbstractFilter::Points));
          break;
        case 26:
          setFilter(new APrioriZSigmaFilter(AbstractFilter::Points));
          break;
        case 27:         // separator
          break;
        case 28:
          setFilter(new ChooserNameFilter(AbstractFilter::Points));
          break;
        case 29:
          setFilter(new PointEditLockedFilter(AbstractFilter::Points));
          break;
        case 30:
          setFilter(new PointIgnoredFilter(AbstractFilter::Points));
          break;
        case 31:
          setFilter(new PointJigsawRejectedFilter(AbstractFilter::Points));
          break;
        case 32:
          setFilter(new MeasureCountFilter(AbstractFilter::Points));
          break;
        case 33:
          setFilter(new PointIdFilter(AbstractFilter::Points));
          break;
        case 34:
          setFilter(new PointTypeFilter(AbstractFilter::Points));
          break;
        case 35:         // separator
          break;
        case 36:
          setFilter(new GoodnessOfFitFilter(AbstractFilter::Points |
              AbstractFilter::Measures, 1));
          break;
        case 37:
          setFilter(new MeasureIgnoredFilter(AbstractFilter::Points |
              AbstractFilter::Measures, 1));
          break;
        case 38:
          setFilter(new ImageIdFilter(AbstractFilter::Points |
              AbstractFilter::Measures, 1));
          break;
        case 39:
          setFilter(new MeasureJigsawRejectedFilter(AbstractFilter::Points |
              AbstractFilter::Measures, 1));
          break;
        case 40:
          setFilter(new LineFilter(AbstractFilter::Points |
              AbstractFilter::Measures, 1));
          break;
        case 41:
          setFilter(new LineResidualFilter(AbstractFilter::Points |
              AbstractFilter::Measures, 1));
          break;
        case 42:
          setFilter(new LineShiftFilter(AbstractFilter::Points |
              AbstractFilter::Measures, 1));
          break;
        case 43:
          setFilter(new MeasureTypeFilter(AbstractFilter::Points |
              AbstractFilter::Measures, 1));
          break;
        case 44:
          setFilter(new ResidualMagnitudeFilter(AbstractFilter::Points |
              AbstractFilter::Measures, 1));
          break;
        case 45:
          setFilter(new SampleFilter(AbstractFilter::Points |
              AbstractFilter::Measures, 1));
          break;
        case 46:
          setFilter(new SampleResidualFilter(AbstractFilter::Points |
              AbstractFilter::Measures, 1));
          break;
        case 47:
          setFilter(new SampleShiftFilter(AbstractFilter::Points |
              AbstractFilter::Measures, 1));
          break;
      }
    }

    emit sizeChanged();
    emit filterChanged();
  }
}

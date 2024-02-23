/** This is free and unencumbered software released into the public domain.

The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */

#include <algorithm>

#include "ImagePointFilterSelector.h"

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
#include "CnetDisplayProperties.h"
#include "ImageIdFilter.h"
#include "GoodnessOfFitFilter.h"
#include "LineFilter.h"
#include "LineResidualFilter.h"
#include "LineShiftFilter.h"
#include "MeasureIgnoredFilter.h"
#include "MeasureJigsawRejectedFilter.h"
#include "MeasureTypeFilter.h"
#include "PointEditLockedFilter.h"
#include "PointIgnoredFilter.h"
#include "PointIdFilter.h"
#include "PointJigsawRejectedFilter.h"
#include "PointTypeFilter.h"
#include "ResidualMagnitudeFilter.h"
#include "SampleFilter.h"
#include "SampleResidualFilter.h"
#include "SampleShiftFilter.h"


using std::swap;


namespace Isis {
  ImagePointFilterSelector::ImagePointFilterSelector() {
    nullify();
    createSelector();
  }


  ImagePointFilterSelector::ImagePointFilterSelector(const ImagePointFilterSelector &other) {
    createSelector();
    getSelector()->setCurrentIndex(other.getSelector()->currentIndex());
    if (other.getFilter()) {
      setFilter(other.getFilter()->clone());
    }
  }


  ImagePointFilterSelector::~ImagePointFilterSelector() {
  }


  ImagePointFilterSelector &ImagePointFilterSelector::operator=(
        const ImagePointFilterSelector &other) {
    *((AbstractFilterSelector *) this) = other;
    return *this;
  }


  void ImagePointFilterSelector::createSelector() {
    AbstractFilterSelector::createSelector();
    
    CnetDisplayProperties *displayProperties = CnetDisplayProperties::getInstance();

    getSelector()->addItem("Image ID");
    getSelector()->insertSeparator(getSelector()->count());

    bool latLonRadDisplay = true;
    if (displayProperties->coordinateDisplayType() == CnetDisplayProperties::XYZ)
      latLonRadDisplay = false;

    if (latLonRadDisplay == true) {
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
    }
    else {
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
    }

    getSelector()->addItem("Chooser Name");
    getSelector()->addItem("Edit Locked Points");
    getSelector()->addItem("Ignored Points");
    getSelector()->addItem("Jigsaw Rejected Points");
    getSelector()->addItem("Point ID");
    getSelector()->addItem("Point Type");
    getSelector()->insertSeparator(getSelector()->count());
    getSelector()->addItem("Goodness Of Fit");
    getSelector()->addItem("Ignored Measures");
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


  void ImagePointFilterSelector::changeFilter(int index) {
    deleteFilter();

    if (index != 0) {
      switch (index) {
        case 0:         // this is the ----Select----- line at the top of the drop down
          break;
        case 1:         // separator
          break;
        case 2:
          setFilter(new ImageIdFilter(AbstractFilter::Images));
          break;
        case 3:         // separator
          break;
        case 4:
          setFilter(new AdjustedLatitudeFilter(AbstractFilter::Images |
              AbstractFilter::Points, 1));
          break;
        case 5:
          setFilter(new AdjustedLatitudeSigmaFilter(AbstractFilter::Images |
              AbstractFilter::Points, 1));
          break;
        case 6:
          setFilter(new AdjustedLongitudeFilter(AbstractFilter::Images |
              AbstractFilter::Points, 1));
          break;
        case 7:
          setFilter(new AdjustedLongitudeSigmaFilter(AbstractFilter::Images |
              AbstractFilter::Points, 1));
          break;
        case 8:
          setFilter(new AdjustedRadiusFilter(AbstractFilter::Images |
              AbstractFilter::Points, 1));
          break;
        case 9:
          setFilter(new AdjustedRadiusSigmaFilter(AbstractFilter::Images |
              AbstractFilter::Points, 1));
          break;
        case 10:
          setFilter(new APrioriLatitudeFilter(AbstractFilter::Images |
              AbstractFilter::Points, 1));
          break;
        case 11:
          setFilter(new APrioriLatitudeSigmaFilter(AbstractFilter::Images |
              AbstractFilter::Points, 1));
          break;
        case 12:
          setFilter(new APrioriLongitudeFilter(AbstractFilter::Images |
              AbstractFilter::Points, 1));
          break;
        case 13:
          setFilter(new APrioriLongitudeSigmaFilter(AbstractFilter::Images |
              AbstractFilter::Points, 1));
          break;
        case 14:
          setFilter(new APrioriRadiusFilter(AbstractFilter::Images |
              AbstractFilter::Points, 1));
          break;
        case 15:
          setFilter(new APrioriRadiusSigmaFilter(AbstractFilter::Images |
              AbstractFilter::Points, 1));
          break;
        case 16:         // separator
          break;
        case 17:
          setFilter(new AdjustedXFilter(AbstractFilter::Images |
              AbstractFilter::Points, 1));
          break;
        case 18:
          setFilter(new AdjustedXSigmaFilter(AbstractFilter::Images |
              AbstractFilter::Points, 1));
          break;
        case 19:
          setFilter(new AdjustedYFilter(AbstractFilter::Images |
              AbstractFilter::Points, 1));
          break;
        case 20:
          setFilter(new AdjustedYSigmaFilter(AbstractFilter::Images |
              AbstractFilter::Points, 1));
          break;
        case 21:
          setFilter(new AdjustedZFilter(AbstractFilter::Images |
              AbstractFilter::Points, 1));
          break;
        case 22:
          setFilter(new AdjustedZSigmaFilter(AbstractFilter::Images |
              AbstractFilter::Points, 1));
          break;
        case 23:
          setFilter(new APrioriXFilter(AbstractFilter::Images |
              AbstractFilter::Points, 1));
          break;
        case 24:
          setFilter(new APrioriXSigmaFilter(AbstractFilter::Images |
              AbstractFilter::Points, 1));
          break;
        case 25:
          setFilter(new APrioriYFilter(AbstractFilter::Images |
              AbstractFilter::Points, 1));
          break;
        case 26:
          setFilter(new APrioriYSigmaFilter(AbstractFilter::Images |
              AbstractFilter::Points, 1));
          break;
        case 27:
          setFilter(new APrioriZFilter(AbstractFilter::Images |
              AbstractFilter::Points, 1));
          break;
        case 28:
          setFilter(new APrioriZSigmaFilter(AbstractFilter::Images |
              AbstractFilter::Points, 1));
          break;
        case 29:         // separator
          break;
        case 30:
          setFilter(new ChooserNameFilter(AbstractFilter::Images |
              AbstractFilter::Points, 1));
          break;
        case 31:
          setFilter(new PointEditLockedFilter(AbstractFilter::Images |
              AbstractFilter::Points, 1));
          break;
        case 32:
          setFilter(new PointIgnoredFilter(AbstractFilter::Images |
              AbstractFilter::Points, 1));
          break;
        case 33:
          setFilter(new PointJigsawRejectedFilter(AbstractFilter::Images |
              AbstractFilter::Points, 1));
          break;
        case 34:
          setFilter(new PointIdFilter(AbstractFilter::Images |
              AbstractFilter::Points, 1));
          break;
        case 35:
          setFilter(new PointTypeFilter(AbstractFilter::Images |
              AbstractFilter::Points, 1));
          break;
        case 36:         // separator
          break;
        case 37:
          setFilter(new GoodnessOfFitFilter(AbstractFilter::Images, 1));
          break;
        case 38:
          setFilter(new MeasureIgnoredFilter(AbstractFilter::Images, 1));
          break;
        case 39:
          setFilter(new MeasureJigsawRejectedFilter(AbstractFilter::Images, 1));
          break;
        case 40:
          setFilter(new LineFilter(AbstractFilter::Images, 1));
          break;
        case 41:
          setFilter(new LineResidualFilter(AbstractFilter::Images, 1));
          break;
        case 42:
          setFilter(new LineShiftFilter(AbstractFilter::Images, 1));
          break;
        case 43:
          setFilter(new MeasureTypeFilter(AbstractFilter::Images, 1));
          break;
        case 44:
          setFilter(new ResidualMagnitudeFilter(AbstractFilter::Images, 1));
          break;
        case 45:
          setFilter(new SampleFilter(AbstractFilter::Images, 1));
          break;
        case 46:
          setFilter(new SampleResidualFilter(AbstractFilter::Images, 1));
          break;
        case 47:
          setFilter(new SampleShiftFilter(AbstractFilter::Images, 1));
          break;
      }
    }

    emit sizeChanged();
    emit filterChanged();
  }
}

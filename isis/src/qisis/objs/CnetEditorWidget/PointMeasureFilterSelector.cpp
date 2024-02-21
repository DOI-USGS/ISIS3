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
#include "AprioriXFilter.h"
#include "AprioriXSigmaFilter.h"
#include "AprioriYFilter.h"
#include "AprioriYSigmaFilter.h"
#include "AprioriZFilter.h"
#include "AprioriZSigmaFilter.h"
#include "ChooserNameFilter.h"
#include "CnetDisplayProperties.h"
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
    CnetDisplayProperties *displayProperties = CnetDisplayProperties::getInstance();

    if (displayProperties->coordinateDisplayType() == CnetDisplayProperties::LatLonRadius) {
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
    }
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

    CnetDisplayProperties *displayProperties = CnetDisplayProperties::getInstance();

    if (index != 0) {
      switch (index) {
        case 2:
          if (displayProperties->coordinateDisplayType() == CnetDisplayProperties::LatLonRadius) {
            setFilter(new AdjustedLatitudeFilter(AbstractFilter::Points));
          }
          else {
            setFilter(new AdjustedXFilter(AbstractFilter::Points));
          }
          break;
        case 3:
          if (displayProperties->coordinateDisplayType() == CnetDisplayProperties::LatLonRadius) {
            setFilter(new AdjustedLatitudeSigmaFilter(AbstractFilter::Points));
          }
          else {
            setFilter(new AdjustedXSigmaFilter(AbstractFilter::Points));
          }
          break;
        case 4:
          if (displayProperties->coordinateDisplayType() == CnetDisplayProperties::LatLonRadius) {
            setFilter(new AdjustedLongitudeFilter(AbstractFilter::Points));
          }
          else {
            setFilter(new AdjustedYFilter(AbstractFilter::Points));
          }
          break;
        case 5:
          if (displayProperties->coordinateDisplayType() == CnetDisplayProperties::LatLonRadius) {
            setFilter(new AdjustedLongitudeSigmaFilter(AbstractFilter::Points));
          }
          else {
              setFilter(new AdjustedYSigmaFilter(AbstractFilter::Points));
          }
          break;
        case 6:
          if (displayProperties->coordinateDisplayType() == CnetDisplayProperties::LatLonRadius) {
            setFilter(new AdjustedRadiusFilter(AbstractFilter::Points));
          }
          else {
            setFilter(new AdjustedZFilter(AbstractFilter::Points));
          }
          break;
        case 7:
          if (displayProperties->coordinateDisplayType() == CnetDisplayProperties::LatLonRadius) {
            setFilter(new AdjustedRadiusSigmaFilter(AbstractFilter::Points));
          }
          else {
            setFilter(new AdjustedZSigmaFilter(AbstractFilter::Points));
          }
          break;
        case 8:
          if (displayProperties->coordinateDisplayType() == CnetDisplayProperties::LatLonRadius) {
            setFilter(new APrioriLatitudeFilter(AbstractFilter::Points));
          }
          else {
            setFilter(new AprioriXFilter(AbstractFilter::Points));
          }
          break;
        case 9:
          if (displayProperties->coordinateDisplayType() == CnetDisplayProperties::LatLonRadius) {
            setFilter(new APrioriLatitudeSigmaFilter(AbstractFilter::Points));
          }
          else {
            setFilter(new AprioriXSigmaFilter(AbstractFilter::Points));
          }
          break;
        case 10:
          if (displayProperties->coordinateDisplayType() == CnetDisplayProperties::LatLonRadius) {
            setFilter(new APrioriLongitudeFilter(AbstractFilter::Points));
          }
          else {
            setFilter(new AprioriYFilter(AbstractFilter::Points));
          }
          break;
        case 11:
          if (displayProperties->coordinateDisplayType() == CnetDisplayProperties::LatLonRadius) {
            setFilter(new APrioriLongitudeSigmaFilter(AbstractFilter::Points));
          }
          else {
            setFilter(new AprioriYSigmaFilter(AbstractFilter::Points));
          }
          break;
        case 12:
          if (displayProperties->coordinateDisplayType() == CnetDisplayProperties::LatLonRadius) {
            setFilter(new APrioriRadiusFilter(AbstractFilter::Points));
          }
          else {
            setFilter(new AprioriZFilter(AbstractFilter::Points));
          }
          break;
        case 13:
          if (displayProperties->coordinateDisplayType() == CnetDisplayProperties::LatLonRadius) {
            setFilter(new APrioriRadiusSigmaFilter(AbstractFilter::Points));
          }
          else {
            setFilter(new AprioriZSigmaFilter(AbstractFilter::Points));
          }
          break;
        case 14:
          setFilter(new ChooserNameFilter(AbstractFilter::Points));
          break;
        case 15:
          setFilter(new PointEditLockedFilter(AbstractFilter::Points));
          break;
        case 16:
          setFilter(new PointIgnoredFilter(AbstractFilter::Points));
          break;
        case 17:
          setFilter(new PointJigsawRejectedFilter(AbstractFilter::Points));
          break;
        case 18:
          setFilter(new MeasureCountFilter(AbstractFilter::Points));
          break;
        case 19:
          setFilter(new PointIdFilter(AbstractFilter::Points));
          break;
        case 20:
          setFilter(new PointTypeFilter(AbstractFilter::Points));
          break;
        case 22:
          setFilter(new GoodnessOfFitFilter(AbstractFilter::Points |
              AbstractFilter::Measures, 1));
          break;
        case 23:
          setFilter(new MeasureIgnoredFilter(AbstractFilter::Points |
              AbstractFilter::Measures, 1));
          break;
        case 24:
          setFilter(new ImageIdFilter(AbstractFilter::Points |
              AbstractFilter::Measures, 1));
          break;
        case 25:
          setFilter(new MeasureJigsawRejectedFilter(AbstractFilter::Points |
              AbstractFilter::Measures, 1));
          break;
        case 26:
          setFilter(new LineFilter(AbstractFilter::Points |
              AbstractFilter::Measures, 1));
          break;
        case 27:
          setFilter(new LineResidualFilter(AbstractFilter::Points |
              AbstractFilter::Measures, 1));
          break;
        case 28:
          setFilter(new LineShiftFilter(AbstractFilter::Points |
              AbstractFilter::Measures, 1));
          break;
        case 29:
          setFilter(new MeasureTypeFilter(AbstractFilter::Points |
              AbstractFilter::Measures, 1));
          break;
        case 30:
          setFilter(new ResidualMagnitudeFilter(AbstractFilter::Points |
              AbstractFilter::Measures, 1));
          break;
        case 31:
          setFilter(new SampleFilter(AbstractFilter::Points |
              AbstractFilter::Measures, 1));
          break;
        case 32:
          setFilter(new SampleResidualFilter(AbstractFilter::Points |
              AbstractFilter::Measures, 1));
          break;
        case 33:
          setFilter(new SampleShiftFilter(AbstractFilter::Points |
              AbstractFilter::Measures, 1));
          break;
      }
    }

    emit sizeChanged();
    emit filterChanged();
  }
}

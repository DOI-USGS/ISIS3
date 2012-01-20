#include "IsisDebug.h"

#include "PointMeasureFilterSelector.h"

#include <algorithm>
#include <iostream>

#include <QComboBox>
#include <QHBoxLayout>

#include "AbstractFilter.h"
#include "CubeSerialNumberFilter.h"
#include "ChooserNameFilter.h"
#include "GoodnessOfFitFilter.h"
#include "LineFilter.h"
#include "LineResidualFilter.h"
#include "PointEditLockedFilter.h"
#include "MeasureCountFilter.h"
#include "MeasureIgnoredFilter.h"
#include "MeasureTypeFilter.h"
#include "PointIdFilter.h"
#include "PointIgnoredFilter.h"
#include "PointTypeFilter.h"
#include "ResidualMagnitudeFilter.h"
#include "SampleFilter.h"
#include "SampleResidualFilter.h"


using std::swap;


namespace Isis
{
  namespace CnetViz
  {
    PointMeasureFilterSelector::PointMeasureFilterSelector()
    {
      createSelector();
    }


    PointMeasureFilterSelector::PointMeasureFilterSelector(
        const PointMeasureFilterSelector & other)
    {
      createSelector();
      getSelector()->setCurrentIndex(other.getSelector()->currentIndex());
      if (other.getFilter())
        setFilter(other.getFilter()->clone());
    }


    PointMeasureFilterSelector::~PointMeasureFilterSelector()
    {
    }


    PointMeasureFilterSelector & PointMeasureFilterSelector::operator=(
        const PointMeasureFilterSelector & other)
    {
      *((AbstractFilterSelector *) this) = other;
      return *this;
    }


    void PointMeasureFilterSelector::createSelector()
    {
      AbstractFilterSelector::createSelector();

      getSelector()->addItem("Chooser Name");
      getSelector()->addItem("Edit Locked Points");
      getSelector()->addItem("Ignored Points");
      getSelector()->addItem("Point Id");
      getSelector()->addItem("Point Type");
      getSelector()->addItem("Measure Count");
      getSelector()->insertSeparator(getSelector()->count());
      getSelector()->addItem("Cube Serial Number");
      getSelector()->addItem("Goodness Of Fit");
      getSelector()->addItem("Ignored Measures");
      getSelector()->addItem("Line");
      getSelector()->addItem("Line Residual");
      getSelector()->addItem("Measure Type");
      getSelector()->addItem("Residual Magnitude");
      getSelector()->addItem("Sample");
      getSelector()->addItem("Sample Residual");
    }


    void PointMeasureFilterSelector::changeFilter(int index)
    {
      deleteFilter();

      if (index != 0)
      {
        switch (index)
        {
          case 2:
            setFilter(new ChooserNameFilter(AbstractFilter::Points));
            break;
          case 3:
            setFilter(new PointEditLockedFilter(AbstractFilter::Points));
            break;
          case 4:
            setFilter(new PointIgnoredFilter(AbstractFilter::Points));
            break;
          case 5:
            setFilter(new PointIdFilter(AbstractFilter::Points));
            break;
          case 6:
            setFilter(new PointTypeFilter(AbstractFilter::Points));
            break;
          case 7:
            setFilter(new MeasureCountFilter(AbstractFilter::Points));
            break;
          case 9:
            setFilter(new CubeSerialNumberFilter(AbstractFilter::Points |
                AbstractFilter::Measures, 1));
            break;
          case 10:
            setFilter(new GoodnessOfFitFilter(AbstractFilter::Points |
                AbstractFilter::Measures, 1));
            break;
          case 11:
            setFilter(new MeasureIgnoredFilter(AbstractFilter::Points |
                AbstractFilter::Measures, 1));
            break;
          case 12:
            setFilter(new LineFilter(AbstractFilter::Points |
                AbstractFilter::Measures, 1));
            break;
          case 13:
            setFilter(new LineResidualFilter(AbstractFilter::Points |
                AbstractFilter::Measures, 1));
            break;
          case 14:
            setFilter(new MeasureTypeFilter(AbstractFilter::Points |
                AbstractFilter::Measures, 1));
            break;
          case 15:
            setFilter(new ResidualMagnitudeFilter(AbstractFilter::Points |
                AbstractFilter::Measures, 1));
            break;
          case 16:
            setFilter(new SampleFilter(AbstractFilter::Points |
                AbstractFilter::Measures, 1));
            break;
          case 17:
            setFilter(new SampleResidualFilter(AbstractFilter::Points |
                AbstractFilter::Measures, 1));
            break;
        }
      }

      emit sizeChanged();
      emit filterChanged();
    }
  }
}


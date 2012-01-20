#include "IsisDebug.h"

#include <algorithm>

#include "ImagePointFilterSelector.h"

#include <QComboBox>
#include <QHBoxLayout>

#include "AbstractFilter.h"
#include "ChooserNameFilter.h"
#include "ImageIdFilter.h"
#include "GoodnessOfFitFilter.h"
#include "LineFilter.h"
#include "LineResidualFilter.h"
#include "MeasureIgnoredFilter.h"
#include "MeasureTypeFilter.h"
#include "PointEditLockedFilter.h"
#include "PointIgnoredFilter.h"
#include "PointIdFilter.h"
#include "PointTypeFilter.h"
#include "ResidualMagnitudeFilter.h"
#include "SampleFilter.h"
#include "SampleResidualFilter.h"


using std::swap;


namespace Isis
{
  namespace CnetViz
  {
    ImagePointFilterSelector::ImagePointFilterSelector()
    {
      nullify();
      createSelector();
    }


    ImagePointFilterSelector::ImagePointFilterSelector(const ImagePointFilterSelector & other)
    {
      createSelector();
      getSelector()->setCurrentIndex(other.getSelector()->currentIndex());
      if (other.getFilter())
        setFilter(other.getFilter()->clone());
    }


    ImagePointFilterSelector::~ImagePointFilterSelector()
    {
    }


    ImagePointFilterSelector & ImagePointFilterSelector::operator=(
      const ImagePointFilterSelector & other)
    {
      *((AbstractFilterSelector *) this) = other;
      return *this;
    }


    void ImagePointFilterSelector::createSelector()
    {
      AbstractFilterSelector::createSelector();

      getSelector()->addItem("Image ID");
      getSelector()->insertSeparator(getSelector()->count());
      getSelector()->addItem("Chooser Name");
      getSelector()->addItem("Edit Locked Points");
      getSelector()->addItem("Ignored Points");
      getSelector()->addItem("Point Id");
      getSelector()->addItem("Point Type");
      getSelector()->insertSeparator(getSelector()->count());
      getSelector()->addItem("Goodness Of Fit");
      getSelector()->addItem("Ignored Measures");
      getSelector()->addItem("Line");
      getSelector()->addItem("Line Residual");
      getSelector()->addItem("Measure Type");
      getSelector()->addItem("Residual Magnitude");
      getSelector()->addItem("Sample");
      getSelector()->addItem("Sample Residual");
    }


    void ImagePointFilterSelector::changeFilter(int index)
    {
      deleteFilter();

      if (index != 0)
      {
        switch (index)
        {
          case 2:
            setFilter(new ImageIdFilter(AbstractFilter::Images));
            break;
          case 4:
            setFilter(new ChooserNameFilter(AbstractFilter::Images |
                AbstractFilter::Points, 1));
            break;
          case 5:
            setFilter(new PointEditLockedFilter(AbstractFilter::Images |
                AbstractFilter::Points, 1));
            break;
          case 6:
            setFilter(new PointIgnoredFilter(AbstractFilter::Images |
                AbstractFilter::Points, 1));
            break;
          case 7:
            setFilter(new PointIdFilter(AbstractFilter::Images |
                AbstractFilter::Points, 1));
            break;
          case 8:
            setFilter(new PointTypeFilter(AbstractFilter::Images |
                AbstractFilter::Points, 1));
            break;
          case 10:
            setFilter(new GoodnessOfFitFilter(AbstractFilter::Images, 1));
            break;
          case 11:
            setFilter(new MeasureIgnoredFilter(AbstractFilter::Images, 1));
            break;
          case 12:
            setFilter(new LineFilter(AbstractFilter::Images, 1));
            break;
          case 13:
            setFilter(new LineResidualFilter(AbstractFilter::Images, 1));
            break;
          case 14:
            setFilter(new MeasureTypeFilter(AbstractFilter::Images, 1));
            break;
          case 15:
            setFilter(new ResidualMagnitudeFilter(AbstractFilter::Images, 1));
            break;
          case 16:
            setFilter(new SampleFilter(AbstractFilter::Images, 1));
            break;
          case 17:
            setFilter(new SampleResidualFilter(AbstractFilter::Images, 1));
            break;
        }
      }

      emit sizeChanged();
      emit filterChanged();
    }
  }
}


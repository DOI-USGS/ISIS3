#include "IsisDebug.h"

#include <algorithm>

#include "SerialFilterSelector.h"

#include <QComboBox>
#include <QHBoxLayout>

#include "AbstractFilter.h"
#include "ChooserNameFilter.h"
#include "CubeSerialNumberFilter.h"
#include "GoodnessOfFitFilter.h"
#include "LineResidualFilter.h"
#include "MeasureIgnoredFilter.h"
#include "PointEditLockedFilter.h"
#include "PointIgnoredFilter.h"
#include "PointIdFilter.h"
#include "ResidualMagnitudeFilter.h"
#include "SampleResidualFilter.h"


using std::swap;


namespace Isis
{
  SerialFilterSelector::SerialFilterSelector()
  {
    nullify();
    createSelector();
  }


  SerialFilterSelector::SerialFilterSelector(const SerialFilterSelector & other)
  {
    createSelector();
    getSelector()->setCurrentIndex(other.getSelector()->currentIndex());
    if (other.getFilter())
      setFilter(other.getFilter()->clone());
  }


  SerialFilterSelector::~SerialFilterSelector()
  {
  }


  SerialFilterSelector & SerialFilterSelector::operator=(
    const SerialFilterSelector & other)
  {
    *((AbstractFilterSelector *) this) = other;
    return *this;
  }


  void SerialFilterSelector::createSelector()
  {
    AbstractFilterSelector::createSelector();

    getSelector()->addItem("Cube Serial Number");
    getSelector()->insertSeparator(getSelector()->count());
    getSelector()->addItem("Chooser Name");
    getSelector()->addItem("Edit Locked Points");
    getSelector()->addItem("Ignored Points");
    getSelector()->addItem("Point Id");
    getSelector()->insertSeparator(getSelector()->count());
    getSelector()->addItem("Goodness Of Fit");
    getSelector()->addItem("Ignored Measures");
    getSelector()->addItem("Line Residual");
    getSelector()->addItem("Residual Magnitude");
    getSelector()->addItem("Sample Residual");
  }


  void SerialFilterSelector::changeFilter(int index)
  {
    deleteFilter();

    if (index != 0)
    {
      switch (index)
      {
        case 2:
          setFilter(new CubeSerialNumberFilter(AbstractFilter::Images));
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
        case 9:
          setFilter(new GoodnessOfFitFilter(AbstractFilter::Images, 1));
          break;
        case 10:
          setFilter(new MeasureIgnoredFilter(AbstractFilter::Images, 1));
          break;
        case 11:
          setFilter(new LineResidualFilter(AbstractFilter::Images, 1));
          break;
        case 12:
          setFilter(new ResidualMagnitudeFilter(AbstractFilter::Images, 1));
          break;
        case 13:
          setFilter(new SampleResidualFilter(AbstractFilter::Images, 1));
          break;
      }
    }

    emit sizeChanged();
    emit filterChanged();
  }
}

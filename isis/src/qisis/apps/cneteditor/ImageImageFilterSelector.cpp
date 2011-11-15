#include "IsisDebug.h"

#include "ImageImageFilterSelector.h"

#include <algorithm>
#include <iostream>

#include <QComboBox>
#include <QHBoxLayout>
#include <QIcon>
#include <QPushButton>

#include "AbstractFilter.h"


using std::cerr;
using std::swap;


namespace Isis
{
  namespace CnetViz
  {
    ImageImageFilterSelector::ImageImageFilterSelector()
    {
      nullify();
      createSelector();
    }


    ImageImageFilterSelector::ImageImageFilterSelector(
      const ImageImageFilterSelector & other)
    {
      createSelector();
      getSelector()->setCurrentIndex(other.getSelector()->currentIndex());
      if (other.getFilter())
        setFilter(other.getFilter()->clone());
    }


    ImageImageFilterSelector::~ImageImageFilterSelector()
    {
    }


    ImageImageFilterSelector & ImageImageFilterSelector::operator=(
      const ImageImageFilterSelector & other)
    {
      *((AbstractFilterSelector *) this) = other;
      return *this;
    }


    void ImageImageFilterSelector::createSelector()
    {
      AbstractFilterSelector::createSelector();

  //     selector->addItem("Point Id");
    }


    void ImageImageFilterSelector::changeFilter(int index)
    {
      deleteFilter();

      if (index != 0)
      {
  //       switch (index)
  //       {
  //         case 1:
  //           filter = new PointIdFilter;
  //           break;
  //         case 2:
  //           filter = new PointIdFilter;
  //           break;
  //       }
  //
      }
    }
  }
}

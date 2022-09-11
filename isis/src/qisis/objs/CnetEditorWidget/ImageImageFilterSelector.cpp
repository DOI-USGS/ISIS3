/** This is free and unencumbered software released into the public domain.

The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */

#include "ImageImageFilterSelector.h"

#include <algorithm>
#include <iostream>

#include <QComboBox>
#include <QHBoxLayout>
#include <QIcon>
#include <QPushButton>

#include "AbstractFilter.h"


using std::swap;


namespace Isis {
  ImageImageFilterSelector::ImageImageFilterSelector() {
    nullify();
    createSelector();
  }


  ImageImageFilterSelector::ImageImageFilterSelector(
    const ImageImageFilterSelector &other) {
    createSelector();
    getSelector()->setCurrentIndex(other.getSelector()->currentIndex());
    if (other.getFilter())
      setFilter(other.getFilter()->clone());
  }


  ImageImageFilterSelector::~ImageImageFilterSelector() {
  }


  ImageImageFilterSelector &ImageImageFilterSelector::operator=(
    const ImageImageFilterSelector &other) {
    *((AbstractFilterSelector *) this) = other;
    return *this;
  }


  void ImageImageFilterSelector::createSelector() {
    AbstractFilterSelector::createSelector();

    //     selector->addItem("Point Id");
  }


  void ImageImageFilterSelector::changeFilter(int index) {
    deleteFilter();

    if (index != 0) {
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

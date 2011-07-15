#include "IsisDebug.h"

#include "ConnectionFilterSelector.h"

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
  ConnectionFilterSelector::ConnectionFilterSelector()
  {
    nullify();
    createSelector();
  }


  ConnectionFilterSelector::ConnectionFilterSelector(
    const ConnectionFilterSelector & other)
  {
    createSelector();
    getSelector()->setCurrentIndex(other.getSelector()->currentIndex());
    if (other.getFilter())
      setFilter(other.getFilter()->clone());
  }


  ConnectionFilterSelector::~ConnectionFilterSelector()
  {
  }


  ConnectionFilterSelector & ConnectionFilterSelector::operator=(
    const ConnectionFilterSelector & other)
  {
    *((AbstractFilterSelector *) this) = other;
    return *this;
  }


  void ConnectionFilterSelector::createSelector()
  {
    AbstractFilterSelector::createSelector();

//     selector->addItem("Point Id");
  }


  void ConnectionFilterSelector::changeFilter(int index)
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

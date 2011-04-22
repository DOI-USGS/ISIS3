#include "IsisDebug.h"

#include "AbstractFilter.h"

#include <QButtonGroup>
#include <QHBoxLayout>
#include <QRadioButton>


namespace Isis
{
  AbstractFilter::AbstractFilter()
  {
  }


  AbstractFilter::~AbstractFilter()
  {
  }


  void AbstractFilter::nullify()
  {
    mainLayout = NULL;
    buttonGroup = NULL;
  }


  void AbstractFilter::createWidget()
  {
    QRadioButton * inclusiveButton = new QRadioButton("Inclusive");
    QRadioButton * exclusiveButton = new QRadioButton("Exclusive");

    QButtonGroup * group = new QButtonGroup;
    connect(group, SIGNAL(buttonClicked(int)), this, SIGNAL(filterChanged()));
    group->addButton(inclusiveButton, 0);
    group->addButton(exclusiveButton, 1);

    mainLayout = new QHBoxLayout;
    mainLayout->addWidget(inclusiveButton);
    mainLayout->addWidget(exclusiveButton);
    setLayout(mainLayout);

    inclusiveButton->click();
  }


  bool AbstractFilter::inclusive() const
  {
    return buttonGroup->checkedId() == 0;
  }
}

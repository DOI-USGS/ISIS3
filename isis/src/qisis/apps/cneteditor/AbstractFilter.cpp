#include "IsisDebug.h"

#include <iostream>
#include <limits>

#include "AbstractFilter.h"

#include <QButtonGroup>
#include <QHBoxLayout>
#include <QLabel>
#include <QMargins>
#include <QRadioButton>
#include <QReadWriteLock>
#include <QSpinBox>
#include <QWriteLocker>


using std::cerr;


namespace Isis
{
  AbstractFilter::AbstractFilter(int minimumForImageSuccess) :
      minForImageSuccess(minimumForImageSuccess)
  {
    lock = NULL;
    lock = new QReadWriteLock;
  }


  AbstractFilter::~AbstractFilter()
  {
  }


  void AbstractFilter::nullify()
  {
    mainLayout = NULL;
    inclusiveExclusiveGroup = NULL;
  }


  void AbstractFilter::createWidget()
  {
    QRadioButton * inclusiveButton = new QRadioButton("Inclusive");
    QRadioButton * exclusiveButton = new QRadioButton("Exclusive");

    inclusiveExclusiveGroup = new QButtonGroup;
    connect(inclusiveExclusiveGroup, SIGNAL(buttonClicked(int)),
        this, SIGNAL(filterChanged()));
    inclusiveExclusiveGroup->addButton(inclusiveButton, 0);
    inclusiveExclusiveGroup->addButton(exclusiveButton, 1);
    
    mainLayout = new QHBoxLayout;
    QMargins margins = mainLayout->contentsMargins();
    margins.setTop(0);
    margins.setBottom(0);
    mainLayout->setContentsMargins(margins);
    mainLayout->addWidget(inclusiveButton);
    mainLayout->addWidget(exclusiveButton);
    if (minForImageSuccess != -1)
    {
      QLabel * label = new QLabel("Min Count: ");
      QSpinBox * spinBox = new QSpinBox;
      spinBox->setRange(1, std::numeric_limits< int >::max());
      spinBox->setValue(1);  // FIXME: QSettings should handle this
      connect(spinBox, SIGNAL(valueChanged(int)),
          this, SLOT(updateMinForImageSuccess(int)));
      mainLayout->addWidget(label);
      mainLayout->addWidget(spinBox);
    }

    setLayout(mainLayout);

    // FIXME: QSettings should handle this
    inclusiveButton->click();
  }


  bool AbstractFilter::inclusive() const
  {
    return inclusiveExclusiveGroup->checkedId() == 0;
  }
  

  bool AbstractFilter::evaluate(const ControlPoint * point,
      bool (ControlPoint::*meth)() const) const
  {
    bool evaluation = true;
    
    if (canFilterPoints())
      evaluation = !((point->*meth)() ^ inclusive());
      
    return evaluation;
  }
  
  
  bool AbstractFilter::evaluate(const ControlMeasure * measure,
      bool (ControlMeasure::*meth)() const) const
  {
    bool evaluation = true;
    
    if (canFilterMeasures())
      evaluation = !((measure->*meth)() ^ inclusive());
    
    return evaluation;
  }
  
  
  QString AbstractFilter::getImageDescription() const
  {
    return "have at least " + QString::number(getMinForImageSuccess()) + " ";
  }


  QString AbstractFilter::getPointDescription() const
  {
    return QString();
  }

  
  QString AbstractFilter::getMeasureDescription() const
  {
    return QString();
  }


  void AbstractFilter::updateMinForImageSuccess(int newMin)
  {
    QWriteLocker locker(lock);
    minForImageSuccess = newMin;
    locker.unlock();
    emit filterChanged();
  }
}

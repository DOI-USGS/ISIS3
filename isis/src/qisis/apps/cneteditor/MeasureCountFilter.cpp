#include "IsisDebug.h"

#include "MeasureCountFilter.h"

#include <iostream>

#include <QButtonGroup>
#include <QFont>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QMargins>
#include <QRadioButton>
#include <QSpinBox>

#include "ControlCubeGraphNode.h"
#include "ControlMeasure.h"
#include "ControlPoint.h"


using std::cerr;


namespace Isis
{
  namespace CnetViz
  {
    MeasureCountFilter::MeasureCountFilter(
        AbstractFilter::FilterEffectivenessFlag flag,
        int minimumForSuccess) : AbstractFilter(flag, minimumForSuccess)
    {
      init();
      createWidget();
    }


    MeasureCountFilter::MeasureCountFilter(const MeasureCountFilter & other)
        : AbstractFilter(other)
    {
      init();
      createWidget();
      
      count = other.count;
      minimum = other.minimum;
      countSpinBox->setValue(other.countSpinBox->value());
      minMaxGroup->button(other.minMaxGroup->checkedId())->click();
    }



    MeasureCountFilter::~MeasureCountFilter()
    {
    }


    void MeasureCountFilter::init()
    {
      minMaxGroup = NULL;
      countSpinBox = NULL;
      count = 0;
      minimum = true;
    }


    void MeasureCountFilter::createWidget()
    {
      QFont minMaxFont("SansSerif", 9);
      QRadioButton * minButton = new QRadioButton("Minimum");
      minButton->setFont(minMaxFont);
      QRadioButton * maxButton = new QRadioButton("Maximum");
      maxButton->setFont(minMaxFont);
      
      minMaxGroup = new QButtonGroup;
      connect(minMaxGroup, SIGNAL(buttonClicked(int)),
              this, SLOT(updateMinMax(int)));
      minMaxGroup->addButton(minButton, 0);
      minMaxGroup->addButton(maxButton, 1);
      
      minButton->click();
      
      countSpinBox = new QSpinBox;
      countSpinBox->setRange(0, std::numeric_limits< int >::max());
      countSpinBox->setValue(count);
      connect(countSpinBox, SIGNAL(valueChanged(int)),
              this, SLOT(updateMeasureCount(int)));
      
      // hide inclusive and exclusive buttons,
      // and add spinbox for min measure count
      getInclusiveExclusiveLayout()->itemAt(0)->widget()->setVisible(false);
      getInclusiveExclusiveLayout()->itemAt(1)->widget()->setVisible(false);
      getInclusiveExclusiveLayout()->addWidget(minButton);
      getInclusiveExclusiveLayout()->addWidget(maxButton);
      getInclusiveExclusiveLayout()->addSpacing(8);
      getInclusiveExclusiveLayout()->addWidget(countSpinBox);
    }


    bool MeasureCountFilter::evaluate(const ControlCubeGraphNode * node) const
    {
      return AbstractFilter::evaluateImageFromPointFilter(node);
    }


    bool MeasureCountFilter::evaluate(const ControlPoint * point) const
    {
      return (point->getMeasures().size() >= count && minimum) ||
            (point->getMeasures().size() <= count && !minimum);
    }


    bool MeasureCountFilter::evaluate(const ControlMeasure * measure) const
    {
      return true;
    }


    AbstractFilter * MeasureCountFilter::clone() const
    {
      return new MeasureCountFilter(*this);
    }


    QString MeasureCountFilter::getImageDescription() const
    {
      QString description = AbstractFilter::getImageDescription();

      if (getMinForSuccess() == 1)
      {
        description += "point that ";
        
        if (!inclusive())
          description += "doesn't have";
        
        description += "has ";
      }
      else
      {
        description += "points that ";
        
        if (!inclusive())
          description += "don't ";
        
        description += "have ";
      }
      
      description += "at ";
      
      if (minimum)
        description += "least ";
      else
        description += "most ";
      
      description += QString::number(count) + " measures";

      return description;
    }


    QString MeasureCountFilter::getPointDescription() const
    {
      QString description;
      
      if (!inclusive())
        description = "don't ";
      
      description = "have at ";
      
      if (minimum)
        description += "least ";
      else
        description += "most ";
      
      description += QString::number(count) + " measures";
      
      return description;
    }
    
    
    void MeasureCountFilter::updateMinMax(int buttonId)
    {
      minimum = (buttonId == 0);
      emit filterChanged();
    }
    
    
    void MeasureCountFilter::updateMeasureCount(int newCount)
    {
      count = newCount;
      emit filterChanged();
    }
  }
}

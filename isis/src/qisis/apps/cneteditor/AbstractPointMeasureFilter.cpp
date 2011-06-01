#include "IsisDebug.h"

#include "AbstractPointMeasureFilter.h"

#include <iostream>

#include <QComboBox>
#include <QHBoxLayout>
#include <QLabel>

#include "ControlCubeGraphNode.h"
#include "ControlPoint.h"
#include "ControlMeasure.h"


using std::cerr;


namespace Isis
{
  AbstractPointMeasureFilter::AbstractPointMeasureFilter(
      int minimumForImageSuccess) : AbstractFilter(minimumForImageSuccess)
  {
  }


  AbstractPointMeasureFilter::~AbstractPointMeasureFilter()
  {
  }
  
  
  bool AbstractPointMeasureFilter::canFilterImages() const
  {
    return getMinForImageSuccess() != -1;
  }
  
  
  bool AbstractPointMeasureFilter::canFilterPoints() const
  {
    return effectiveness != AbstractPointMeasureFilter::MeasuresOnly;
  }
  
  
  bool AbstractPointMeasureFilter::canFilterMeasures() const
  {
    return effectiveness != AbstractPointMeasureFilter::PointsOnly;
  }
  
  
  bool AbstractPointMeasureFilter::evaluate(
      const ControlCubeGraphNode * node) const
  {
    bool evaluation = true;
    
    if (canFilterImages())
    {
      int passedMeasures = 0;
      int passedPoints = 0;
      
      QList< ControlMeasure * > measures = node->getMeasures();
      foreach (ControlMeasure * measure, measures)
      {
        ASSERT(measure);
        if (measure && evaluate(measure))
          passedMeasures++;
        
        ControlPoint * point = measure->Parent();
        ASSERT(point);
        if (point && evaluate(point))
          passedPoints++;
      }
      
      bool pointsPass = true;
      bool measuresPass = true;
      
      if (effectiveness != AbstractPointMeasureFilter::MeasuresOnly)
      {
        pointsPass = passedPoints >= getMinForImageSuccess();
      }
      
      if (effectiveness != AbstractPointMeasureFilter::PointsOnly)
      {
        measuresPass = passedMeasures >= getMinForImageSuccess();
      }
    
      evaluation = pointsPass && measuresPass;
    }
    
    return evaluation;
  }
  
  
  void AbstractPointMeasureFilter::createWidget()
  {
    AbstractFilter::createWidget();
    
    QLabel * label = new QLabel("Effect: ");
    label->setFont(QFont("SansSerif", 10, QFont::DemiBold));

    QComboBox * combo = new QComboBox;
    combo->addItem("points only");
    combo->addItem("measuresOnly");
    combo->addItem("both");
    connect(combo, SIGNAL(currentIndexChanged(int)),
        this, SLOT(changeEffectiveness(int)));
        
    mainLayout->addWidget(label);
    mainLayout->addWidget(combo);
    
    // FIXME: QSettings should handle this
    combo->setCurrentIndex(1);
    combo->setCurrentIndex(0);
  }
  
  
  void AbstractPointMeasureFilter::changeEffectiveness(int button)
  {
    effectiveness = (Effectiveness) button;
    emit filterChanged();
  }
}

#include "IsisDebug.h"

#include "IgnoredFilter.h"

#include <QHBoxLayout>

#include "ControlCubeGraphNode.h"
#include "ControlMeasure.h"
#include "ControlPoint.h"


namespace Isis
{
  IgnoredFilter::IgnoredFilter(int minimumForImageSuccess) :
      AbstractPointMeasureFilter(minimumForImageSuccess)
  {
    nullify();
    createWidget();
  }


  IgnoredFilter::~IgnoredFilter()
  {
  }


  bool IgnoredFilter::evaluate(const ControlPoint * point) const
  {
    if (effectiveness == AbstractPointMeasureFilter::MeasuresOnly)
      return true;
      
    return !(point->IsIgnored() ^ inclusive());
  }


  bool IgnoredFilter::evaluate(const ControlMeasure * measure) const
  {
    if (effectiveness == AbstractPointMeasureFilter::PointsOnly)
      return true;
      
    return !(measure->IsIgnored() ^ inclusive());
  }


  bool IgnoredFilter::evaluate(const ControlCubeGraphNode * node) const
  {
    bool evaluation = true;
    
    if (minForImageSuccess != -1)
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
        pointsPass = passedPoints >= minForImageSuccess;
      }
      
      if (effectiveness != AbstractPointMeasureFilter::PointsOnly)
      {
        measuresPass = passedMeasures >= minForImageSuccess;
      }
    
      evaluation = pointsPass && measuresPass;
    }
    
    return evaluation;
  }
  
  
  QString IgnoredFilter::getDescription() const
  {
    QString description;
    
    if (minForImageSuccess != -1)
    {
      description += "contain at least ";
      description += QString::number(minForImageSuccess);
      switch (effectiveness)
      {
        case AbstractPointMeasureFilter::PointsOnly:
          description += " points ";
          break;
        case AbstractPointMeasureFilter::MeasuresOnly:
          description += " measures ";
          break;          
        case AbstractPointMeasureFilter::Both:
          description += " points & measures ";
          break;
      }
      description += "that ";
    }
    
    description += "are ";
     
    if (!inclusive())
      description += "not ";
    
    description += "ignored";
    
    return description;
  }
}

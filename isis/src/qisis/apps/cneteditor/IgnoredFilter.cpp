#include "IsisDebug.h"

#include "IgnoredFilter.h"

#include <iostream>

#include <QHBoxLayout>

#include "ControlCubeGraphNode.h"
#include "ControlMeasure.h"
#include "ControlPoint.h"


using std::cerr;


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
  
  
  bool IgnoredFilter::canFilterImages() const
  {
    return minForImageSuccess != -1;
  }
  
  
  bool IgnoredFilter::canFilterPoints() const
  {
    return effectiveness != AbstractPointMeasureFilter::MeasuresOnly;
  }
  
  
  bool IgnoredFilter::canFilterMeasures() const
  {
    return effectiveness != AbstractPointMeasureFilter::PointsOnly;
  }


  bool IgnoredFilter::evaluate(const ControlCubeGraphNode * node) const
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
  

  bool IgnoredFilter::evaluate(const ControlPoint * point) const
  {
    bool evaluation = true;
    
    if (canFilterPoints())
      evaluation = !(point->IsIgnored() ^ inclusive());
      
    return evaluation;
  }


  bool IgnoredFilter::evaluate(const ControlMeasure * measure) const
  {
    bool evaluation = true;
    
    if (canFilterMeasures())
      evaluation = !(measure->IsIgnored() ^ inclusive());
    
    return evaluation;
  }

  
  QString IgnoredFilter::getDescription() const
  {
    QString description = "<font color=black>that</font> are ";
     
    if (!inclusive())
      description += "not ";
    
    description += "ignored";
    
    return description;
  }
}

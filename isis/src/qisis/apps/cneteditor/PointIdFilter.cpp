#include "IsisDebug.h"

#include "PointIdFilter.h"

#include <iostream>

#include <QHBoxLayout>
#include <QLineEdit>

#include "ControlCubeGraphNode.h"
#include "ControlMeasure.h"
#include "ControlPoint.h"


using std::cerr;


namespace Isis
{
  PointIdFilter::PointIdFilter(int minimumForImageSuccess) :
      AbstractFilter(minimumForImageSuccess)
  {
    nullify();
    createWidget();
  }


  PointIdFilter::~PointIdFilter()
  {
  }


  void PointIdFilter::nullify()
  {
    AbstractFilter::nullify();

    lineEdit = NULL;
  }


  void PointIdFilter::createWidget()
  {
    AbstractFilter::createWidget();

    lineEdit = new QLineEdit;
    lineEdit->setMinimumWidth(200);
    connect(lineEdit, SIGNAL(textChanged(QString)),
        this, SIGNAL(filterChanged()));
    mainLayout->addWidget(lineEdit);
  }


  bool PointIdFilter::canFilterImages() const
  {
    return minForImageSuccess != -1;
  }
  
  
  bool PointIdFilter::canFilterPoints() const
  {
    return true;
  }
  
  
  bool PointIdFilter::canFilterMeasures() const
  {
    return false;
  }


  bool PointIdFilter::evaluate(const ControlPoint * point) const
  {
    bool evaluation = true;
    
    QString lineEditText = lineEdit->text();
    if (lineEditText.size() >= 1)
    {
      bool match = ((QString) point->GetId()).contains(
          lineEditText, Qt::CaseInsensitive);
      evaluation = !(inclusive() ^ match);
      
      //  inclusive() | match | evaluation
      //  ------------|-------|-----------
      //       T      |   T   |   T
      //  ------------|-------|-----------
      //       T      |   F   |   F
      //  ------------|-------|-----------
      //       F      |   T   |   F
      //  ------------|-------|-----------
      //       F      |   F   |   T
      //  ------------|-------|-----------
    }
    
    return evaluation;
  }


  bool PointIdFilter::evaluate(const ControlMeasure * measure) const
  {
    return true;
  }


  bool PointIdFilter::evaluate(const ControlCubeGraphNode * node) const
  {
    bool evaluation = true;
    
    if (canFilterImages())
    {
      int passedPoints = 0;
      
      QList< ControlMeasure * > measures = node->getMeasures();
      foreach (ControlMeasure * measure, measures)
      {
        ASSERT(measure);
        ControlPoint * point = measure->Parent();
        ASSERT(point);
        if (point && evaluate(point))
          passedPoints++;
      }
      
      evaluation = passedPoints >= minForImageSuccess;
    }
    
    return evaluation;
  }
  
  
  QString PointIdFilter::getDescription() const
  {
    QString description = "<font color=black>with</font> IDs ";
    
    if (inclusive())
      description += "containing ";
    else
      description += "that don't contain ";
    
    ASSERT(lineEdit);
    description += "\"" + lineEdit->text() + "\"";
    
    return description;
  }
}

#include "IsisDebug.h"

#include "ChooserNameFilter.h"

#include <QHBoxLayout>
#include <QLineEdit>

#include "ControlCubeGraphNode.h"
#include "ControlMeasure.h"
#include "ControlPoint.h"


namespace Isis
{
  ChooserNameFilter::ChooserNameFilter(int minimumForImageSuccess) :
      AbstractFilter(minimumForImageSuccess)
  {
    nullify();
    createWidget();
  }


  ChooserNameFilter::~ChooserNameFilter()
  {
  }


  void ChooserNameFilter::nullify()
  {
    AbstractFilter::nullify();

    lineEdit = NULL;
  }


  void ChooserNameFilter::createWidget()
  {
    AbstractFilter::createWidget();

    lineEdit = new QLineEdit;
    lineEdit->setMinimumWidth(200);
    connect(lineEdit, SIGNAL(textChanged(QString)),
        this, SIGNAL(filterChanged()));
    mainLayout->addWidget(lineEdit);
  }


  bool ChooserNameFilter::canFilterImages() const
  {
    return minForImageSuccess != -1;
  }
  
  
  bool ChooserNameFilter::canFilterPoints() const
  {
    return true;
  }
  
  
  bool ChooserNameFilter::canFilterMeasures() const
  {
    return false;
  }


  bool ChooserNameFilter::evaluate(const ControlPoint * point) const
  {
    bool evaluation = true;
    
    QString lineEditText = lineEdit->text();
    if (lineEditText.size() >= 1)
    {
      bool match = ((QString) point->GetChooserName()).contains(
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
  
  
  bool ChooserNameFilter::evaluate(const ControlMeasure * measure) const
  {
    return true;
  }


  bool ChooserNameFilter::evaluate(const ControlCubeGraphNode * node) const
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
  
  
  QString ChooserNameFilter::getDescription() const
  {
    QString description = "have chooser names ";
    
    if (inclusive())
      description += "containing ";
    else
      description += "that don't contain ";
    
    ASSERT(lineEdit);
    description += "\"" + lineEdit->text() + "\"";
    
    return description;
  }
}

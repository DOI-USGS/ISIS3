#include "IsisDebug.h"

#include "PointIdFilter.h"

#include <iostream>

#include <QHBoxLayout>
#include <QLineEdit>
#include <QReadLocker>
#include <QWriteLocker>

#include "ControlCubeGraphNode.h"
#include "ControlMeasure.h"
#include "ControlPoint.h"


using std::cerr;


namespace Isis
{
  PointIdFilter::PointIdFilter(AbstractFilter::FilterEffectivenessFlag flag,
      AbstractFilterSelector * parent, int minimumForSuccess) :
      AbstractFilter(flag, parent, minimumForSuccess)
  {
    nullify();
    lineEditText = new QString;
    createWidget();
  }


  PointIdFilter::~PointIdFilter()
  {
    if (lineEditText)
    {
      delete lineEditText;
      lineEditText = NULL;
    }
  }


  void PointIdFilter::nullify()
  {
    AbstractFilter::nullify();

    lineEdit = NULL;
    lineEditText = NULL;
  }


  void PointIdFilter::createWidget()
  {
    AbstractFilter::createWidget();

    lineEdit = new QLineEdit;
    lineEdit->setMinimumWidth(250);
    connect(lineEdit, SIGNAL(textChanged(QString)),
        this, SLOT(updateLineEditText(QString)));
    connect(lineEdit, SIGNAL(textChanged(QString)),
        this, SIGNAL(filterChanged()));
    getMainLayout()->addWidget(lineEdit);
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
      
      evaluation = passedPoints >= getMinForSuccess();
    }
    
    return evaluation;
  }
  
  
  bool PointIdFilter::evaluate(const ControlPoint * point) const
  {
    bool evaluation = true;
    
    
    QReadLocker locker(lock);
    QString text = *lineEditText;
    locker.unlock();
    
    
    if (text.size() >= 1)
    {
      bool match = ((QString) point->GetId()).contains(
          text, Qt::CaseInsensitive);
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
  
  
  QString PointIdFilter::getImageDescription() const
  {
    QString description = AbstractFilter::getImageDescription();
    description += "point";
    
    if (getMinForSuccess() == 1)
      description += " with it's ID ";
    else
      description += "s with IDs ";
    
    if (inclusive())
      description += "containing \"";
    else
      description += "not containing \"";
    
    QReadLocker locker(lock);
    description += *lineEditText;
    locker.unlock();
    
    description += "\"";
    
    return description;
  }
  
  
  QString PointIdFilter::getPointDescription() const
  {
    QString description = "have IDs ";
    
    if (inclusive())
      description += "containing \"";
    else
      description += "that don't contain \"";
    
    QReadLocker locker(lock);
    description += *lineEditText;
    locker.unlock();
    
    description += "\"";
    
    return description;
  }
  
  
  void PointIdFilter::updateLineEditText(QString newText)
  {
    QWriteLocker locker(lock);
    *lineEditText = newText;
  }
}

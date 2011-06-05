#include "IsisDebug.h"

#include "CubeSerialNumberFilter.h"

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
  CubeSerialNumberFilter::CubeSerialNumberFilter(
      AbstractFilter::FilterEffectivenessFlag flag,
      AbstractFilterSelector * parent, int minimumForSuccess) :
      AbstractFilter(flag, parent, minimumForSuccess)
  {
    nullify();
    lineEditText = new QString;
    createWidget();
  }


  CubeSerialNumberFilter::~CubeSerialNumberFilter()
  {
    if (lineEditText)
    {
      delete lineEditText;
      lineEditText = NULL;
    }
  }


  void CubeSerialNumberFilter::nullify()
  {
    AbstractFilter::nullify();

    lineEdit = NULL;
    lineEditText = NULL;
  }


  void CubeSerialNumberFilter::createWidget()
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


  bool CubeSerialNumberFilter::evaluate(const ControlCubeGraphNode * node) const
  {
    bool evaluation = true;
    
    
    QReadLocker locker(lock);
    QString text = *lineEditText;
    locker.unlock();
    
    
    if (text.size() >= 1)
    {
      bool match = ((QString) node->getSerialNumber()).contains(
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
  
  
  bool CubeSerialNumberFilter::evaluate(const ControlPoint * point) const
  {
    bool evaluation = true;
    
    if (canFilterPoints())
    {
      int passedMeasures = 0;
      
      QList< ControlMeasure * > measures = point->getMeasures();
      foreach (ControlMeasure * measure, measures)
      {
        ASSERT(measure);
        if (measure && evaluate(measure))
          passedMeasures++;
      }
      
      evaluation = passedMeasures >= getMinForSuccess();
    }
    
    return evaluation;
  }
  
  
  bool CubeSerialNumberFilter::evaluate(const ControlMeasure * measure) const
  {
    bool evaluation = true;
    
    
    QReadLocker locker(lock);
    QString text = *lineEditText;
    locker.unlock();
    
    
    if (text.size() >= 1)
    {
      bool match = ((QString) measure->GetCubeSerialNumber()).contains(
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
  
  
  QString CubeSerialNumberFilter::getImageDescription() const
  {
    return getMeasureDescription();
  }
  
  
  QString CubeSerialNumberFilter::getPointDescription() const
  {
    QString description = AbstractFilter::getImageDescription();
    
    if (getMinForSuccess() == 1)
      description += "measure with it's cube serial number ";
    else
      description += "measures with cube serial numbers ";
    
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
  
  
  QString CubeSerialNumberFilter::getMeasureDescription() const
  {
    QString description = "have cube serial numbers ";
    
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
  
  
  void CubeSerialNumberFilter::updateLineEditText(QString newText)
  {
    QWriteLocker locker(lock);
    *lineEditText = newText;
  }
}

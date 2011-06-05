#include "IsisDebug.h"

#include "ChooserNameFilter.h"

#include <QHBoxLayout>
#include <QLineEdit>
#include <QReadLocker>
#include <QWriteLocker>

#include "ControlCubeGraphNode.h"
#include "ControlMeasure.h"
#include "ControlPoint.h"


namespace Isis
{
  ChooserNameFilter::ChooserNameFilter(
      AbstractFilter::FilterEffectivenessFlag flag,
      AbstractFilterSelector * parent, int minimumForSuccess) :
      AbstractFilter(flag, parent, minimumForSuccess)
  {
    nullify();
    lineEditText = new QString;
    createWidget();
  }


  ChooserNameFilter::~ChooserNameFilter()
  {
    if (lineEditText)
    {
      delete lineEditText;
      lineEditText = NULL;
    }
  }


  void ChooserNameFilter::nullify()
  {
    AbstractFilter::nullify();

    lineEdit = NULL;
    lineEditText = NULL;
  }


  void ChooserNameFilter::createWidget()
  {
    AbstractFilter::createWidget();

    lineEdit = new QLineEdit;
    lineEdit->setMinimumWidth(200);
    connect(lineEdit, SIGNAL(textChanged(QString)),
        this, SLOT(updateLineEditText(QString)));
    connect(lineEdit, SIGNAL(textChanged(QString)),
        this, SIGNAL(filterChanged()));
    getMainLayout()->addWidget(lineEdit);
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
      
      evaluation = passedPoints >= getMinForSuccess();
    }
    
    return evaluation;
  }


  bool ChooserNameFilter::evaluate(const ControlPoint * point) const
  {
    bool evaluation = true;
    
    
    QReadLocker locker(lock);
    QString text = *lineEditText;
    locker.unlock();
    
    
    if (text.size() >= 1)
    {
      bool match = ((QString) point->GetChooserName()).contains(
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
  
  
  QString ChooserNameFilter::getImageDescription() const
  {
    QString description = AbstractFilter::getImageDescription();
    description += "point";
    
    if (getMinForSuccess() == 1)
      description += " with it's chooser name ";
    else
      description += "s with chooser names ";
    
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
  
  
  QString ChooserNameFilter::getPointDescription() const
  {
    QString description = "have chooser names ";
    
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
  
  
  void ChooserNameFilter::updateLineEditText(QString newText)
  {
    QWriteLocker locker(lock);
    *lineEditText = newText;
  }
}

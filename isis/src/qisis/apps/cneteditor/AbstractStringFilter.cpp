#include "IsisDebug.h"

#include "AbstractStringFilter.h"

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
  AbstractStringFilter::AbstractStringFilter(
      AbstractFilter::FilterEffectivenessFlag flag,
      AbstractFilterSelector * parent, int minimumForSuccess) :
      AbstractFilter(flag, parent, minimumForSuccess)
  {
  }


  AbstractStringFilter::~AbstractStringFilter()
  {
    if (lineEditText)
    {
      delete lineEditText;
      lineEditText = NULL;
    }
  }


  void AbstractStringFilter::nullify()
  {
    AbstractFilter::nullify();

    lineEdit = NULL;
    lineEditText = NULL;
  }


  void AbstractStringFilter::createWidget()
  {
    AbstractFilter::createWidget();

    lineEditText = new QString;
    
    lineEdit = new QLineEdit;
    lineEdit->setMinimumWidth(250);
    connect(lineEdit, SIGNAL(textChanged(QString)),
        this, SLOT(updateLineEditText(QString)));
    connect(lineEdit, SIGNAL(textChanged(QString)),
        this, SIGNAL(filterChanged()));
    
    QHBoxLayout * layout = new QHBoxLayout;
    QMargins margins = layout->contentsMargins();
    margins.setTop(0);
    margins.setBottom(0);
    layout->setContentsMargins(margins);
    layout->addWidget(lineEdit);
    layout->addStretch();
    
    getMainLayout()->addLayout(layout);
  }
  
  
  bool AbstractStringFilter::evaluate(QString str) const
  {
    bool evaluation = true;

    // multiple threads reading the lineEditText so lock it
    QReadLocker locker(lock);
    QString text = *lineEditText;
    locker.unlock();
    
    if (text.size() >= 1)
    {
      bool match = str.contains(text, Qt::CaseInsensitive);
      
      // inclusive() and match must either be both true or both false
      evaluation = !(inclusive() ^ match);
    }
    
    return evaluation;
  }


  QString AbstractStringFilter::descriptionSuffix() const
  {
    QString suffix;
    
    if (inclusive())
      suffix += "containing \"";
    else
      suffix += "not containing \"";
    
    QReadLocker locker(lock);
    suffix += *lineEditText;
    locker.unlock();
    
    suffix += "\"";
    return suffix;
  }

  
  void AbstractStringFilter::updateLineEditText(QString newText)
  {
    QWriteLocker locker(lock);
    *lineEditText = newText;
  }
}

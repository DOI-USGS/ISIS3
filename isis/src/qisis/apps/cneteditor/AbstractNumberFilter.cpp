#include "IsisDebug.h"

#include "AbstractNumberFilter.h"

#include <iostream>

#include <QButtonGroup>
#include <QFont>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QMargins>
#include <QRadioButton>
#include <QReadLocker>
#include <QWriteLocker>

#include "ControlCubeGraphNode.h"
#include "ControlMeasure.h"
#include "ControlPoint.h"


using std::cerr;


namespace Isis
{
  AbstractNumberFilter::AbstractNumberFilter(
      AbstractFilter::FilterEffectivenessFlag flag,
      AbstractFilterSelector * parent, int minimumForSuccess) :
      AbstractFilter(flag, parent, minimumForSuccess)
  {
  }


  AbstractNumberFilter::~AbstractNumberFilter()
  {
    if (lineEditText)
    {
      delete lineEditText;
      lineEditText = NULL;
    }
  }


  void AbstractNumberFilter::nullify()
  {
    AbstractFilter::nullify();

    greaterThanLessThan = NULL;
    lineEdit = NULL;
    lineEditText = NULL;
  }


  void AbstractNumberFilter::createWidget()
  {
    AbstractFilter::createWidget();
    
    QFont greaterThanLessThanFont("SansSerif", 9);
    
    QRadioButton * lessThanButton = new QRadioButton("<=");
    lessThanButton->setFont(greaterThanLessThanFont);
    QRadioButton * greaterThanButton = new QRadioButton(">=");
    greaterThanButton->setFont(greaterThanLessThanFont);

    greaterThanLessThan = new QButtonGroup;
    connect(greaterThanLessThan, SIGNAL(buttonClicked(int)),
        this, SIGNAL(filterChanged()));
    greaterThanLessThan->addButton(lessThanButton, 0);
    greaterThanLessThan->addButton(greaterThanButton, 1);
    
    // hide inclusive and exclusive buttons, and add greater than and less than
    // in the inclusiveExclusiveLayout.
    getInclusiveExclusiveLayout()->itemAt(0)->widget()->setVisible(false);
    getInclusiveExclusiveLayout()->itemAt(1)->widget()->setVisible(false);
    getInclusiveExclusiveLayout()->addWidget(lessThanButton);
    getInclusiveExclusiveLayout()->addWidget(greaterThanButton);
    
    lineEditText = new QString;
    
    lineEdit = new QLineEdit;
    lineEdit->setMinimumWidth(75);
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
    
    // FIXME: QSettings should handle this
    lessThanButton->click();
  }
  
  
  bool AbstractNumberFilter::evaluate(double number) const
  {
    bool evaluation = true;

    // multiple threads reading the lineEditText so lock it
    QReadLocker locker(lock);
    QString text = *lineEditText;
    locker.unlock();
    
    bool ok = false;
    double d = text.toDouble(&ok);
    if (ok)
      evaluation = !(inclusive() ^ lessThan() ^ (d <= number));
    
    return evaluation;
  }


  QString AbstractNumberFilter::descriptionSuffix() const
  {
    QString suffix;
    if (!inclusive())
      suffix += "not ";
    
    if (lessThan())
      suffix += "less than or equal to \"";
    else
      suffix += "greater than or equal to \"";
    
    QReadLocker locker(lock);
    suffix += *lineEditText;
    locker.unlock();
    
    suffix += "\"";
    return suffix;
  }
  
  
  bool AbstractNumberFilter::lessThan() const
  {
    return greaterThanLessThan->checkedId() == 0;
  }


  void AbstractNumberFilter::updateLineEditText(QString newText)
  {
    QWriteLocker locker(lock);
    *lineEditText = newText;
  }
}

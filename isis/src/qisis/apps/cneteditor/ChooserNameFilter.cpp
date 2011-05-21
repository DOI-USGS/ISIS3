#include "IsisDebug.h"

#include "ChooserNameFilter.h"

#include <QHBoxLayout>
#include <QLineEdit>

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
    return true;
  }
  
  
  QString ChooserNameFilter::getDescription() const
  {
    QString description;
    
    ASSERT(lineEdit);
    if (lineEdit)
    {
      description = "have chooser names ";
      if (inclusive())
        description += "containing ";
      else
        description += "that don't contain ";
      description += "\"" + lineEdit->text() + "\"";
    }
    
    return description;
  }
}

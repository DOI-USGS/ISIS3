#include "IsisDebug.h"

#include "PointIdFilter.h"

#include <QHBoxLayout>
#include <QLineEdit>

#include "ControlPoint.h"


namespace Isis
{
  PointIdFilter::PointIdFilter() : AbstractFilter()
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
    connect(lineEdit, SIGNAL(textChanged(QString)),
        this, SIGNAL(filterChanged()));
    mainLayout->insertWidget(0, lineEdit);
  }


  bool PointIdFilter::evaluate(ControlPoint const * point) const
  {
    if (inclusive())
      return ((QString) point->GetId()).startsWith(lineEdit->text());
    else
      return !((QString) point->GetId()).startsWith(lineEdit->text());
  }


  bool PointIdFilter::evaluate(ControlMeasure const * measure) const
  {
    return true;
  }


  bool PointIdFilter::evaluate(ControlCubeGraphNode const * node) const
  {
    return true;
  }
}
